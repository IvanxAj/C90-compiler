#pragma once

#include "ast_node.hpp"
#include "../ast.hpp"

class ProgramRoot
    : public Node
{
public:
    ProgramRoot(List* _list ) : list(_list) {};

    void compile(std::ostream& os, Context& context, int destReg) const override {
        for (auto node: *list) {
            node->compile(os, context, destReg);
        }
    }

private:
    List* list;
};


class BaseExpression
    : public Node
{
public:
    virtual std::string getIdentifier() {}

};


