#ifndef AST_EXPRESSION_NODE_H
#define AST_EXPRESSION_NODE_H

#include "AST/ast.hpp"
#include "AST/PType.hpp"

class ExpressionNode : public AstNode {
  public:
    ~ExpressionNode() = default;
    ExpressionNode(const uint32_t line, const uint32_t col)
        : AstNode{line, col} {}
    
    PTypeSharedPtr getTypeSharedPtr() const { return m_type; }
    void setType(PTypeSharedPtr type) { m_type = type; }
    PTypeSharedPtr getType() const { return m_type; }

  protected:
    // for carrying type of result of an expression
    // TODO: for next assignment
    PTypeSharedPtr m_type;
};

#endif
