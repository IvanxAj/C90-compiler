#pragma once

#include "ast_node.hpp"

class Integer
    : public BaseExpression
{
public:
    Integer(int _value): value(_value) {}

    void compile(std::ostream& os, Context& context, int destReg) const override {

        os << "li x"<< destReg << "," << value << std::endl;
    }

private:
    int value;
};

class Identifier
    : public BaseExpression
{
public:
    Identifier(std::string _name): name(_name) {}

    void compile(std::ostream& os, Context& context, int destReg) const override { }

    std::string getIdentifier()  override {
        return name;
    }
private:
    std::string name;
};
