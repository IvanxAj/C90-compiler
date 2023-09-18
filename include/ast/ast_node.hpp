#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "context.hpp"


class Context;
class Node;

typedef const Node* Node_Ptr;
typedef std::vector<Node_Ptr> List;
typedef List* List_Ptr;


class Node {
public:

    // virtual destructor allowing derived classes to be properly destroyed when deleted through pointer to the base class.
    virtual ~Node() = default;
    virtual void compile(std::ostream& os, Context& context, int destReg) const = 0;

};
