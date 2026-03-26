#ifndef __AST_RETURN_NODE_H
#define __AST_RETURN_NODE_H

#include "AST/ast.hpp"
#include "AST/AstDumper.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "AST/expression.hpp"


class ReturnNode : public AstNode {
  public:
    ReturnNode(const uint32_t line, const uint32_t col,
               ExpressionNode *expression);
    ~ReturnNode() = default;
    
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    void accept(AstNodeVisitor &p_visitor) override{
        p_visitor.visit(*this);
    }
    void print() override;

  private:
    ExpressionNode *m_expression;
    // TODO: expression
};

#endif
