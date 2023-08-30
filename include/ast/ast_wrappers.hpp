#pragma once

#include "ast_node.hpp"
class ProgramRoot
    : public Node
{
public:
    ProgramRoot(List* _list );

    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    List* list;
};


class BaseExpression
    : public Node
{
public:
    virtual ~BaseExpression();
    virtual std::string getIdentifier();

};


