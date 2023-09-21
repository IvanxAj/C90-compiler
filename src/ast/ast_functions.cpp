#include "ast/ast_functions.hpp"

// Three branches: type, declarator, compound statement
FunctionDefinition::FunctionDefinition(Specifier _type, BaseDeclaration* _funcDeclarator, BaseStatement* _statements)
    : type(_type), funcDeclarator(_funcDeclarator), statements(_statements) {}

FunctionDefinition::~FunctionDefinition() {
    delete funcDeclarator;
    delete statements;
}

void FunctionDefinition::compile(std::ostream& os, Context& context, int destReg) const {

    if (type == Specifier::_float || type == Specifier::_double) {
        destReg = 42;
    }

    // print various flags
    os << ".text" << std::endl;
    os << ".globl " << funcDeclarator->getIdentifier() << std::endl;
    os << funcDeclarator->getIdentifier() << ":" << std::endl;

    context.ret_label = context.makeLabel(".FUNC_RETURN");
    context.resetOffsets();
    // call getSize on its children nodes - want to return size required by: local_vars, parameters
    // call calcStackSizez(local_var_size, param_size) - hardcode to 32 bytes for now

    int param_list_size = funcDeclarator->getSize();
    std::cerr << "Param list size: " << param_list_size << std::endl;

    int statement_size = statements->getSize();
    std::cerr << "Statement list size: " << statement_size << std::endl;

    int stack_size = context.calculateStackSize(statement_size, param_list_size);

    // stack frame
    os << "addi sp,sp," << -1 * stack_size << std::endl;
    os << "sw ra, "<< stack_size - 4 << "(sp)" << std::endl;
    os << "sw s0, " << stack_size - 8 << "(sp)" << std::endl;
    os << "sw s1, " << stack_size - 12 << "(sp)" << std::endl;  // adjust the offset accordingly
    os << "addi s0,sp," << stack_size << std::endl;

    funcDeclarator->compile(os, context, destReg);

    // handle statements
    statements->compile(os, context, destReg);

    // handle return
    os << context.ret_label << ":" << std::endl;

    switch(type) {
        case Specifier::_int:
            os << "mv a0, a5" << std::endl;
            break;
        case Specifier::_double:
            os << "fmv.d fa0, fa5" << std::endl;
            break;
        case Specifier::_float:
            os << "fmv.s fa0, fa5" << std::endl;
            break;
    }

    // tear down stack frame
    os << "lw ra, "<< stack_size - 4 << "(sp)" << std::endl;
    os << "lw s0, " << stack_size - 8 << "(sp)" << std::endl;
    os << "lw s1, " << stack_size - 12 << "(sp)" << std::endl;  // adjust the offset accordingly
    os << "addi sp,sp, " << stack_size << std::endl;
    os << "jr ra \n" << std::endl;

}


FuncDeclarator::FuncDeclarator(BaseDeclaration* _declarator, List_Ptr _param_list)
    : declarator(_declarator), param_list(_param_list) {}

// Constructor with just declarator, no parameter list
FuncDeclarator::FuncDeclarator(BaseDeclaration* _declarator)
    : declarator(_declarator), param_list(nullptr) {}

FuncDeclarator::~FuncDeclarator() {
    delete declarator;
    if (param_list) {
        for (auto node: *param_list) {
            delete node;
        }
        delete param_list;
    }
}

const std::string& FuncDeclarator::getIdentifier() {
    return declarator->getIdentifier();
}

int FuncDeclarator::getSize() const {

    if (!param_list) {
        return 0;
    }
    int param_size = 0;
    int param_count = 0;
    for (auto param : *param_list) {
        param_size += dynamic_cast<const BaseDeclaration*>(param)->getSize();
        param_count++;

        if (param_count >=8) break;
    }
    return param_size;
};

void FuncDeclarator::compile(std::ostream& os, Context& context, int destReg) const {
    // needs to be in function scope, but new scope happens in compound statement :(
    // also need to handle where we have more then 8 args - no of a regs

    context.newScope();
    context.isFunctionDef = 1;

    if (param_list != nullptr) {

        int param_no = 0;
        for (auto param : *param_list) {
            // param_no above 8 is handled using the stack instead of registers
            param->compile(os, context, param_no);
            param_no++;
        }
    }

};



ParamDeclaration::ParamDeclaration(Specifier _type, BaseDeclaration* _declarator)
    : type(_type), declarator(_declarator) {}
ParamDeclaration::~ParamDeclaration() {
    delete declarator;
}

int ParamDeclaration::getSize() const {
    return typeSizes.at(type);
}

void ParamDeclaration::compile(std::ostream& os, Context& context, int destReg) const {

    int param_index = destReg;
    std::string var_name = declarator->getIdentifier();

    int param_offset = context.addParam(var_name, type, param_index);

    // params that didn't fit are stored at bottom of calller stack frame 0(sp), 4(sp)...
    // which is equivalent to callee 0(s0), 4(s0)
    if (param_offset == 1) return;

    switch(type) {
        case(Specifier::_int):
            os << "sw " << context.getMnemonic(10 + param_index) << ", " << param_offset << "(s0)" << std::endl;
            break;
        case(Specifier::_double):
            os << "fsd " << context.getMnemonic(42 + param_index) << ", " << param_offset << "(s0)" << std::endl;
            break;
        case(Specifier::_float):
            os << "fsw " << context.getMnemonic(42 + param_index) << ", " << param_offset << "(s0)" << std::endl;
            break;
    }
}


FunctionCall::FunctionCall(BaseExpression* _id): id(_id) { }
FunctionCall::FunctionCall(BaseExpression* _id, List_Ptr _args)
    : id(_id), args(_args) { }

FunctionCall::~FunctionCall() {
    delete id;
    if (args) {
        for (auto node: *args) {
            delete node;
        }
        delete args;
    }
}

bool FunctionCall::getFuncCall() const {
    return true;
}

void FunctionCall::compile(std::ostream& os, Context& context, int destReg) const  {
    std::string funcName = id->getIdentifier();

    if (args) {

        int arg_no = 0;
        for (auto arg : *args) {
            // args < 8 = store in a regs
            if (arg_no < 8) arg->compile(os, context, 10 + arg_no);
            // args > 8 store on stack
            else {
                int reg = context.allocateReg();
                context.useReg(reg);
                arg->compile(os, context, reg);
                // each arg is either a constant, or a variable which should exist in bindings already then
                // dynamic cast the arg->getId->context.getType->handle offset properly
                int arg_offset = (arg_no-8) * 4;
                os << "sw " << context.getMnemonic(reg) << ", " << arg_offset << "(sp)" << std::endl;
                context.freeReg(reg);
            }
        arg_no++;
        }

    }

    os << "call " << funcName << std::endl;
    os << "mv " << context.getMnemonic(destReg) << ",a0" << std::endl;
}



