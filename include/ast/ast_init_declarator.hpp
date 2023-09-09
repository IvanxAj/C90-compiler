#pragma once

#include "ast_node.hpp"
#include "ast_wrappers.hpp"

class Init_Declarator
    : public BaseDeclaration
{
public:
    // two constructors - with + without initialisation
    Init_Declarator(BaseDeclaration* _declarator, BaseExpression* _initialiser);
    Init_Declarator(BaseDeclaration* _declarator);

    ~Init_Declarator();

    const std::string& getIdentifier() override;

    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    BaseDeclaration* declarator;
    BaseExpression* initialiser;
};
