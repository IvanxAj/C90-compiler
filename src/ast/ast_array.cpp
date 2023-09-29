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
    // Get variable info
    std::string array_name = getIdentifier();
    Variable var = context.getVar(array_name);
    Specifier array_type = var.type;

    int var_size = typeSizes.at(array_type);

    // TODO: handle arrays declared at global scope
    // Allocate and use register for index
    int index_reg = context.allocateReg();
    context.useReg(index_reg);
    index->compile(os, context, index_reg);
    os << "slli " << context.getMnemonic(index_reg) << ", " << context.getMnemonic(index_reg) << ", " << log2(var_size) << std::endl;

    if (var.is_pointer) {
        array_type = Specifier::_int;
        int pointer_reg = context.allocateReg();
        context.useReg(pointer_reg);
        os << "lw " << context.getMnemonic(pointer_reg) << ", " << var.offset << "(s0)" << std::endl;
        os << "add " << context.getMnemonic(index_reg) << ", " << context.getMnemonic(pointer_reg) << ", " << context.getMnemonic(index_reg) << std::endl;
        context.freeReg(pointer_reg);
    } else {
        os << "sub " << context.getMnemonic(index_reg) << ", s0, " << context.getMnemonic(index_reg) << std::endl;
        os << "addi " << context.getMnemonic(index_reg) << ", " << context.getMnemonic(index_reg) << ", " << var.offset << std::endl;
    }

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


ArrayInitialiser::ArrayInitialiser(List_Ptr _initialiser_list)
    : initialiser_list(_initialiser_list) {}

ArrayInitialiser::~ArrayInitialiser() {
    for (auto node: *initialiser_list) {
        delete node;
    }
    delete initialiser_list;
};

void ArrayInitialiser::compile(std::ostream& os, Context& context, int destReg) const {

    // destReg holds the mem address of element 0 of array
    // all types inside initialiser list should be the same, and should match the array type for correct code
    auto first_init = dynamic_cast<const BaseExpression*>(initialiser_list->front());
    Specifier array_type = first_init->getType(context);
    int var_size = typeSizes.at(array_type);

    int init_reg = -1;
    std::string store_instruction;
    switch(array_type) {
        case Specifier::_int:
            init_reg = context.allocateReg();
            store_instruction = "sw ";
            break;
        case Specifier::_float:
            init_reg = context.allocateFloatReg();
            store_instruction = "fsw ";
            break;
        case Specifier::_double:
            init_reg = context.allocateFloatReg();
            store_instruction = "fsd ";
            break;
        default:
            std::cerr << "Initialiser: Invalid type" << std::endl;
            exit(1);
    }

    int index = 0;
    for (auto init : *initialiser_list) {

        context.useReg(init_reg);
        init->compile(os, context, init_reg);

        int element_offset = var_size * index;

        // store the initializer value into the array element
        os << store_instruction << context.getMnemonic(init_reg) << ", " << -element_offset <<"(" << context.getMnemonic(destReg) << ")" << std::endl;

        context.freeReg(init_reg);
        index++;
    }

}

