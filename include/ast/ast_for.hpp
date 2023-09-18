#pragma once

#include "ast_node.hpp"
#include "ast_wrappers.hpp"

class For
    : public BaseStatement
{
public:
    // Constructor with initialiser
    For(BaseStatement* _initialise, BaseStatement* _condition, BaseExpression* _increment, BaseStatement* _statements);
    // Constructor without initialiser
    For(BaseStatement* _initialise, BaseStatement* _condition, BaseStatement* _statements);
    ~For();

    int getSize() const override;
    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    BaseStatement* initialise;
    BaseStatement* condition;
    BaseExpression* increment;
    BaseStatement* statements;
};
