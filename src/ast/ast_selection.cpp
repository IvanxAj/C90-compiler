#include "ast/ast_selection.hpp"

IfElse::IfElse(BaseExpression* _condition, BaseStatement* _statements1, BaseStatement* _statements2)
    : condition(_condition), statements1(_statements1), statements2(_statements2)
    {}

IfElse::IfElse(BaseExpression* _condition, BaseStatement* _statements1)
    : condition(_condition), statements1(_statements1), statements2(nullptr)
    {}

int IfElse::getSize() const {
    int size1 = 0, size2 = 0;

    if (statements1) {
        size1 = statements1->getSize();
    }

    if (statements2) {
        size2 = statements2->getSize();
    }

    return std::max(size1, size2);
}

void IfElse::compile(std::ostream& os, Context& context, int destReg) const {

        std::string label1 = context.makeLabel(".L");

        // evaluate condition
        int cond_reg = context.allocateReg();
        context.useReg(destReg);
        condition->compile(os, context, cond_reg);
        os << "beq "  <<  context.getMnemonic(cond_reg) << "," << context.getMnemonic(0) << "," << label1 << std::endl;

        statements1->compile(os, context, destReg);

        //  no else
        if (statements2 == nullptr) {
             os << label1 << ":" << std::endl;
             context.freeReg(cond_reg);
             return;
        }

        // else body
        std::string label2 = context.makeLabel(".L");
        os << "j " << label2 << std::endl;

        os << label1 << ":" << std::endl;
        statements2->compile(os,context,destReg);
        os << label2 << ":" << std::endl;

        context.freeReg(cond_reg);
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
