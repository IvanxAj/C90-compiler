#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <string>


#include "ast/ast_node.hpp"
#include "ast/ast_wrappers.hpp"
#include "ast/ast_declarator.hpp"
#include "ast/ast_expressions.hpp"
#include "ast/ast_statements.hpp"
#include "ast/ast_primitives.hpp"
#include "ast/ast_functions.hpp"
#include "ast/ast_init_declarator.hpp"
#include "ast/ast_declaration.hpp"
#include "ast/ast_assignment.hpp"
#include "ast/ast_binary_operation.hpp"
#include "ast/ast_arithmetic.hpp"
#include "ast/ast_logical.hpp"
#include "ast/ast_unary.hpp"
#include "ast/ast_selection.hpp"
#include "ast/ast_while.hpp"
#include "ast/ast_jump_statement.hpp"
#include "ast/ast_for.hpp"
#include "ast/ast_array.hpp"
#include "ast/ast_pointer_declarator.hpp"

#include "context.hpp"

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
