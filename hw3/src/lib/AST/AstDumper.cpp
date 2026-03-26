#include "AST/AstDumper.hpp"
#include "AST/BinaryOperator.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/UnaryOperator.hpp"
#include "AST/VariableReference.hpp"
#include "AST/assignment.hpp"
#include "AST/decl.hpp"
#include "AST/for.hpp"
#include "AST/function.hpp"
#include "AST/if.hpp"
#include "AST/print.hpp"
#include "AST/program.hpp"
#include "AST/read.hpp"
#include "AST/return.hpp"
#include "AST/variable.hpp"
#include "AST/while.hpp"

#include <cstdio>

// FIXME: remove this line if you choose to use visitor pattern with this template
// #ifdef I_WANT_TO_USE_VISITOR_PATTERN

void AstDumper::printIndent() const {
    std::printf("%s", m_indenter.indent().c_str());
}

void AstDumper::visit(ProgramNode &p_program) {
    printIndent();

    std::printf("program <line: %u, col: %u> %s %s\n",
                p_program.getLocation().line, p_program.getLocation().col,
                p_program.getNameCString(), "void");

    m_indenter.increaseLevel();
    p_program.visitChildNodes(*this);
    m_indenter.decreaseLevel();
}

void AstDumper::visit(DeclNode &p_decl) {
    printIndent();

    std::printf("declaration <line: %u, col: %u>\n", p_decl.getLocation().line,
                p_decl.getLocation().col);

    m_indenter.increaseLevel();
    p_decl.visitChildNodes(*this);
    m_indenter.decreaseLevel();
}

void AstDumper::visit(VariableNode &p_variable) {
    printIndent();
    std::string type_str;
    switch (p_variable.getType()) {
        case VarType::IntType:
            type_str = "integer";
            break;
        case VarType::FloatType:
            type_str = "real";
            break;
        case VarType::RealType:
            type_str = "real";
            break;
        case VarType::StringType:
            type_str = "string";
            break;
        case VarType::BoolType:
            type_str = "boolean";
            break;
        case VarType::SciType:
            type_str = "sci";
            break;
        default:
            type_str = "unknown";
            break;
    }

    // TODO: name, type
    std::printf("variable <line: %u, col: %u> %s %s",
        p_variable.getLocation().line, p_variable.getLocation().col,
        p_variable.getNameCString(), type_str.c_str());
    
    const auto &dims = p_variable.getArrayDims();
    if (!dims.empty()) {
        std::printf(" ");
        for (auto d : dims) {
            std::printf("[%u]", d);
        }
    }
    std::printf("\n");

    m_indenter.increaseLevel();
    p_variable.visitChildNodes(*this);
    m_indenter.decreaseLevel();
}

void AstDumper::visit(ConstantValueNode &p_constant_value) {
    printIndent();
    std::string val = p_constant_value.getValue();
    
    if (val.size() >= 2 && val.front() == '"' && val.back() == '"') {
        val = val.substr(1, val.size() - 2);
    }
    std::printf("constant <line: %u, col: %u> %s\n",
                p_constant_value.getLocation().line,
                p_constant_value.getLocation().col,
                val.c_str());
}

static inline const char* varTypeToString(VarType t) {
    switch (t) {
    case VarType::IntType: return "integer";
    case VarType::RealType: return "real";
    case VarType::BoolType: return "boolean";
    case VarType::StringType: return "string";
    case VarType::SciType: return "real";
    case VarType::VoidType: return "void";
    default: return "unknown";
    }
}

void AstDumper::visit(FunctionNode &p_function) {
    printIndent();

    // TODO: name, prototype string
    std::printf("function declaration <line: %u, col: %u> %s %s (",
                p_function.getLocation().line, p_function.getLocation().col,
                p_function.getName().c_str(), varTypeToString(p_function.getReturnType()));
    
    bool first = true; // 用來處理逗號
    if (auto decls = p_function.getParamDecls(); decls) { 
        for (auto *decl : *decls) {
            // DeclNode 裡面VariableNode（參數）
            for (auto *var : decl->getVariables()) {
                if (!first) std::printf(", ");
                std::printf("%s", varTypeToString(var->getType()));
                if (var->getArrayDims().size() > 0) {
                    std::printf(" ");
                    for (auto d : var->getArrayDims()) {
                        std::printf("[%u]", d);
                    }
                }
                first = false;
            }
        }
    }
    std::printf(")\n");

    

    m_indenter.increaseLevel();
    p_function.visitChildNodes(*this);
    m_indenter.decreaseLevel();
}

