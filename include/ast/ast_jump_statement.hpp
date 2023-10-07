#include "ast_node.hpp"
#include "ast_base.hpp"

enum class JumpType {
    Return,
    Continue,
    Break
};

class JumpStatement
    : public BaseStatement
{
public:
    JumpStatement(JumpType _statement_type, BaseExpression* _expression = nullptr);
    ~JumpStatement();

    void compile(std::ostream& os, Context& context, int destReg) const override;

private:
    JumpType statement_type;
    BaseExpression* expression;
};
