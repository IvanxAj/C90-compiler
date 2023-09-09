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


class BaseDeclaration
    : public Node
{
public:

    virtual ~BaseDeclaration();
    virtual const std::string& getIdentifier();
    virtual int getSize() const;

    // ?: if these base classes continue to have the same methods - combine?

};