void AstDumper::visit(CompoundStatementNode &p_compound_statement) {
    printIndent();

    std::printf("compound statement <line: %u, col: %u>\n",
                p_compound_statement.getLocation().line,
                p_compound_statement.getLocation().col);

    m_indenter.increaseLevel();
    p_compound_statement.visitChildNodes(*this);
    m_indenter.decreaseLevel();
}

void AstDumper::visit(PrintNode &p_print) {
    printIndent();

    std::printf("print statement <line: %u, col: %u>\n",
                p_print.getLocation().line, p_print.getLocation().col);

    m_indenter.increaseLevel();
    p_print.visitChildNodes(*this);
    m_indenter.decreaseLevel();
}

void AstDumper::visit(BinaryOperatorNode &p_bin_op) {
    printIndent();

    // TODO: operator string
    std::printf("binary operator <line: %u, col: %u> %s\n",
                p_bin_op.getLocation().line, p_bin_op.getLocation().col,
                p_bin_op.getOperator().c_str());

    m_indenter.increaseLevel();
    p_bin_op.visitChildNodes(*this);
    m_indenter.decreaseLevel();
}

void AstDumper::visit(UnaryOperatorNode &p_un_op) {
    printIndent();

    // TODO: operator string
    std::printf("unary operator <line: %u, col: %u> %s\n",
                p_un_op.getLocation().line, p_un_op.getLocation().col,
                p_un_op.getOperator().c_str());

    m_indenter.increaseLevel();
    p_un_op.visitChildNodes(*this);
    m_indenter.decreaseLevel();
}

void AstDumper::visit(FunctionInvocationNode &p_func_invocation) {
    printIndent();

    // TODO: function name
    std::printf("function invocation <line: %u, col: %u> %s\n",
                p_func_invocation.getLocation().line,
                p_func_invocation.getLocation().col,
                p_func_invocation.getNameCString());

    m_indenter.increaseLevel();
    p_func_invocation.visitChildNodes(*this);
    m_indenter.decreaseLevel();
}

void AstDumper::visit(VariableReferenceNode &p_variable_ref) {
    printIndent();

    // TODO: variable name
    std::printf("variable reference <line: %u, col: %u> %s\n",
                p_variable_ref.getLocation().line,
                p_variable_ref.getLocation().col,
                p_variable_ref.getName().c_str());

    m_indenter.increaseLevel();
    p_variable_ref.visitChildNodes(*this);
    m_indenter.decreaseLevel();
}

void AstDumper::visit(AssignmentNode &p_assignment) {
    printIndent();

    std::printf("assignment statement <line: %u, col: %u>\n",
                p_assignment.getLocation().line,
                p_assignment.getLocation().col);

    m_indenter.increaseLevel();
    p_assignment.visitChildNodes(*this);
    m_indenter.decreaseLevel();
}

void AstDumper::visit(ReadNode &p_read) {
    printIndent();

    std::printf("read statement <line: %u, col: %u>\n",
                p_read.getLocation().line, p_read.getLocation().col);

    m_indenter.increaseLevel();
    p_read.visitChildNodes(*this);
    m_indenter.decreaseLevel();
}

void AstDumper::visit(IfNode &p_if) {
    printIndent();

    std::printf("if statement <line: %u, col: %u>\n", p_if.getLocation().line,
                p_if.getLocation().col);

    m_indenter.increaseLevel();
    p_if.visitChildNodes(*this);
    m_indenter.decreaseLevel();
}

void AstDumper::visit(WhileNode &p_while) {
    printIndent();

    std::printf("while statement <line: %u, col: %u>\n",
                p_while.getLocation().line, p_while.getLocation().col);

    m_indenter.increaseLevel();
    p_while.visitChildNodes(*this);
    m_indenter.decreaseLevel();
}

void AstDumper::visit(ForNode &p_for) {
    printIndent();

    std::printf("for statement <line: %u, col: %u>\n", p_for.getLocation().line,
                p_for.getLocation().col);

    m_indenter.increaseLevel();
    p_for.visitChildNodes(*this);
    m_indenter.decreaseLevel();
}

void AstDumper::visit(ReturnNode &p_return) {
    printIndent();

    std::printf("return statement <line: %u, col: %u>\n",
                p_return.getLocation().line, p_return.getLocation().col);

    m_indenter.increaseLevel();
    p_return.visitChildNodes(*this);
    m_indenter.decreaseLevel();
}

// FIXME: remove this line if you choose to use visitor pattern with this template
//#endif
