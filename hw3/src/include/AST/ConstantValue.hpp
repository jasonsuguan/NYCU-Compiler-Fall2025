#ifndef __AST_CONSTANT_VALUE_NODE_H
#define __AST_CONSTANT_VALUE_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>

class ConstantValueNode : public ExpressionNode {
  public:
    ConstantValueNode(const uint32_t line, const uint32_t col
                       , const std::string &value);
    ~ConstantValueNode() = default;
    
    void print() override;
  
    void visitChildNodes(AstNodeVisitor &p_visitor) override {}
    void accept(AstNodeVisitor &p_visitor) override {
        p_visitor.visit(*this);
    }
    const char* getValueCString() const { return m_value.c_str(); }
    const std::string& getValue() const { return m_value; }

  private:
    std::string m_value;
};

#endif
