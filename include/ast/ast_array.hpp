#pragma once

#include "ast_wrappers.hpp"

class ArrayDeclarator
    : public BaseDeclaration
{
public:
    ArrayDeclarator(BaseDeclaration* _declarator, BaseExpression* _array_size);
    ~ArrayDeclarator();

    int getArraySize() const override;
    const std::string& getIdentifier() const override;
    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    BaseDeclaration* declarator;
    BaseExpression* array_size;
};

class ArrayIndex
    : public BaseExpression
{
public:
    ArrayIndex(BaseExpression* _array_id, BaseExpression* _index);
    ~ArrayIndex();

    bool isArray() const override;
    const std::string& getIdentifier() const override;
    const BaseExpression* getIndex() const;
    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    BaseExpression* array_id;
    BaseExpression* index;
};

class ArrayInitialiser
    : public BaseExpression
{
public:
    ArrayInitialiser(List_Ptr _initialiser_list);
    ~ArrayInitialiser();

    void compile(std::ostream& os, Context& context, int destReg) const override;
private:
    List_Ptr initialiser_list;
};
