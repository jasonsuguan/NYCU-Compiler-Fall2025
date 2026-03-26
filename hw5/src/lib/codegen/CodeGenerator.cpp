#include "AST/CompoundStatement.hpp"
#include "AST/for.hpp"
#include "AST/function.hpp"
#include "AST/program.hpp"
#include "codegen/CodeGenerator.hpp"
#include "sema/SemanticAnalyzer.hpp"
#include "sema/SymbolTable.hpp"
#include "visitor/AstNodeInclude.hpp"

#include <algorithm>
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

CodeGenerator::CodeGenerator(const std::string &source_file_name,
                             const std::string &save_path,
                             std::unordered_map<SemanticAnalyzer::AstNodeAddr,
                                                      SymbolManager::Table>
                                 &&p_symbol_table_of_scoping_nodes)
    : m_symbol_manager(false /* no dump */),
      m_source_file_path(source_file_name),
      m_symbol_table_of_scoping_nodes(std::move(p_symbol_table_of_scoping_nodes)) {
    // FIXME: assume that the source file is always xxxx.p
    const auto &real_path =
        save_path.empty() ? std::string{"."} : save_path;
    auto slash_pos = source_file_name.rfind('/');
    auto dot_pos = source_file_name.rfind('.');

    if (slash_pos != std::string::npos) {
        ++slash_pos;
    } else {
        slash_pos = 0;
    }
    auto output_file_path{
        real_path + "/" +
        source_file_name.substr(slash_pos, dot_pos - slash_pos) + ".S"};
    m_output_file.reset(fopen(output_file_path.c_str(), "w"));
    assert(m_output_file.get() && "Failed to open output file");
}

static void dumpInstructions(FILE *p_out_file, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(p_out_file, format, args);
    va_end(args);
}

void CodeGenerator::visit(ProgramNode &p_program) {
    // Generate RISC-V instructions for program header
    // clang-format off
    this->m_current_offset = 12;
    constexpr const char *const riscv_assembly_file_prologue =
        "    .file \"%s\"\n"
        "    .option nopic\n"
        ".section    .text\n"
        "    .align 2\n";
    // clang-format on
    dumpInstructions(m_output_file.get(), riscv_assembly_file_prologue,
                     m_source_file_path.c_str());

    // Reconstruct the scope for looking up the symbol entry.
    // Hint: Use m_symbol_manager->lookup(symbol_name) to get the symbol entry.
    m_symbol_manager.pushScope(
        std::move(m_symbol_table_of_scoping_nodes.at(&p_program)));

    auto visit_ast_node = [&](auto &ast_node) { ast_node->accept(*this); };
    for_each(p_program.getDeclNodes().begin(), p_program.getDeclNodes().end(),
             visit_ast_node);
    for_each(p_program.getFuncNodes().begin(), p_program.getFuncNodes().end(),
             visit_ast_node);


    // main
    dumpInstructions(m_output_file.get(), ".section    .text\n");
    dumpInstructions(m_output_file.get(), "    .globl main\n");
    dumpInstructions(m_output_file.get(), "    .type main, @function\n");
    dumpInstructions(m_output_file.get(), "main:\n");
    
    // Function Prologue
    dumpInstructions(m_output_file.get(), "    addi sp, sp, -128\n");
    dumpInstructions(m_output_file.get(), "    sw ra, 124(sp)\n");
    dumpInstructions(m_output_file.get(), "    sw s0, 120(sp)\n");
    dumpInstructions(m_output_file.get(), "    addi s0, sp, 128\n");

    const_cast<CompoundStatementNode &>(p_program.getBody()).accept(*this);

    // Function Epilogue
    dumpInstructions(m_output_file.get(), "    lw ra, 124(sp)\n    lw s0, 120(sp)\n    addi sp, sp, 128\n    jr ra\n"); //
    dumpInstructions(m_output_file.get(), "    .size main, .-main\n");

    m_symbol_manager.popScope();
}

void CodeGenerator::visit(DeclNode &p_decl) {
    p_decl.visitChildNodes(*this);
}

