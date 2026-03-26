#include "AST/while.hpp"

// TODO
WhileNode::WhileNode(const uint32_t line, const uint32_t col,  
                   ExpressionNode *condition, CompoundStatementNode *body)
    : AstNode{line, col}, m_condition{condition}, m_body{body} {}

// TODO: You may use code snippets in AstDumper.cpp
void WhileNode::print() {}

void WhileNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if (m_condition != nullptr) {
        m_condition->accept(p_visitor);
    }
    if (m_body != nullptr) {
        m_body->accept(p_visitor);
    }
}
