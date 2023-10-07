#pragma once

#include "ast_node.hpp"
#include "ast_base.hpp"

class InitDeclarator
    : public BaseDeclaration
{
public:
    // two constructors - with + without initialisation
    InitDeclarator(BaseDeclaration* _declarator, BaseExpression* _initialiser);
    InitDeclarator(BaseDeclaration* _declarator);

    ~InitDeclarator();

    bool isPointer() const override;
    int getArraySize() const override;
    const std::string& getIdentifier() const override;
    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    BaseDeclaration* declarator;
    BaseExpression* initialiser;
};
