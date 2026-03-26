#ifndef __AST_ASSIGNMENT_NODE_H
#define __AST_ASSIGNMENT_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "AST/VariableReference.hpp"

class AssignmentNode : public AstNode {
  public:
    AssignmentNode(const uint32_t line, const uint32_t col,
                   VariableReferenceNode *variable_ref, ExpressionNode *expression);
    ~AssignmentNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
  private:
    VariableReferenceNode *m_variable_ref;
    ExpressionNode *m_expression;
};

#endif
