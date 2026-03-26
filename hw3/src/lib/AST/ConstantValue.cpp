#include "AST/ConstantValue.hpp"

// TODO
ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col    
                       , const std::string &value)
    : ExpressionNode{line, col}, m_value(value) {}

// TODO: You may use code snippets in AstDumper.cpp

void ConstantValueNode::print() {}

