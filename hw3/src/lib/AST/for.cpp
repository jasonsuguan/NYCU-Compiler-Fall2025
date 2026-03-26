#include "AST/for.hpp"

// TODO
ForNode::ForNode(const uint32_t line, const uint32_t col,
                 DeclNode *declaration, AstNode *assignment,
                 ExpressionNode *condition, CompoundStatementNode *body)
    : AstNode{line, col}, m_declaration{declaration}, m_assignment{assignment}, m_condition{condition}, m_body{body} {}

// TODO: You may use code snippets in AstDumper.cpp
void ForNode::print() {}

void ForNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if (m_declaration != nullptr) {
        m_declaration->accept(p_visitor);
    }
    if (m_assignment != nullptr) {
        m_assignment->accept(p_visitor);
    }
    if (m_condition != nullptr) {
        m_condition->accept(p_visitor);
    }
    if (m_body != nullptr) {
        m_body->accept(p_visitor);
    }
}
