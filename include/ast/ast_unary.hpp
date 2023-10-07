#pragma once

#include "ast_node.hpp"
#include "ast_base.hpp"

enum class UnaryOperator {
    Plus,
    Minus,
    BitwiseNot,
    LogicalNot,
    Deref,
    AddressOf
};

class UnaryOp : public BaseExpression
{
public:
    UnaryOp(UnaryOperator _op, BaseExpression* _expr);
    ~UnaryOp();

    bool isDerefPointer() const override;
    const std::string& getIdentifier() const override;
    Specifier getType(Context& context) const override;
    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    UnaryOperator op;
    BaseExpression* expr;
};


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
