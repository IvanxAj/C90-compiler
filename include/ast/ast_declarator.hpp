#pragma once

#include "ast_node.hpp"
#include "ast_base.hpp"

class Declarator
    : public BaseDeclaration
{
// printing identifier is behaviour of functiondef not declarator -> pass up
public:
    Declarator(const std::string& _identifier);

    const std::string& getIdentifier() const override;
    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    std::string identifier;
};
