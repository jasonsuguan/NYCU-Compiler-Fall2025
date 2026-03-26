#ifndef __AST_VARIABLE_REFERENCE_NODE_H
#define __AST_VARIABLE_REFERENCE_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>
#include <vector>

class VariableReferenceNode : public ExpressionNode {
  public:
    // normal reference
    VariableReferenceNode(const uint32_t line, const uint32_t col,const char* name,
                         const std::vector<ExpressionNode *> indices)
                          /* TODO: name */;
    // array reference
    // VariableReferenceNode(const uint32_t line, const uint32_t col
    //                       /* TODO: name, expressions */);
    ~VariableReferenceNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &v) override ;
    void accept(AstNodeVisitor &v) override { v.visit(*this); }

    const std::string &getName() const { return m_name; }

  private:
    std::string m_name;
    std::vector<ExpressionNode *> m_indices;
    // TODO: variable name, expressions
};

#endif
