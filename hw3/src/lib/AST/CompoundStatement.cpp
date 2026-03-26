#include "AST/CompoundStatement.hpp"
#include "visitor/AstNodeVisitor.hpp"

// TODO
CompoundStatementNode::CompoundStatementNode(const uint32_t line,
                                             const uint32_t col ,
                                             const NodeList *declarations,
                                             const NodeList *statements)
    : AstNode{line, col} {
        if (declarations){
            m_declarations.assign(declarations->begin(), declarations->end());
            delete declarations;
        }
        if (statements){
            m_statements .assign(statements ->begin(), statements ->end());
            delete statements ;
        }
    }

// TODO: You may use code snippets in AstDumper.cpp
void CompoundStatementNode::print() {}

void CompoundStatementNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for (auto *decl : m_declarations) {
        if (decl) {
            decl->accept(p_visitor);
        }
    }
    for (auto *stmt : m_statements) {
        if (stmt) {
            stmt->accept(p_visitor);
        }
    }
}