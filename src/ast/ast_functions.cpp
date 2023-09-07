#include "ast/ast_functions.hpp"

// Three branches: type, declarator, compound statement
FunctionDefinition::FunctionDefinition(BaseDeclarator* _funcDeclarator, Node_Ptr _statements)
    : funcDeclarator(_funcDeclarator), statements(_statements) {}

FunctionDefinition::~FunctionDefinition() {
    delete funcDeclarator;
    delete statements;
}

void FunctionDefinition::compile(std::ostream& os, Context& context, int destReg) const {
    // print various flags
    os << ".text" << std::endl;
    os << ".globl " << funcDeclarator->getIdentifier() << std::endl;
    os << funcDeclarator->getIdentifier() << ":" << std::endl;


    // call getSize on its children nodes - want to return size required by: local_vars, parameters
    // call calcStackSizez(local_var_size, param_size) - hardcode to 32 bytes for now
    int stack_size = context.calculateStackSize(4, 0);

    // stack frame
    os << "addi sp,sp," << -1 * stack_size << std::endl;
    os << "sw ra, "<< stack_size - 4 << "(sp)" << std::endl;
    os << "sw s0, " << stack_size - 8 << "(sp)" << std::endl;
    os << "addi s0,sp," << stack_size << std::endl;

    // TODO: handle parameters
    funcDeclarator->compile(os, context, destReg);

    // handle statements
    statements->compile(os, context, destReg);

    // tear down stack frame
    os << "lw ra, "<< stack_size - 4 << "(sp)" << std::endl;
    os << "lw s0, " << stack_size - 8 << "(sp)" << std::endl;
    os << "addi sp,sp, " << stack_size << std::endl;
    os << "jr ra \n" << std::endl;
}


FunctionCall::FunctionCall(BaseExpression* _id): id(_id) { }

void FunctionCall::compile(std::ostream& os, Context& context, int destReg) const  {
    std::string funcName = id->getIdentifier();
    std::cout << "Function call" << std::endl;
    os << "call " << funcName << std::endl;
    os << "mv a5,a0" << std::endl;
}



