#ifndef __AST_FUNCTION_INVOCATION_NODE_H
#define __AST_FUNCTION_INVOCATION_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>
#include <vector>

class FunctionInvocationNode : public ExpressionNode {
  public:
    FunctionInvocationNode(const uint32_t line, const uint32_t col,
                           const char *name,
                           const std::vector<ExpressionNode*> args);
    ~FunctionInvocationNode() = default;

    const char *getNameCString() const;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    void accept(AstNodeVisitor &p_visitor) override {
        p_visitor.visit(*this);
    }

    std::vector<ExpressionNode*> getArgs() const { return m_args; }

  private:
    std::string m_name;
    std::vector<ExpressionNode*> m_args;

    // TODO: function name, expressions
};

#endif
