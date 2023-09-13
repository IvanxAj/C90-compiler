#include "ast/ast_selection.hpp"

IfElse::IfElse(BaseExpression* _condition, BaseStatement* _statements1, BaseStatement* _statements2)
    : condition(_condition), statements1(_statements1), statements2(_statements2)
    {}

IfElse::IfElse(BaseExpression* _condition, BaseStatement* _statements1)
    : condition(_condition), statements1(_statements1), statements2(nullptr)
    {}

void IfElse::compile(std::ostream& os, Context& context, int destReg) const {

        // int reg = context.allocateReg();
        context.useReg(destReg);
        condition->compile(os, context, destReg);

        std::string label1 = context.makeLabel(".L");
        os << "beq "  <<  context.getMnemonic(destReg) << "," << context.getMnemonic(0) << "," << label1 << std::endl;

        statements1->compile(os, context, destReg);

        //  handle else - check if it exists
        if (statements2 == nullptr) {
             os << label1 << ":" << std::endl;
             context.freeReg(destReg);
             return;
        }

        std::string label2 = context.makeLabel(".L");
        os << "j " << label2 << std::endl;

        os << label1 << ":" << std::endl;
        statements2->compile(os,context,destReg);
        os << label2 << ":" << std::endl;

        context.freeReg(destReg);
};


/*

int x = 1;
if (x)
main:
        addi    sp,sp,-32
        sw      s0,28(sp)
        addi    s0,sp,32

        li      a5,1
        sw      a5,-20(s0)

        lw      a5,-20(s0) -> compile condition
        beq     a5,zero,.L2 -> generate

        li      a5,5 -> statement compiled
        sw      a5,-28(s0)

        j       .L3 ->
.L2:
        li      a5,10
        sw      a5,-24(s0)
.L3:
        nop
        mv      a0,a5
        lw      s0,28(sp)
        addi    sp,sp,32
        jr      ra

*/
