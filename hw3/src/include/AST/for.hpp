#ifndef __AST_FOR_NODE_H
#define __AST_FOR_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "AST/CompoundStatement.hpp"

class ForNode : public AstNode {
  public:
    ForNode(const uint32_t line, const uint32_t col,
            DeclNode *declaration, AstNode *assignment,
            ExpressionNode *condition, CompoundStatementNode *body);
    ~ForNode() = default;
    
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    void accept(AstNodeVisitor &p_visitor) override{
        p_visitor.visit(*this);
    };
    void print() override;

  private:
    DeclNode *m_declaration;
    AstNode *m_assignment;
    ExpressionNode *m_condition;
    CompoundStatementNode *m_body;
};

#endif
