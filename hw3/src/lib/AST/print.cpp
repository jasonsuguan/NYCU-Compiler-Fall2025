#include "AST/print.hpp"


// TODO
PrintNode::PrintNode(const uint32_t line, const uint32_t col, AstNode *expression)
    : AstNode{line, col}, expression{expression} {}

// TODO: You may use code snippets in AstDumper.cpp
void PrintNode::print() {}

void PrintNode::visitChildNodes(AstNodeVisitor &p_visitor) {
// TODO
    if (expression != nullptr) {
        expression->accept(p_visitor);
    }
}