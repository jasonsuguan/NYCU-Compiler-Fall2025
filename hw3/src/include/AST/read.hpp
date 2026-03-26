#ifndef __AST_READ_NODE_H
#define __AST_READ_NODE_H

#include "AST/ast.hpp"
#include "AST/VariableReference.hpp"

class ReadNode : public AstNode {
  public:
    ReadNode(const uint32_t line, const uint32_t col,
             VariableReferenceNode *target);
    ~ReadNode() = default;
    
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void print() override;

  private:
    VariableReferenceNode *m_target;
    // TODO: variable reference
};

#endif
