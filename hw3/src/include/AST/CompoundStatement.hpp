#ifndef __AST_COMPOUND_STATEMENT_NODE_H
#define __AST_COMPOUND_STATEMENT_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "AST/decl.hpp"
#include <vector>

using NodeList = std::vector<AstNode *>;

class CompoundStatementNode : public AstNode {
  public:
    CompoundStatementNode(const uint32_t line, const uint32_t col,
                          const NodeList *declarations,
                          const NodeList *statements);
    ~CompoundStatementNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

  private:
    // TODO: declarations, statements
    NodeList m_declarations;
    NodeList m_statements;
};

#endif
