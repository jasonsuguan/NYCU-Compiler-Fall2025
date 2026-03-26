#ifndef __AST_DECL_NODE_H
#define __AST_DECL_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/variable.hpp"
#include <vector>

enum class DeclType {
    Var,
    Const
};

class DeclNode : public AstNode {
  public:
    // variable declaration
    DeclNode(const uint32_t line, const uint32_t col
            ,DeclType type, std::vector<AstNode*> decl_list/* TODO: identifiers, type */);

    // constant variable declaration
    //DeclNode(const uint32_t, const uint32_t col
    //         /* TODO: identifiers, constant */);

    ~DeclNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    void accept(AstNodeVisitor &p_visitor) override ;
    DeclType gettype() const ;

    /*for function */
    std::vector<VariableNode*> getVariables() const{
      std::vector<VariableNode*> vars;
        for (auto *n : m_decl_list)
            if (auto *v = dynamic_cast<VariableNode*>(n))
                vars.push_back(v);
        return vars;
    }

  private:
    DeclType m_type;
    std::vector<AstNode*> m_decl_list;
    // TODO: variables
};

#endif
