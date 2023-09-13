#pragma once

#include "ast_node.hpp"
#include "ast_wrappers.hpp"

class Increment
    : public BaseExpression
{
public:
    Increment(BaseExpression* _expr);
    ~Increment();

    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    BaseExpression* expr;
};


class Decrement
    : public BaseExpression
{
public:
    Decrement(BaseExpression* _expr);
    ~Decrement();

    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    BaseExpression* expr;
};


class SizeOf
    : public BaseExpression
{
public:
    SizeOf(Specifier _type);
    SizeOf(BaseExpression* _expr);

    ~SizeOf();

    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    Specifier type;
    BaseExpression* expr;

};
