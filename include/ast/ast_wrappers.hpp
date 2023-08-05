#pragma once

#include "ast_node.hpp"

class BaseExpression
    : public Node
{
public:
    virtual std::string getIdentifier() {}

};
