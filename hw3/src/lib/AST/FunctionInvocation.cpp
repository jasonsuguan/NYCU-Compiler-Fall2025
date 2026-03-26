#include "AST/FunctionInvocation.hpp"

// TODO
FunctionInvocationNode::FunctionInvocationNode(const uint32_t line,
                                               const uint32_t col,const char *name,
                                               const std::vector<ExpressionNode*> args)
    : ExpressionNode{line, col}, m_name{name}, m_args{std::move(args)} {}

// TODO: You may use code snippets in AstDumper.cpp
void FunctionInvocationNode::print() {}

void FunctionInvocationNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for (auto *arg : m_args) {
        arg->accept(p_visitor);
    }
}
const char *FunctionInvocationNode::getNameCString() const {
    return m_name.c_str();
}