void CodeGenerator::visit(VariableNode &p_variable) {
    const SymbolEntry *entry = m_symbol_manager.lookup(p_variable.getName());

    if (entry == nullptr) {
        return; 
    }

    if (entry->getLevel() == 0) { 
        if (entry->getKind() == SymbolEntry::KindEnum::kConstantKind) { 
            // 常數處理到 .rodata 區段
            dumpInstructions(m_output_file.get(), ".section    .rodata\n");
            dumpInstructions(m_output_file.get(), "    .align 2\n");
            dumpInstructions(m_output_file.get(), "    .globl %s\n", entry->getNameCString());
            dumpInstructions(m_output_file.get(), "    .type %s, @object\n", entry->getNameCString());
            dumpInstructions(m_output_file.get(), "%s:\n", entry->getNameCString());
            dumpInstructions(m_output_file.get(), "    .word %s\n", 
                             p_variable.getConstantPtr()->getConstantValueCString());
            dumpInstructions(m_output_file.get(), ".section    .text\n");
        } 
        else { 
            // 全域變數使用 .comm
            dumpInstructions(m_output_file.get(), ".comm %s, 4, 4\n", entry->getNameCString());
        }
    } 
    
    else { 
        // 區域變數分配 Offset 並存入符號表
        const_cast<SymbolEntry *>(entry)->setOffset(-m_current_offset);
        int current_offset = -m_current_offset;
        m_current_offset += 4;
        // 區域常數執行期賦予初值
        if (entry->getKind() == SymbolEntry::KindEnum::kConstantKind) { 
            const char *val_str = p_variable.getConstantPtr()->getConstantValueCString();

            // 將值存入 Stack 中的對應位置
            dumpInstructions(m_output_file.get(), "    li t0, %s\n", val_str);
            dumpInstructions(m_output_file.get(), "    sw t0, %d(s0)\n", current_offset);
        }
    }
}

void CodeGenerator::visit(ConstantValueNode &p_constant_value) {
    this->m_is_lvalue = false;
    std::string val_str = p_constant_value.getConstantValueCString();
    if (val_str == "true") {
        dumpInstructions(m_output_file.get(), "    li t0, 1\n");
    } else if (val_str == "false") {
        dumpInstructions(m_output_file.get(), "    li t0, 0\n");
    } else {
        dumpInstructions(m_output_file.get(), "    li t0, %s\n", val_str.c_str());
    }

    dumpInstructions(m_output_file.get(), "    addi sp, sp, -4\n");
    dumpInstructions(m_output_file.get(), "    sw t0, 0(sp)\n");
}

void CodeGenerator::visit(FunctionNode &p_function) {
    // 重置偏移量
    this->m_current_function_name = p_function.getNameCString();
    this->m_current_offset = 12;
    

    dumpInstructions(m_output_file.get(), ".section .text\n");
    dumpInstructions(m_output_file.get(), "    .globl %s\n", p_function.getNameCString());
    dumpInstructions(m_output_file.get(), "%s:\n", p_function.getNameCString());
    
    dumpInstructions(m_output_file.get(), "    addi sp, sp, -128\n");
    dumpInstructions(m_output_file.get(), "    sw ra, 124(sp)\n");
    dumpInstructions(m_output_file.get(), "    sw s0, 120(sp)\n");
    dumpInstructions(m_output_file.get(), "    addi s0, sp, 128\n");

    // 建立一個傳入參數的符號表
    m_symbol_manager.pushScope(
        std::move(m_symbol_table_of_scoping_nodes.at(&p_function)));

    // 將傳入的 a0, a1存入 Stack Frame，讓 VariableReference 找得到
    
    int arg_idx = 0;
    auto &params = p_function.getParameters();

    for (auto &param_decl : params) {
    for (auto &var_node : param_decl->getVariables()) {
        const SymbolEntry *entry = m_symbol_manager.lookup(var_node->getName());
        if(entry){
            if (arg_idx < 8) {
                // 前 8 個參數：從 a0-a7 存入目前的 Stack Frame
                const_cast<SymbolEntry*>(entry)->setOffset(-m_current_offset);
                dumpInstructions(m_output_file.get(), "    sw a%d, %d(s0)\n", arg_idx, -m_current_offset);
                m_current_offset += 4;
            } else {
                // 第 9 個以後的參數：在 s0 之上的位置
                // RISC-V中，第 9 個參數在 0(s0), 第 10 個在 4(s0)...
                int stack_offset = (arg_idx - 8) * 4;
                const_cast<SymbolEntry*>(entry)->setOffset(stack_offset);
            }
            arg_idx++;
        }
    }
}
    p_function.visitBodyChildNodes(*this);
    

    dumpInstructions(m_output_file.get(), "%s_end:\n", p_function.getNameCString()); // 方便 Return 跳轉
    dumpInstructions(m_output_file.get(), "    lw ra, 124(sp)\n");
    dumpInstructions(m_output_file.get(), "    lw s0, 120(sp)\n");
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 128\n");
    dumpInstructions(m_output_file.get(), "    jr ra\n");

    m_symbol_manager.popScope();
}

