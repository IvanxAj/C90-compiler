#pragma once

#include "ast_node.hpp"
#include "ast_wrappers.hpp"

class FunctionDefinition
    : public Node
{
// Three branches: type, declarator, compound statement
public:
    FunctionDefinition(BaseDeclarator* _funcDeclarator, Node_Ptr _statements);
    ~FunctionDefinition();

    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    // Type* node;
    BaseDeclarator* funcDeclarator;
    Node_Ptr statements;
};


class FunctionCall
    : public BaseExpression
{
public:
        FunctionCall(BaseExpression* _id);

        void compile(std::ostream& os, Context& context, int destReg) const override;
private:
    BaseExpression* id;
};



