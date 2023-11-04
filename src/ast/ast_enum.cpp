#include "ast/ast_enum.hpp"

EnumSpecifier::EnumSpecifier(List_Ptr _enumerator_list) : enumerator_list(_enumerator_list) { }

EnumSpecifier::~EnumSpecifier() {
    for (auto node : *enumerator_list) {
        delete node;
    }
    delete enumerator_list;
}

void EnumSpecifier::compile(std::ostream& os, Context& context, int destReg) const {
    int val = -1;
    for (auto node : *enumerator_list) {
        auto enumerator = dynamic_cast<const Enumerator*>(node);
        if (enumerator->getValue() != INT_MIN ) {
            val = enumerator->getValue();
        } else {
            val++;
        }

        context.addEnum(enumerator->getIdentifier(), val);
    }

}


Enumerator::Enumerator(const std::string& _identifier, BaseExpression* _expression)
    : identifier(_identifier), expression(_expression) { }

Enumerator::~Enumerator() {
    delete expression;
}

double Enumerator::getValue() const {
    // if no expression, need to return previous value + 1 (has to be handled by EnumSpecifier)
    if(!expression) return INT_MIN;

    return expression->getValue();
}
const std::string& Enumerator::getIdentifier() const { return identifier; }

void Enumerator::compile(std::ostream& os, Context& context, int destReg) const {}
