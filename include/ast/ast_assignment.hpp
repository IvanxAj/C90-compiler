#pragma once

#include "ast_base.hpp"
#include "ast_array.hpp"

class Assignment
    : public BaseExpression
{
public:
    Assignment(BaseExpression* _expr1, BaseExpression* _expr2);
    ~Assignment();

    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    BaseExpression* expr1;
    BaseExpression* expr2;
};
