#pragma once

#include "ast_node.hpp"
#include "ast_wrappers.hpp"

class CompoundStatement
    : public BaseStatement
{
public:
    CompoundStatement();
    CompoundStatement(List_Ptr _statement_list = nullptr, List_Ptr _declaration_list = nullptr);

    ~CompoundStatement();

    int getSize() const override;

    // handle scope stuff as well
    void compile(std::ostream& os, Context& context, int destReg) const override;
private:
    List_Ptr statement_list;
    List_Ptr declaration_list;
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

// Return will contain a single node
class Return
    : public BaseStatement
{
public:
    Return(BaseExpression* _expression);

    ~Return();
    // compile value of expression into correct register AO = 10
    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    BaseExpression* expression;
};
