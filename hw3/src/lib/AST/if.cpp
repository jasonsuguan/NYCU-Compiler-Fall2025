#include "AST/if.hpp"

// TODO
IfNode::IfNode(const uint32_t line, const uint32_t col,
               ExpressionNode *condition,
               CompoundStatementNode *body_,
               CompoundStatementNode *else_)
    : AstNode{line, col},
      m_condition{condition},
      m_body{body_},
      m_else{else_} {}

// TODO: You may use code snippets in AstDumper.cpp
void IfNode::print() {}

void IfNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if (m_condition != nullptr) {
        m_condition->accept(p_visitor);
    }
    if (m_body != nullptr) {
        m_body->accept(p_visitor);
    }
    if (m_else != nullptr) {
        m_else->accept(p_visitor);
    }
}