void CodeGenerator::visit(CompoundStatementNode &p_compound_statement) {
    bool has_scope = m_symbol_table_of_scoping_nodes.count(&p_compound_statement);
    
    if (has_scope) {
        m_symbol_manager.pushScope(
            std::move(m_symbol_table_of_scoping_nodes.at(&p_compound_statement)));
    }

    p_compound_statement.visitChildNodes(*this);

    if (has_scope) {
        m_symbol_manager.popScope();
    }
}

void CodeGenerator::visit(PrintNode &p_print) {
    // 1. 先去執行子節點 (Expr)，這會把要印的值推到 Stack 頂端
    p_print.visitChildNodes(*this);

    // 2. 從 Stack 拿出來放到 a0 (RISC-V 函式參數規定)
    dumpInstructions(m_output_file.get(), "    lw a0, 0(sp)\n");
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");

    // 3. 呼叫助教提供的印表函式
    dumpInstructions(m_output_file.get(), "    jal ra, printInt\n");
}

void CodeGenerator::visit(BinaryOperatorNode &p_bin_op) {
    this->m_is_lvalue = false;
    p_bin_op.visitChildNodes(*this); 

    // 從 Stack 取出運算數
    dumpInstructions(m_output_file.get(), "    lw t1, 0(sp)\n");
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");
    
    // 接著 Pop 出左運算放到 t0
    dumpInstructions(m_output_file.get(), "    lw t0, 0(sp)\n");
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");

    std::string op = p_bin_op.getOpCString();
    if (op == "+") {
        dumpInstructions(m_output_file.get(), "    add t0, t0, t1\n");
    } else if (op == "-") {
        dumpInstructions(m_output_file.get(), "    sub t0, t0, t1\n");
    } else if (op == "*") {
        dumpInstructions(m_output_file.get(), "    mul t0, t0, t1\n");
    } else if (op == "/") {
        dumpInstructions(m_output_file.get(), "    div t0, t0, t1\n");
    } else if (op == "mod") {
        dumpInstructions(m_output_file.get(), "    rem t0, t0, t1\n");
    } else if (op == "==" || op == "=") {
        dumpInstructions(m_output_file.get(), "    sub t2, t0, t1\n");
        dumpInstructions(m_output_file.get(), "    seqz t0, t2\n");
    } else if (op == "!=") {
        dumpInstructions(m_output_file.get(), "    sub t2, t0, t1\n");
        dumpInstructions(m_output_file.get(), "    snez t0, t2\n");
    } else if (op == "<") {
        dumpInstructions(m_output_file.get(), "    slt t0, t0, t1\n");
    } else if (op == "<=") {
        dumpInstructions(m_output_file.get(), "    sgt t2, t0, t1\n");
        dumpInstructions(m_output_file.get(), "    seqz t0, t2\n");
    } else if (op == ">") {
        dumpInstructions(m_output_file.get(), "    sgt t0, t0, t1\n");
    } else if (op == ">=") {
        dumpInstructions(m_output_file.get(), "    slt t2, t0, t1\n");
        dumpInstructions(m_output_file.get(), "    seqz t0, t2\n");
    } 

    // 運算結果 Push 回 Stack，供下一個運算或 Assignment
    dumpInstructions(m_output_file.get(), "    addi sp, sp, -4\n");
    dumpInstructions(m_output_file.get(), "    sw t0, 0(sp)\n");
}

void CodeGenerator::visit(UnaryOperatorNode &p_un_op) {
    this->m_is_lvalue = false;
    p_un_op.visitChildNodes(*this);

    dumpInstructions(m_output_file.get(), "    lw t0, 0(sp)\n");
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");

    std::string op = p_un_op.getOpCString();
    if (op == "neg") {
        dumpInstructions(m_output_file.get(), "    sub t0, x0, t0\n");
    } 

    else if (op == "not") {
        dumpInstructions(m_output_file.get(), "    seqz t0, t0\n");
    }

    dumpInstructions(m_output_file.get(), "    addi sp, sp, -4\n");
    dumpInstructions(m_output_file.get(), "    sw t0, 0(sp)\n");
}

