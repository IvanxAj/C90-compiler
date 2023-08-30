#pragma once

class Node;

typedef const Node* Node_Ptr;

typedef std::vector<Node_Ptr> List;
typedef List* List_Ptr;


class Node {
public:

    // virtual destructor allowing derived classes to be properly destroyed when deleted through pointer to the base class.
    virtual ~Node() {};

    virtual void compile(std::ostream& os, Context& context, int destReg) const = 0;
    // shouldnt mutate ast value

    // virtual std::string& getIdentifier() const {}

};

// int main() {
//     Context context;
//     Node_Ptr ast = new FunctionDefinition( new Declarator("f"), new Return( new Integer(5)));
//     ast->compile(std::cout, context);
//     delete ast;
//     return 0;
// };
