#pragma once

#include "ast_node.hpp"
#include "ast_wrappers.hpp"

class Declarator
    : public BaseDeclarator
{
// printing identifier is behaviour of functiondef not declarator -> pass up
public:
    Declarator(const std::string& _identifier);

    void compile(std::ostream& os, Context& context, int destReg) const override;

    // declarator have identifier so getIdentifier method
    const std::string& getIdentifier() override;

private:
    std::string identifier;
};