void CodeGenerator::visit(FunctionInvocationNode &p_func_invocation) {
    this->m_is_lvalue = false;
    auto &arguments = p_func_invocation.getArguments();

    // 從 Stack 把參數 Pop 出來，放到 a0, a1...
    int param_count = p_func_invocation.getArguments().size();

    for (int i = param_count - 1; i >= 8; --i) {
        arguments[i]->accept(*this); 
    }

    for (int i = 0; i < std::min(param_count, 8); ++i) {
        arguments[i]->accept(*this);
    }

    for (int i = std::min(param_count, 8) - 1; i >= 0; --i) {
        dumpInstructions(m_output_file.get(), "    lw a%d, 0(sp)\n", i);
        dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");
    }

    dumpInstructions(m_output_file.get(), "    jal ra, %s\n", p_func_invocation.getNameCString());

    if (param_count > 8) {
        int overflow_bytes = (param_count - 8) * 4;
        dumpInstructions(m_output_file.get(), "    addi sp, sp, %d\n", overflow_bytes);
    }
    dumpInstructions(m_output_file.get(), "    addi sp, sp, -4\n");
    dumpInstructions(m_output_file.get(), "    sw a0, 0(sp)\n");
}

void CodeGenerator::visit(VariableReferenceNode &p_variable_ref) {
    // 尋找該變數的 Entry
    const SymbolEntry *entry = m_symbol_manager.lookup(p_variable_ref.getName());
    const char* var_name = entry->getNameCString();

    if (entry->getLevel() == 0) {
        // 全域變數 使用 la 取得全域標籤的位址
        dumpInstructions(m_output_file.get(), "    la t0, %s\n", var_name);

        if (!this->m_is_lvalue) {
            dumpInstructions(m_output_file.get(), "    lw t1, 0(t0)\n");
            dumpInstructions(m_output_file.get(), "    mv t0, t1\n");
        }
    } 
    else {
        // 區域變數 取得我們之前在 visit(VariableNode) 存入的 Offset
        int offset = entry->getOffset();

        if (this->m_is_lvalue) {
            // lhs計算地址
            dumpInstructions(m_output_file.get(), "    addi t0, s0, %d\n", offset);
        } else {
            // rhs從該位址載入數值
            dumpInstructions(m_output_file.get(), "    lw t0, %d(s0)\n", offset);
        }
    }

    // 將結果推入 Stack
    dumpInstructions(m_output_file.get(), "    addi sp, sp, -4\n");
    dumpInstructions(m_output_file.get(), "    sw t0, 0(sp)\n");

    this->m_is_lvalue = false;
}

void CodeGenerator::visit(AssignmentNode &p_assignment) {
    // 左式需要取得變數的「地址」
    this->m_is_lvalue = true;
    
    const_cast<VariableReferenceNode &>(p_assignment.getLvalue()).accept(*this);

    this->m_is_lvalue = false;

    // 處理右式需要取得運算的「數值」
    const_cast<ExpressionNode &>(p_assignment.getExpr()).accept(*this);

    // 執行賦值Stack 頂端 (0(sp)) 是右式的值，下一層 (4(sp)) 是左式的地址

    // Pop 出數值到 t0
    dumpInstructions(m_output_file.get(), "    lw t0, 0(sp)\n");
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");
    
    // Pop 出地址到 t1
    dumpInstructions(m_output_file.get(), "    lw t1, 0(sp)\n");
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");
    
    // 存值
    dumpInstructions(m_output_file.get(), "    sw t0, 0(t1)\n");
}

void CodeGenerator::visit(ReadNode &p_read) {
    // 取得目標變數的「地址」
    // 設定旗標為 true，VariableReferenceNode 產生 addi t0, s0, offset 指令
    this->m_is_lvalue = true;
    
    // 拜訪變數參考節點
    const_cast<VariableReferenceNode &>(p_read.getTarget()).accept(*this);
    
    // 用完地址重設
    this->m_is_lvalue = false;

    // 呼叫外部函式 readInt
    // 執行完後，使用者輸入的整數會存在 a0 暫存器中
    dumpInstructions(m_output_file.get(), "    jal ra, readInt\n");

    // 執行儲存 此時 Stack 頂端存放的是剛才推上去的變數地址
    dumpInstructions(m_output_file.get(), "    lw t0, 0(sp)\n"); // 取出地址到 t0
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");
    dumpInstructions(m_output_file.get(), "    sw a0, 0(t0)\n");
}

