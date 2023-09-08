#pragma once

#include "ast_node.hpp"
#include "ast_wrappers.hpp"

class Addition
    : public BaseExpression
{
public:
    Addition(BaseExpression* _left, BaseExpression* _right);
    ~Addition();

    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    BaseExpression* left;
    BaseExpression* right;
};

