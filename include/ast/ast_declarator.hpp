#pragma once

#include "ast_node.hpp"

class Declarator
    : Node
{
// printing identifier is behaviour of functiondef not declarator -> pass up
public:
    Declarator(std::string _identifier);


    void compile(std::ostream& os, Context& context, int destReg) const override;

    // only declarator have identifier so getIdentifier method only for declarators
    const std::string& getIdentifier() const;

private:
    std::string identifier;
};
