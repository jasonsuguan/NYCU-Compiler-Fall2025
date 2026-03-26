#ifndef __AST_VARIABLE_NODE_H
#define __AST_VARIABLE_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "AST/ConstantValue.hpp"
#include <string>
#include <vector>

enum class VarType {
    Unknown,
    IntType,
    FloatType,
    RealType,
    StringType,
    BoolType,
    SciType,
    VoidType
};

class VariableNode : public AstNode {
  public:
    VariableNode(const uint32_t line, const uint32_t col
                 , const char *name, VarType type = VarType::Unknown, AstNode *p_value = nullptr, const bool is_const = false);
    ~VariableNode() = default;

    void print() override;
    const char *getNameCString() const { return name.c_str(); }
    void setType(const VarType type) { var_type = type; }
    void setInitValue(AstNode *init) { m_init = init; }
    void setIsConst(bool v) { is_constant = v; }
    
    void setArrayDims(const std::vector<uint32_t>& d) { array_dims = d; }
    const std::vector<uint32_t>& getArrayDims() const { return array_dims; }
    VarType getType() const { return var_type; }
    bool isConstant() const { return is_constant; }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    void accept(AstNodeVisitor &p_visitor) override {
        p_visitor.visit(*this);
    }

  private:
    std::string name;
    VarType var_type;
    std::vector<uint32_t> array_dims;
    AstNode *m_init;
    bool is_constant;
    // TODO: variable name, type, constant value
};

#endif
