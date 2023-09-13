#pragma once

#include "ast_node.hpp"
#include "ast_wrappers.hpp"

class IfElse
    : public BaseStatement
 {
    public:
        IfElse(BaseExpression* _condition, BaseStatement* _statements1,BaseStatement* _statements2);

        IfElse(BaseExpression* _condition, BaseStatement* _statements1);

        void compile(std::ostream& os, Context& context, int destReg) const override;
    private:
        BaseExpression* condition;
        BaseStatement* statements1;
        BaseStatement* statements2;
};
