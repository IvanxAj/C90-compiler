#include "ast_node.hpp"
#include "ast_wrappers.hpp"

class JumpStatement
    : public BaseStatement
{
public:
    JumpStatement(const std::string& _statement_type, BaseExpression* _expression = nullptr);

    ~JumpStatement();
    // compile value of expression into correct register AO = 10
    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    std::string statement_type;
    BaseExpression* expression;
};
