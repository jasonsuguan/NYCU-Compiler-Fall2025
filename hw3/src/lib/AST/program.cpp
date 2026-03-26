#include "AST/program.hpp"

// TODO
ProgramNode::ProgramNode(const uint32_t line, const uint32_t col,
                         const char *const p_name,NodeList* decls,
                         FunctionList *const funcs ,CompoundStatementNode *const p_body)
    : AstNode{line, col}, name(p_name), p_global_decls(decls), p_functions(funcs), m_body(p_body) {
    if (p_global_decls) {
        m_global_decls.assign(p_global_decls->begin(), p_global_decls->end());
        delete p_global_decls;
    }
    if (p_functions) {
        m_functions.assign(p_functions->begin(), p_functions->end());
        delete p_functions;
    }
}

// visitor pattern version: const char *ProgramNode::getNameCString() const { return name.c_str(); }

void ProgramNode::print() {
    // TODO
    // outputIndentationSpace();

    std::printf("program <line: %u, col: %u> %s %s\n",
                location.line, location.col,
                name.c_str(), "void");

    // TODO
    // incrementIndentation();
    // visitChildNodes();
    // decrementIndentation();
}

//void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     m_body->accept(p_visitor);
//}

void ProgramNode::visitChildNodes(AstNodeVisitor &v) {
    for (auto *n : m_global_decls) {
        if (n) n->accept(v);
    }
    for (auto *f : m_functions) {
        if (f) f->accept(v);
    }
    if (m_body) m_body->accept(v);
}

// void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor) { // visitor pattern version
//     /* TODO
//      *
//      * for (auto &decl : var_decls) {
//      *     decl->accept(p_visitor);
//      * }
//      *
//      * // functions
//      *
//      * body->accept(p_visitor);
//      */
// }
