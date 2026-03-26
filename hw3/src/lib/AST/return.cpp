#include "AST/return.hpp"

// TODO
ReturnNode::ReturnNode(const uint32_t line, const uint32_t col,
                       ExpressionNode *expression)
    : AstNode{line, col}, m_expression(expression) {}

// TODO: You may use code snippets in AstDumper.cpp
void ReturnNode::print() {}

void ReturnNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if (m_expression != nullptr) {
        m_expression->accept(p_visitor);
    }
}
