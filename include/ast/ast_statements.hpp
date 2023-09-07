#pragma once

#include "ast_node.hpp"

class CompoundStatement
    : public Node
{
public:
    CompoundStatement(List_Ptr _statement_list = nullptr, List_Ptr _declaration_list = nullptr);

    ~CompoundStatement();

    // handle scope stuff as well
    void compile(std::ostream& os, Context& context, int destReg) const override;
private:
    List_Ptr statement_list;
    List_Ptr declaration_list;
};

// Return will contain a single node
class Return
    : public Node
{
public:
    Return(Node_Ptr _expression);

    ~Return();
    // compile value of expression into correct register AO = 10
    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    Node_Ptr expression;
};
