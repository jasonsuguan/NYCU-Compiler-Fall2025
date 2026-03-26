#ifndef AST_PROGRAM_NODE_H
#define AST_PROGRAM_NODE_H

#include "AST/ast.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/decl.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <vector>
#include "AST/function.hpp"


#include <memory>
#include <string>

using NodeList = std::vector<AstNode*>;
using FunctionList = std::vector<FunctionNode*>;

class ProgramNode final : public AstNode {
  private:
    std::string name;
    std::vector<AstNode*> m_global_decls; // 全域宣告
    std::vector<FunctionNode*> m_functions; // 函式列表
    NodeList *p_global_decls;
    FunctionList *p_functions;
    CompoundStatementNode *m_body;
    // TODO: return type, declarations, functions, compound statement

  public:
    ~ProgramNode() = default;
    ProgramNode(const uint32_t line, const uint32_t col,
                const char *const p_name, NodeList* decls,
                FunctionList *const p_functions ,CompoundStatementNode *const p_body);
                /* TODO: return type, declarations, functions,
                 *       compound statement */
    /*Hint*/
    const char *getNameCString() const { return name.c_str(); }
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    // visitor pattern version: const char *getNameCString() const;
    void print() override;
};

#endif
