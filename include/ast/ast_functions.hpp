#pragma once

#include "ast_node.hpp"
#include "ast_wrappers.hpp"

class FunctionDefinition
    : public Node
{
// Three branches: type, declarator, compound statement
public:
    FunctionDefinition(BaseDeclaration* _funcDeclarator, BaseStatement* _statements);
    ~FunctionDefinition();

    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    // Type* node;
    BaseDeclaration* funcDeclarator;
    BaseStatement* statements;
};


class FuncDeclarator
    : public BaseDeclaration
{
public:
    FuncDeclarator(BaseDeclaration* _declarator, List_Ptr _param_list);
    // Constructor with just declarator, no parameter list
    FuncDeclarator(BaseDeclaration* _declarator);
    ~FuncDeclarator();

    const std::string& getIdentifier() override;
    int getSize() const override;
    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    BaseDeclaration* declarator;
    List_Ptr param_list;
};


class ParamDeclaration
: public BaseDeclaration
{
public:
    ParamDeclaration(Specifier _type, BaseDeclaration* _declarator);
    ~ParamDeclaration();

    int getSize() const override;
    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    Specifier type;
    BaseDeclaration* declarator;
};

class FunctionCall
    : public BaseExpression
{
public:
    FunctionCall(BaseExpression* _id);
    FunctionCall(BaseExpression* _id, List_Ptr _args);
    ~FunctionCall();

    bool getFuncCall() const override;
    void compile(std::ostream& os, Context& context, int destReg) const override;
private:
    BaseExpression* id;
    List_Ptr args;
};