void CodeGenerator::visit(IfNode &p_if) {
    // 生成這次 if 需要的標籤
    std::string label_else = "L" + std::to_string(m_label_count++);
    std::string label_exit = "L" + std::to_string(m_label_count++);

    // 計算條件判斷式 (結果會被 Push 到 Stack)
    this->m_is_lvalue = false;
    //p_if.getCondition().accept(*this);
    const_cast<ExpressionNode &>(p_if.getCondition()).accept(*this);

    // 取出條件結果到 t0
    dumpInstructions(m_output_file.get(), "    lw t0, 0(sp)\n");
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");

    // t0 == 0跳轉到 Else
    dumpInstructions(m_output_file.get(), "    beqz t0, %s\n", label_else.c_str());

    // 拜訪 Then 區塊 
    const_cast<CompoundStatementNode &>(p_if.getThenBody()).accept(*this);

    // 執行完 Then 後，必須跳過 Else 區塊直接去 Exit
    dumpInstructions(m_output_file.get(), "    j %s\n", label_exit.c_str());

    // Else 標籤起點
    dumpInstructions(m_output_file.get(), "%s:\n", label_else.c_str());
    
    // 有 Else 區塊才拜訪
    if (p_if.getElseBody()) {
        const_cast<CompoundStatementNode *>(p_if.getElseBody())->accept(*this);
    }
    dumpInstructions(m_output_file.get(), "%s:\n", label_exit.c_str());
}

void CodeGenerator::visit(WhileNode &p_while) {
    // 1. 生成兩個唯一的標籤
    std::string label_start = "L" + std::to_string(m_label_count++);
    std::string label_exit = "L" + std::to_string(m_label_count++);

    dumpInstructions(m_output_file.get(), "%s:\n", label_start.c_str());

    this->m_is_lvalue = false;
    const_cast<ExpressionNode &>(p_while.getCondition()).accept(*this);

    dumpInstructions(m_output_file.get(), "    lw t0, 0(sp)\n");
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");

    // 若條件不成立跳出迴圈
    dumpInstructions(m_output_file.get(), "    beqz t0, %s\n", label_exit.c_str());

    // 拜訪迴圈主體
    const_cast<CompoundStatementNode &>(p_while.getBody()).accept(*this);

    // 跳回起點重新檢查條件
    dumpInstructions(m_output_file.get(), "    j %s\n", label_start.c_str());

    // 迴圈結束標籤
    dumpInstructions(m_output_file.get(), "%s:\n", label_exit.c_str());
}

void CodeGenerator::visit(ForNode &p_for) {
    // 進入 For Scope
    m_symbol_manager.pushScope(std::move(m_symbol_table_of_scoping_nodes.at(&p_for)));

    std::string label_start = "L" + std::to_string(m_label_count++);
    std::string label_exit = "L" + std::to_string(m_label_count++);

    const_cast<DeclNode &>(p_for.getLoopVarDecl()).accept(*this); 
    
    const_cast<AssignmentNode &>(p_for.getInitStmt()).accept(*this); // Push init value
    this->m_is_lvalue = true;
    const_cast<AssignmentNode &>(p_for.getInitStmt()).accept(*this); // Push address of i
    this->m_is_lvalue = false;

    dumpInstructions(m_output_file.get(), "%s:\n", label_start.c_str());
    // 條件判斷：檢查 i <= end_condition
    // 取得 i 的當前數值
    this->m_is_lvalue = false;
    const_cast<VariableReferenceNode &>(p_for.getInitStmt().getLvalue()).accept(*this);
    
    // 取得終止條件的值
    // 取得 end_expr 的值
    const_cast<ExpressionNode &>(p_for.getEndCondition()).accept(*this);
    
    // 執行 <= 比較
    dumpInstructions(m_output_file.get(), "    lw t1, 0(sp)\n"); // t1 = end value
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");
    dumpInstructions(m_output_file.get(), "    lw t0, 0(sp)\n"); // t0 = i value
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");
    dumpInstructions(m_output_file.get(), "    slt t2, t0, t1\n"); // t2 = (i < end)
    dumpInstructions(m_output_file.get(), "    beqz t2, %s\n", label_exit.c_str());

    const_cast<CompoundStatementNode &>(p_for.getBody()).accept(*this);

    const std::string &var_name = p_for.getInitStmt().getLvalue().getName();
    const SymbolEntry *entry = m_symbol_manager.lookup(var_name);
    
    if (entry) {
        int offset = entry->getOffset();
        dumpInstructions(m_output_file.get(), "    lw t0, %d(s0)\n", offset);
        dumpInstructions(m_output_file.get(), "    addi t0, t0, 1\n");
        dumpInstructions(m_output_file.get(), "    sw t0, %d(s0)\n", offset);
    }

    dumpInstructions(m_output_file.get(), "    j %s\n", label_start.c_str());
    dumpInstructions(m_output_file.get(), "%s:\n", label_exit.c_str());

    m_symbol_manager.popScope();
}

void CodeGenerator::visit(ReturnNode &p_return) {
    p_return.visitChildNodes(*this);

    dumpInstructions(m_output_file.get(), "    lw a0, 0(sp)\n");
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");
    dumpInstructions(m_output_file.get(), "    j %s_end\n", this->m_current_function_name.c_str());
}
