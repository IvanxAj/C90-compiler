#pragma once

#include "ast_node.hpp"
#include "ast_wrappers.hpp"

class Declaration
    : public BaseDeclarator
{
public:
    // two constructors - with + without initialisation
    Declaration(Specifier _type, BaseDeclarator* _declarator);

    ~Declaration();

    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    Specifier type;
    BaseDeclarator* init_declarator ;
};
