#include "ast/ast_node.hpp"

void Node::compile(std::ostream& os, Context& context, int destReg) const {
     std::cerr << "tried to compile node interface" << std::endl;
     exit(1);
};

// int main() {
//     Context context;
//     Node_Ptr ast = new FunctionDefinition( new Declarator("f"), new Return( new Integer(5)));
//     ast->compile(std::cout, context);
//     delete ast;
//     return 0;
// };
