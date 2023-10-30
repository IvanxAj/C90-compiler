#pragma once

#include "ast_base.hpp"
#include <climits>

class EnumSpecifier
    : public BaseDeclaration
{
public:
    EnumSpecifier(List_Ptr _enumerator_list);
    ~EnumSpecifier();

    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    List_Ptr enumerator_list;

};


class Enumerator
    : public BaseExpression
{
public:
    Enumerator(const std::string& _identifier, BaseExpression* _expression = nullptr);
    ~Enumerator();

    int getValue() const override;
    const std::string& getIdentifier() const override;

    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    std::string identifier;
    BaseExpression* expression;
};
