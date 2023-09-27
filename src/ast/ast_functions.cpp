#include "ast/ast_functions.hpp"

// Three branches: type, declarator, compound statement
FunctionDefinition::FunctionDefinition(Specifier _type, BaseDeclaration* _func_declarator, BaseStatement* _statements)
    : type(_type), func_declarator(_func_declarator), statements(_statements) {}

FunctionDefinition::~FunctionDefinition() {
    delete func_declarator;
    delete statements;
}

void FunctionDefinition::compile(std::ostream& os, Context& context, int destReg) const {

    std::string func_name = func_declarator->getIdentifier();

    context.saveFuncReturnType(func_name, type);
    if (type == Specifier::_float || type == Specifier::_double) {
        destReg = 42;
    }


    // print various flags
    os << ".text" << std::endl;
    os << ".globl " << func_name << std::endl;
    os << func_name << ":" << std::endl;

    context.ret_label = context.makeLabel(".FUNC_RETURN");
    context.resetOffsets();
    // call getSize on its children nodes - want to return size required by: local_vars, parameters
    // call calcStackSizez(local_var_size, param_size) - hardcode to 32 bytes for now

    int param_list_size = func_declarator->getSize();
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

    func_declarator->compile(os, context, destReg);

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
        default:
            std::cerr << "FunctionDefinition: Invalid function return type" << std::endl;
            exit(1);
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

const std::string& FuncDeclarator::getIdentifier() const {
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

        // if (param_count >=8) break;
    }
    return param_size;
};

void FuncDeclarator::compile(std::ostream& os, Context& context, int destReg) const {
    // needs to be in function scope, but new scope happens in compound statement :(
    // also need to handle where we have more then 8 args - no of a regs

    context.newScope();
    context.isFunctionDef = 1;

    if (param_list != nullptr) {

        // int and float params stored independently - ie f(float a, int b)
        // where the args are stored in fa0 and a0 respectively
        // so maintain count of each param type, to fetch from correct regs

        int int_param_count = 0;
        int float_param_count = 0;

        for (auto param : *param_list) {

            // TODO param_no above 8 should be handled using the stack instead of registers
            auto param_object = dynamic_cast<const BaseDeclaration*>(param);
            Specifier param_type = param_object->isPointer() ? Specifier::_int : param_object->getType(context);
            if (param_type == Specifier::_int) {
                param->compile(os, context, int_param_count);
                int_param_count++;
            } else {
                param->compile(os, context, float_param_count);
                float_param_count++;
            }
        }
    }

};


ParamDeclaration::ParamDeclaration(Specifier _type, BaseDeclaration* _declarator)
    : type(_type), declarator(_declarator) {}
ParamDeclaration::~ParamDeclaration() {
    delete declarator;
}

bool ParamDeclaration::isPointer() const {
    return declarator->isPointer();
}

int ParamDeclaration::getSize() const {
    return typeSizes.at(type);
}

Specifier ParamDeclaration::getType(Context& context) const {
    return type;
}

void ParamDeclaration::compile(std::ostream& os, Context& context, int destReg) const {

    int param_index = destReg;
    std::string var_name = declarator->getIdentifier();
    bool is_pointer = declarator->isPointer();

    int param_offset = context.addParam(var_name, type, param_index, is_pointer);

    // params that didn't fit are stored at bottom of calller stack frame 0(sp), 4(sp)...
    // which is equivalent to callee 0(s0), 4(s0)
    if (param_offset == 1) return;

    // Pointers use standard int regs + operations
    // even if param is float *a - use the lw instruction and default regs
    Specifier effective_type = is_pointer ? Specifier::_int : type;

    switch(effective_type) {
        case(Specifier::_int):
            os << "sw " << context.getMnemonic(10 + param_index) << ", " << param_offset << "(s0)" << std::endl;
            break;
        case(Specifier::_float):
            os << "fsw " << context.getMnemonic(42 + param_index) << ", " << param_offset << "(s0)" << std::endl;
            break;
        case(Specifier::_double):
            os << "fsd " << context.getMnemonic(42 + param_index) << ", " << param_offset << "(s0)" << std::endl;
            break;
        default:
            std::cerr << "ParamDeclaration: Invalid param type" << std::endl;
            exit(1);
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

bool FunctionCall::isFuncCall() const {
    return true;
}

Specifier FunctionCall::getType(Context& context) const {

    auto it = context.functionReturnTypes.find(id->getIdentifier());
    // default to int if func call id not found - as defined by c90
    if (it == context.functionReturnTypes.end()) return Specifier::_int;
    return it->second;
}

void FunctionCall::compile(std::ostream& os, Context& context, int destReg) const  {
    std::string funcName = id->getIdentifier();

    if (args) {
        int int_arg_count = 0;
        int float_arg_count = 0;

        for (auto arg : *args) {
            Specifier arg_type = dynamic_cast<const BaseExpression*>(arg)->getType(context);

            if (arg_type == Specifier::_int) {
                if (int_arg_count < 8) arg->compile(os, context, 10 + int_arg_count);
                // ... code to store on stack
                int_arg_count++;
            } else if (arg_type == Specifier::_double || arg_type == Specifier::_float) {
                if (float_arg_count < 8) arg->compile(os, context, 42 + float_arg_count);
                // ... code to store on the stack
                float_arg_count++;
            }
        }

        /* Code when storing >8 args on stack
        int reg = context.allocateReg();
        context.useReg(reg);
        arg->compile(os, context, reg);
        int arg_offset = (arg_no-8) * 4; - hardcoded int
        os << "sw " << context.getMnemonic(reg) << ", " << arg_offset << "(sp)" << std::endl;
        context.freeReg(reg);
        */
    }

    os << "call " << funcName << std::endl;

    Specifier return_type = getType(context);
    switch (return_type) {
        case Specifier::_int:
            os << "mv " << context.getMnemonic(destReg) << ",a0" << std::endl;
            break;
        case Specifier::_float:
            os << "fmv.s " << context.getMnemonic(destReg) << ",fa0" << std::endl;
            break;
        default:
            std::cerr << "FunctionCall: Invalid function return type" << std::endl;
            exit(1);
    }

}



