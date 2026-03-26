#include "AST/function.hpp"

// TODO
FunctionNode::FunctionNode(const uint32_t line, const uint32_t col,
                         const char *const p_name, VarType p_return_type,
                         std::vector<DeclNode*> *p_declarations,
                         CompoundStatementNode *const p_body)
    : AstNode{line, col}, name{p_name}, return_type{p_return_type},
      declarations{p_declarations}, body{p_body} {}

// TODO: You may use code snippets in AstDumper.cpp
void FunctionNode::print() {}

void FunctionNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if (declarations) {
        for (auto decl : *declarations) {
            if (decl) decl->accept(p_visitor);
        }
    }
    if (body) body->accept(p_visitor);
}
