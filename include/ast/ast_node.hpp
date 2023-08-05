#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <map>
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

    // virtual std::string& getIdentifier() const {}

};




// class Type
//     : Node
// {

// };



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
