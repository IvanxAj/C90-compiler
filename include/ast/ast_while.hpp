#pragma once

#include "ast_node.hpp"
#include "ast_wrappers.hpp"

class While
    : public BaseStatement
{
public:
    While(BaseExpression* _condition, BaseStatement* _statements);
    ~While();

    int getSize() const override;
    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    BaseExpression* condition;
    BaseStatement* statements;
};
