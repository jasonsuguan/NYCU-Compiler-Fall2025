#ifndef __AST_IF_NODE_H
#define __AST_IF_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "AST/CompoundStatement.hpp"


class IfNode : public AstNode {
  public:
    IfNode(const uint32_t line, const uint32_t col, 
           ExpressionNode *condition,
           CompoundStatementNode *body_,
           CompoundStatementNode *else_);
    ~IfNode() = default;

    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    void accept(AstNodeVisitor &p_visitor) override{
        p_visitor.visit(*this);
    };
    void print() override;

  private:
    ExpressionNode *m_condition;
    CompoundStatementNode *m_body;
    CompoundStatementNode *m_else;
};

#endif
