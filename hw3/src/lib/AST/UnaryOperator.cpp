#include "AST/UnaryOperator.hpp"

// TODO
UnaryOperatorNode::UnaryOperatorNode(const uint32_t line, const uint32_t col, const std::string &op, ExpressionNode *expr)
    : ExpressionNode{line, col}, op(op), expr(expr) {}

// TODO: You may use code snippets in AstDumper.cpp
void UnaryOperatorNode::print() {}

void UnaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if (expr != nullptr) {
        expr->accept(p_visitor);
    }
}
