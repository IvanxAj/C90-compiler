#pragma once

#include "ast_node.hpp"
#include "ast_wrappers.hpp"

class Integer
    : public BaseExpression
{
public:
    Integer(int _value);

    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    int value;
};

class Identifier
    : public BaseExpression
{
public:
    Identifier(std::string _name);

    void compile(std::ostream& os, Context& context, int destReg) const override;
    std::string getIdentifier()  override;
private:
    std::string name;
};
