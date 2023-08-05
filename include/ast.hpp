#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include "context.hpp"

#include "ast/ast_node.hpp"
#include "ast/ast_declarations.hpp"
#include "ast/ast_expressions.hpp"
#include "ast/ast_statements.hpp"
#include "ast/ast_primitives.hpp"
#include "ast/ast_functions.hpp"
#include "ast/ast_wrappers.hpp"

extern const Node *parseAST();
