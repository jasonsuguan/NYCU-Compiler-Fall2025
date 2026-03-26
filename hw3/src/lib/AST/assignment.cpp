#include "AST/assignment.hpp"

// TODO
AssignmentNode::AssignmentNode(const uint32_t line, const uint32_t col,
                               VariableReferenceNode *variable_ref,
                               ExpressionNode *expression)
    : AstNode{line, col}, m_variable_ref{variable_ref}, m_expression{expression} {}

// TODO: You may use code snippets in AstDumper.cpp
void AssignmentNode::print() {}

void AssignmentNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    m_variable_ref->accept(p_visitor);
    m_expression->accept(p_visitor);
}