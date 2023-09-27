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

    virtual bool isArray() const;
    virtual bool isDerefPointer() const;
    virtual bool isFuncCall() const;

    virtual int getValue() const;
    virtual const std::string& getIdentifier() const;
    virtual Specifier getType(Context& context) const;
};


class BaseDeclaration
    : public Node
{
public:

    virtual ~BaseDeclaration() = default;

    virtual bool isPointer() const;

    virtual int getSize() const;
    virtual int getArraySize() const;

    virtual const std::string& getIdentifier() const;
    virtual Specifier getType(Context& context) const;

};

class BaseStatement
    : public Node {
public:
    virtual ~BaseStatement() = default;
    virtual int getSize() const;
};

