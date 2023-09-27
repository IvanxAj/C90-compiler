#pragma once

#include "ast_wrappers.hpp"

class PointerDeclarator
    : public BaseDeclaration
{
public:
    PointerDeclarator(BaseDeclaration* _identifier);
    ~PointerDeclarator();

    bool isPointer() const override;
    const std::string& getIdentifier() const override;
    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    BaseDeclaration* identifier;
};
