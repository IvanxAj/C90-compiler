#pragma once

#include "ast_node.hpp"
#include "ast_wrappers.hpp"

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
        // processing here
        funcDeclarator->compile(os, context, destReg);
        statements->compile(os, context, destReg);
    }

private:
    // Type* node;
    Declarator* funcDeclarator;
    Node_Ptr statements;
};


class FunctionCall
    : public BaseExpression
{
public:
        // FunctionCall(std::string _name): name(_name) {}
        FunctionCall(BaseExpression* _id): id(_id) { }

        void compile(std::ostream& os, Context& context, int destReg) const override {
            std::string funcName = id->getIdentifier();
            std::cout << "Function call" << std::endl;
            os << "call " << funcName << std::endl;
            os << "mv a5,a0" << std::endl;
        }
private:
    BaseExpression* id;
};



