#pragma once

#include "ast_node.hpp"
#include "ast_wrappers.hpp"
#include "ast_binary_operation.hpp"

// using BinaryOperation::BinaryOperation; lets us recycle constructors + destructors from BinaryOperation
// Left and Right in BinaryOp are also protected - so can still be accessed if needed


class Equal
    : public BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;

    void compile(std::ostream& os, Context& context, int destReg) const override;

};


class NotEqual
    : public BinaryOperation
{
    using BinaryOperation::BinaryOperation;

    void compile(std::ostream& os, Context& context, int destReg) const override;
};


class LessThan
    : public BinaryOperation
{
    using BinaryOperation::BinaryOperation;

    void compile(std::ostream& os, Context& context, int destReg) const override;
};


class GreaterThan
    : public BinaryOperation
{
    using BinaryOperation::BinaryOperation;

    void compile(std::ostream& os, Context& context, int destReg) const override;
};


class LessThanEqual
    : public BinaryOperation
{
    using BinaryOperation::BinaryOperation;

    void compile(std::ostream& os, Context& context, int destReg) const override;
};


class GreaterThanEqual
    : public BinaryOperation
{
    using BinaryOperation::BinaryOperation;

    void compile(std::ostream& os, Context& context, int destReg) const override;
};


class BitwiseAnd
    : public BinaryOperation
{
    using BinaryOperation::BinaryOperation;

    void compile(std::ostream& os, Context& context, int destReg) const override;
};


class BitwiseXor
    : public BinaryOperation
{
    using BinaryOperation::BinaryOperation;

    void compile(std::ostream& os, Context& context, int destReg) const override;
};


class BitwiseOr
    : public BinaryOperation
{
    using BinaryOperation::BinaryOperation;

    void compile(std::ostream& os, Context& context, int destReg) const override;
};


class LogicalAnd
    : public BinaryOperation
{
    using BinaryOperation::BinaryOperation;

    void compile(std::ostream& os, Context& context, int destReg) const override;
};


class LogicalOr
    : public BinaryOperation
{
    using BinaryOperation::BinaryOperation;

    void compile(std::ostream& os, Context& context, int destReg) const override;
};
