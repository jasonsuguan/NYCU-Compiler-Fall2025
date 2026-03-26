#include "AST/VariableReference.hpp"

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line,
                                             const uint32_t col,const char* name,
                         const std::vector<ExpressionNode *> indices)
    : ExpressionNode{line, col}, m_name{name}, m_indices{indices} {}

// TODO
// VariableReferenceNode::VariableReferenceNode(const uint32_t line,
//                                              const uint32_t col)
//     : ExpressionNode{line, col} {}

// TODO: You may use code snippets in AstDumper.cpp
void VariableReferenceNode::print() {}

void VariableReferenceNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for (auto *e : m_indices) {
        if (e)
            e->accept(p_visitor);
    }
}
