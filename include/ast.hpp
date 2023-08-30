#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include "context.hpp"

#include "ast/ast_node.hpp"
#include "ast/ast_wrappers.hpp"
#include "ast/ast_declarations.hpp"
#include "ast/ast_expressions.hpp"
#include "ast/ast_statements.hpp"
#include "ast/ast_primitives.hpp"
#include "ast/ast_functions.hpp"

extern const Node *parseAST();

typedef std::vector<Node_Ptr> List;
typedef List* List_Ptr;

inline List_Ptr createList(Node_Ptr node) {
    List_Ptr nodeList = new List();
    nodeList->push_back(node);
    return nodeList;
}

inline List_Ptr appendList(List_Ptr nodeList, Node_Ptr node) {
    nodeList->push_back(node);
    return nodeList;
}
