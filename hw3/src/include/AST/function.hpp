#ifndef __AST_FUNCTION_NODE_H
#define __AST_FUNCTION_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/decl.hpp"
#include <string>
#include <vector>

class FunctionNode : public AstNode {
  public:
    FunctionNode(const uint32_t line, const uint32_t col,
                 const char *const p_name, VarType p_return_type,
                 std::vector<DeclNode*> *p_declarations,
                 CompoundStatementNode *const p_body = nullptr);
    ~FunctionNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

    std::string getName() const { return name; }
    VarType getReturnType() const { return return_type; }
    std::vector<DeclNode*>* getParamDecls() const { return declarations; }
    

  private:
    std::string name;
    VarType return_type;
    std::vector<DeclNode*> *declarations; // declaration list 參數
    CompoundStatementNode *body;
    // TODO: name, declarations, return type, compound statement
};

#endif
