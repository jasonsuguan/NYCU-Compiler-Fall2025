#ifndef __AST_PRINT_NODE_H
#define __AST_PRINT_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"

class PrintNode : public AstNode {
  public:
    PrintNode(const uint32_t line, const uint32_t col, AstNode *expression
              /* TODO: expression */);
    ~PrintNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    void accept(AstNodeVisitor &v) override { v.visit(*this); }
    AstNode *getExpression() { return expression; }

  private:
    AstNode *expression;
    // TODO: expression
};

#endif
