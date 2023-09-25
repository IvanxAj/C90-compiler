#include "ast/ast_array.hpp"


ArrayDeclarator::ArrayDeclarator(BaseDeclaration* _declarator, BaseExpression* _array_size)
    : declarator(_declarator), array_size(_array_size) {}

ArrayDeclarator::~ArrayDeclarator() {
    delete declarator;
    delete array_size;
}

int ArrayDeclarator::getArraySize() const {
    int size = array_size->getValue();
    std::cerr << "Array declarator returned size: " << size << std::endl;
    return size;
}

const std::string& ArrayDeclarator::getIdentifier() const {
    return declarator->getIdentifier();
}

void ArrayDeclarator::compile(std::ostream& os, Context& context, int destReg) const { }



ArrayIndex::ArrayIndex(BaseExpression* _array_id, BaseExpression* _index)
    : array_id(_array_id), index(_index) {}

ArrayIndex::~ArrayIndex() {
    delete array_id;
    delete index;
}

bool ArrayIndex::isArray() const {
    return true;
}

const std::string& ArrayIndex::getIdentifier() const {
    return array_id->getIdentifier();
}

const BaseExpression* ArrayIndex::getIndex() const {
    return index;
}

void ArrayIndex::compile(std::ostream& os, Context& context, int destReg) const {
    std::string array_name = getIdentifier();
    Specifier array_type = context.getVarType(array_name);
    int array_offset = context.getVarOffset(array_name);
    int var_size = typeSizes.at(array_type);

    // TODO: handle arrays declared at global scope
    int index_reg = context.allocateReg();
    context.useReg(index_reg);
    index->compile(os, context, index_reg);

    os << "slli " << context.getMnemonic(index_reg) << ", " << context.getMnemonic(index_reg) << ", " << log2(var_size) << std::endl;
    os << "sub " << context.getMnemonic(index_reg) << ", s0, " << context.getMnemonic(index_reg) << std::endl;
    os << "addi " << context.getMnemonic(index_reg) << ", " << context.getMnemonic(index_reg) << ", " << array_offset << std::endl;

    switch(array_type) {
        case Specifier::_int:
            os << "lw " << context.getMnemonic(destReg) << ", 0(" << context.getMnemonic(index_reg) << ") " << std::endl;
            break;
        case Specifier::_double:
            os << "fld " << context.getMnemonic(destReg) << ", 0(" << context.getMnemonic(index_reg) << ") " << std::endl;
            break;
        case Specifier::_float:
            os << "flw " << context.getMnemonic(destReg) << ", 0(" << context.getMnemonic(index_reg) << ") " << std::endl;
            break;
        default:
            std::cerr << "Array index: Invalid type" << std::endl;
            exit(1);
    }

    context.freeReg(index_reg);

}
