#pragma once

#include "ast_node.hpp"
#include "ast_base.hpp"

class IfElse
    : public BaseStatement
 {
    public:
        IfElse(BaseExpression* _condition, BaseStatement* _statements1,BaseStatement* _statements2);
        IfElse(BaseExpression* _condition, BaseStatement* _statements1);
        ~IfElse();

        int getSize() const override;
        void compile(std::ostream& os, Context& context, int destReg) const override;
    private:
        BaseExpression* condition;
        BaseStatement* statements1;
        BaseStatement* statements2;
};


class Switch
    : public BaseStatement
{
public:
    Switch(BaseExpression* _expression, BaseStatement* _statements);
    ~Switch();

    int getSize() const override;
    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    BaseExpression* expression;
    BaseStatement* statements;
};


class Case
    : public BaseStatement
{
public:
    Case(BaseExpression* _expression, BaseStatement* _statements);
    Case(BaseStatement* _statements);  // For "default"
    ~Case();

    int getSize() const override;
    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    BaseExpression* expression;
    BaseStatement* statements;
};
