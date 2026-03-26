#include "AST/BinaryOperator.hpp"

// TODO
BinaryOperatorNode::BinaryOperatorNode(const uint32_t line, const uint32_t col, const std::string &operator_,
                                       ExpressionNode *left,
                                       ExpressionNode *right)
    : ExpressionNode{line, col}, operator_{operator_}, left_{left}, right_{right} {}

// TODO: You may use code snippets in AstDumper.cpp
void BinaryOperatorNode::print() {}

void BinaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if (left_) {
        left_->accept(p_visitor);
    }
    if (right_) {
        right_->accept(p_visitor);
    }
}
