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
    virtual ~BaseExpression() = default;
    virtual const std::string& getIdentifier();
    virtual FunctionCallInfo getFuncCall() const;
};


class BaseDeclaration
    : public Node
{
public:

    virtual ~BaseDeclaration() = default;
    virtual const std::string& getIdentifier();
    virtual FunctionCallInfo getFuncCall() const;
    virtual int getSize() const;

    // ?: if these base classes continue to have the same methods - combine?

};

class BaseStatement
    : public Node {
public:
    virtual ~BaseStatement() = default;
    virtual int getSize() const;
    virtual FunctionCallInfo getFuncCall() const;
};

