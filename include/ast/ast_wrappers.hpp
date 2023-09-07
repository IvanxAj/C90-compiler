#pragma once

#include "ast_node.hpp"
class ProgramRoot
    : public Node
{
public:
    ProgramRoot(List_Ptr _list );
    ~ProgramRoot();
    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    List_Ptr list;
};


class BaseExpression
    : public Node
{
public:
    virtual ~BaseExpression();
    virtual const std::string& getIdentifier();

};


class BaseDeclarator
    : public Node
{
public:

    // will need a getSize soon probs
    virtual ~BaseDeclarator();
    virtual const std::string& getIdentifier();

    // ?: if these base classes continue to have the same methods - combine?

};

