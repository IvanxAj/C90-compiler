#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include "context.hpp"

/*
Alternative of ifndef to stop header files being pasted multiple times
*/

class Node;

typedef const Node* Node_Ptr;


class Node {
public:

    // virtual destructor allowing derived classes to be properly destroyed when deleted through pointer to the base class.
    virtual ~Node() {};

    virtual void compile(std::ostream& os, Context& context, int destReg) const = 0;
    // shouldnt mutate ast value

};


// Return will contain a single node

class Return
    : public Node
{
public:
    Return(Node_Ptr _expression): expression(_expression) {}

    ~Return() { delete expression; }
    // compile value of expression into correct register AO = 10
    void compile(std::ostream& os, Context& context, int destReg) const override {

        int expr_reg = context.allocate();
        expression->compile(os,context, expr_reg);

        os << "mv a0, x" << expr_reg << std::endl;

    };

private:
    Node_Ptr expression;

};

class Integer
    : public Node
{
public:
    Integer(int _value): value(_value) {}

    void compile(std::ostream& os, Context& context, int destReg) const override {

        os << "li x"<< destReg << "," << value << std::endl;
    }

private:
    int value;
};

// class Type
//     : Node
// {

// };

class Declarator
    : Node
{
// printing identifier is behaviour of functiondef not declarator -> pass up
public:
    Declarator(std::string _identifier) : identifier(_identifier) {}


    void compile(std::ostream& os, Context& context, int destReg) const override {}

    // only declarator have identifier so getIdentifier method only for declarators
    const std::string& getIdentifier() const {
        return identifier;
    }

private:
    std::string identifier;
};



class FunctionDefinition
    : public Node
{
// Three branches: type, declarator, compound statement
public:
    FunctionDefinition(Declarator* _funcDeclarator, Node_Ptr _statements) : funcDeclarator(_funcDeclarator), statements(_statements) {}
    ~FunctionDefinition() {
        delete funcDeclarator;
        delete statements;
    }

    void compile(std::ostream& os, Context& context, int destReg) const override {
        // print flag + do stack + frame pointer stuff
        auto funcName = funcDeclarator->getIdentifier();
        os << ".text" << std::endl;
        os << ".globl " << funcName << std::endl;

        os << funcName << ":" << std::endl;

        os << "addi sp,sp,-16" << std::endl;
        os << "sw s0,12(sp)" << std::endl;
        os << "addi s0,sp,16" << std::endl;

        // processing here
        statements->compile(os, context, destReg);

        os  << "lw s0,12(sp)" << std::endl;
        os <<"addi sp,sp,16" << std::endl;
        os << "jr ra" << std::endl;

    }

private:
    // Type* node;
    Declarator* funcDeclarator;
    Node_Ptr statements;
};

// class Identfier
//     : public Node
// {
//     public:
//         void compile(std::ostream& os, int destReg) const override {
//             ;
//         }
// }



// class CompoundStatement
//     : public Node
// {
// public:
//     void compile(std::ostream&os, int destReg) const {
//         for (auto node : statements) {
//             node->compile();
//         }
//     }

// private:
//     std::vector<Node_Ptr> statements;

//     // could be vector of unique ptr instead
//     // std::vector< std::unique_ptr<Node> > statements;
//     // then as we are transferring ownership wld use std::move

// };

// int main() {
//     Context context;
//     Node_Ptr ast = new FunctionDefinition( new Declarator("f"), new Return( new Integer(5)));
//     ast->compile(std::cout, context);
//     delete ast;
//     return 0;
// };
