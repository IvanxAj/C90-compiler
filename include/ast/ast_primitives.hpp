#pragma once

#include "ast_node.hpp"
#include "ast_base.hpp"


class Number
    : public BaseExpression
{
public:

    Number(double _value, Specifier _type);

    double getValue() const override;
    Specifier getType(Context& context) const override;
    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    double value;
    Specifier type;
};

class Identifier
    : public BaseExpression
{
public:
    Identifier(const std::string& _name);

    Specifier getType(Context& context) const override;
    const std::string& getIdentifier() const override;
    void compile(std::ostream& os, Context& context, int destReg) const override;
private:
    std::string name;
};
