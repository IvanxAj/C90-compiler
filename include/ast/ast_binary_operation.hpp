#pragma once

#include "ast_node.hpp"
#include "ast_wrappers.hpp"

class BinaryOperation
    : public BaseExpression
{
public:
    BinaryOperation(BaseExpression* _left, BaseExpression* _right, bool _owns_operands = true);
    ~BinaryOperation();

    int prepLeft(std::ostream& os, Context& context, int destReg) const;
    int prepRight(std::ostream& os, Context& context, int destReg) const;

    void compile(std::ostream& os, Context& context, int destReg) const override;

protected:
    BaseExpression* left;
    BaseExpression* right;
    bool owns_operands;
};
