#include "AST/decl.hpp"

// TODO
DeclNode::DeclNode(const uint32_t line, const uint32_t col,
                   DeclType type, std::vector<AstNode*> decl_list)
    : AstNode{line, col} , m_type(type), m_decl_list(std::move(decl_list)) {}

// TODO
//DeclNode::DeclNode(const uint32_t line, const uint32_t col)
//    : AstNode{line, col} {}

// TODO: You may use code snippets in AstDumper.cpp
void DeclNode::print() {}

void DeclNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for (auto &decl : m_decl_list) {
        decl->accept(p_visitor);
    }
}

void DeclNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
}

DeclType DeclNode::gettype() const {
    return m_type;
}