#pragma once

#include "ast_node.hpp"

// Return will contain a single node

class Return
    : public Node
{
public:
    Return(Node_Ptr _expression): expression(_expression) {}

    ~Return() { delete expression; }
    // compile value of expression into correct register AO = 10
    void compile(std::ostream& os, Context& context, int destReg) const override {

        int expr_reg = context.allocate();
        expression->compile(os,context, expr_reg);

        os << "mv a0, x" << expr_reg << std::endl;
        os  << "lw s0,12(sp)" << std::endl;
        os <<"addi sp,sp,16" << std::endl;
        os << "jr ra" << std::endl;

    };

private:
    Node_Ptr expression;

};
