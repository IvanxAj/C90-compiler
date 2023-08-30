#pragma once

#include "ast_node.hpp"

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
        // print various flags
        os << ".text" << std::endl;
        os << ".globl " << funcDeclarator->getIdentifier() << std::endl;
        os << funcDeclarator->getIdentifier() << ":" << std::endl;

        // stack frame - hard coded for now
        os << "addi sp,sp,-16" << std::endl;
        os << "sw ra,12(sp)" << std::endl;
        os << "sw s0,8(sp)" << std::endl;
        os << "addi s0,sp,16" << std::endl;

        // handle parameters
        funcDeclarator->compile(os, context, destReg);

        // handle statements
        statements->compile(os, context, destReg);

        // tear down stack frame
        os << "lw ra,12(sp)" << std::endl;
        os << "lw s0,8(sp)" << std::endl;
        os << "addi sp,sp,16" << std::endl;
        os << "jr ra \n" << std::endl;
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



