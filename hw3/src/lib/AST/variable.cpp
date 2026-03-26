#include "AST/variable.hpp"

// TODO
VariableNode::VariableNode(const uint32_t line, const uint32_t col 
                 , const char *name, VarType type, AstNode *p_value, const bool is_const)
    : AstNode{line, col}, name(name), var_type(type), m_init(p_value), is_constant(is_const) {}

// TODO: You may use code snippets in AstDumper.cpp
void VariableNode::print() {}

void VariableNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(m_init) {
        m_init->accept(p_visitor);
    }
}
