#pragma once

#include "ast_node.hpp"
#include "ast_wrappers.hpp"

class CompoundStatement
    : public BaseStatement
{
public:
    CompoundStatement(List_Ptr _statement_list = nullptr, List_Ptr _declaration_list = nullptr);

    ~CompoundStatement();

    int getSize() const override;
    FunctionCallInfo getFuncCall() const override;
    void compile(std::ostream& os, Context& context, int destReg) const override;
private:
    List_Ptr statement_list;
    List_Ptr declaration_list;
    bool isEmpty = false;
};

class ExpressionStatement
    : public BaseStatement
{
public:
    ExpressionStatement(BaseExpression* _expression);
    ~ExpressionStatement();

    void compile(std::ostream& os, Context& context, int destReg) const override;
private:
    BaseExpression* expression;
};
