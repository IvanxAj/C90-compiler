#include "ast/ast_while.hpp"

While::While(BaseExpression* _condition, BaseStatement* _statements)
    : condition(_condition), statements(_statements) { };

While::~While() {
    delete condition;
    delete statements;
}

void While::compile(std::ostream& os, Context& context, int destReg) const {



    std::string label1 = context.makeLabel(".L");
    std::string label2 = context.makeLabel(".L");

    os << "j " << label1 << std::endl;

    os << label2 << ":" << std::endl;
    context.useReg(destReg);
    statements->compile(os, context, destReg);
    context.freeReg(destReg);

    os << label1 << ":" << std::endl;

    context.useReg(destReg);
    condition->compile(os, context, destReg);
    context.freeReg(destReg);

    os << "bne " << context.getMnemonic(destReg) << ", " << context.getMnemonic(0) << ", " << label2 << std::endl;

};

/*
main:
        addi    sp,sp,-32
        sw      s0,28(sp)
        addi    s0,sp,32
        li      a5,1
        sw      a5,-20(s0)
        j       .L2
.L3:
        li      a5,5
        sw      a5,-20(s0)
.L2:
        lw      a5,-20(s0)
        bne     a5,zero,.L3
        nop
        mv      a0,a5
        lw      s0,28(sp)
        addi    sp,sp,32
        jr      ra


*/
