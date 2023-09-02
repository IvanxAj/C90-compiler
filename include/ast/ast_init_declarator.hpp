#pragma once

#include "ast_node.hpp"
#include "ast_declarator.hpp"
#include "ast_wrappers.hpp"

class Init_Declarator
    : public Node
{
public:
    // two constructors - with + without initialisation
    Init_Declarator(Declarator* _declarator, BaseExpression* _initialiser);
    Init_Declarator(Declarator* _declarator);

    ~Init_Declarator();

    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    Declarator* declarator;
    BaseExpression* initialiser;
};
