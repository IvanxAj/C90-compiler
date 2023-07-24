#pragma once

#include <iostream>
#include <memory>
#include <vector>

/*
Alternative of ifndef to stop header files being pasted multiple times
*/

class Node;

typedef const Node* Node_Ptr;


class Node {
public:
    virtual void compile(std::ostream& os, int destReg) const = 0;
    // shouldnt mutate ast value

};



// Return will contain a single node
class Return
    : public Node
{
public:
    Return(Node_Ptr _expression): expression(_expression) {}
    // compile value of expression into correct register AO = 10
    void compile(std::ostream& os, int destReg) const override {
        expression->compile(os, 10);

        // expression->compile(os, destReg);
        // os << "mov " << "10, " << destReg << std::endl;
    };

private:
    Node_Ptr expression;

};



class Integer
    : public Node
{
public:
    Integer(int _value): value(_value) {}
    void compile(std::ostream& os, int destReg) const override {
        os << "li " << destReg << ", " << value << "std::endl";
    };

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

    void compile(std::ostream& os, int destReg) const override {

    }

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

    void compile(std::ostream& os, int destReg) const override {
        // print flag + do stack + frame pointer stuff
        auto funcName = funcDeclarator->getIdentifier();
        os << funcName << ":\n";
        statements->compile(os, destReg);
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

