#ifndef __AST_BINARY_OPERATOR_NODE_H
#define __AST_BINARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "AST/ast.hpp"
#include "AST/AstDumper.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>

#include <memory>

class BinaryOperatorNode : public ExpressionNode {
  public:
    BinaryOperatorNode(const uint32_t line, const uint32_t col, const std::string &operator_,
                       ExpressionNode *left,
                       ExpressionNode *right);
    ~BinaryOperatorNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    void accept(AstNodeVisitor &v) override { v.visit(*this); }
    const std::string &getOperator() const { return operator_; }

  private:
    std::string operator_;
    ExpressionNode *left_;
    ExpressionNode *right_;
    // TODO: operator, expressions
};

#endif
