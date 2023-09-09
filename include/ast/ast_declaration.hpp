#pragma once

#include "ast_node.hpp"
#include "ast_wrappers.hpp"

class Declaration
    : public BaseDeclaration
{
public:
    // two constructors - with + without initialisation
    Declaration(Specifier _type, BaseDeclaration* _declarator);

    ~Declaration();

    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    Specifier type;
    BaseDeclaration* init_declarator ;
};
