#pragma once

#include "ast_node.hpp"
#include "ast_base.hpp"
#include "ast_binary_operation.hpp"

// using BinaryOperation::BinaryOperation; lets us recycle constructors + destructors from BinaryOperation
// Left and Right in BinaryOp are also protected - so can still be accessed if needed


class Addition
    : public BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;

    void compile(std::ostream& os, Context& context, int destReg) const override;

};

class Subtraction
    : public BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;

    void compile(std::ostream& os, Context& context, int destReg) const override;

};

class Multiplication
    : public BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;

    void compile(std::ostream& os, Context& context, int destReg) const override;

};

class Division
    : public BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;

    void compile(std::ostream& os, Context& context, int destReg) const override;

};


class Modulus
    : public BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;

    void compile(std::ostream& os, Context& context, int destReg) const override;

};


class LeftShift
    : public BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;

    void compile(std::ostream& os, Context& context, int destReg) const override;

};


class RightShift
    : public BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;

    void compile(std::ostream& os, Context& context, int destReg) const override;

};

