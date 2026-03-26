#ifndef __AST_UNARY_OPERATOR_NODE_H
#define __AST_UNARY_OPERATOR_N

#include "AST/ast.hpp"
#include "AST/AstDumper.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "AST/expression.hpp"
#include <string>

class UnaryOperatorNode : public ExpressionNode {
  public:
    UnaryOperatorNode(const uint32_t line, const uint32_t col, const std::string &op, ExpressionNode *expr
                      /* TODO: operator, expression */);
    ~UnaryOperatorNode() = default;

    void visitChildNodes(AstNodeVisitor &p_visitor) override;

    void accept(AstNodeVisitor &p_visitor) override {
        p_visitor.visit(*this);
    }
    void print() override;
    std::string getOperator() const { return op; }
    ExpressionNode* getExpression() const { return expr; }

  private:
    std::string op;
    ExpressionNode *expr;
    // TODO: operator, expression
};

#endif
