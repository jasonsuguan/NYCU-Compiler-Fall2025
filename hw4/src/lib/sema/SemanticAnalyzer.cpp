#include "sema/Error.hpp"
#include "sema/SemanticAnalyzer.hpp"
#include "visitor/AstNodeInclude.hpp"

void SemanticAnalyzer::visit(ProgramNode &p_program) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    // Global Scope
    m_symbol_manager.pushScope();
    auto voidType = std::make_shared<PType>(PType::PrimitiveTypeEnum::kVoidType);
    
    //    注意：這裡不需要檢查重複宣告，因為這是第一個符號
    m_symbol_manager.addSymbol(
        p_program.getNameCString(), 
        SymbolKind::program, 
        voidType, 
        ""
    );
    m_return_type_stack.push(voidType);
    p_program.visitChildNodes(*this);
    m_return_type_stack.pop();
    m_symbol_manager.popScope();
}

void SemanticAnalyzer::visit(DeclNode &p_decl) {
    for (auto &var_node : p_decl.getVariables()) {
        std::string name = var_node->getNameCString();
        std::string type = var_node->getTypeCString();
        // Variable or Constant
        SymbolKind kind = SymbolKind::variable;
        std::string attri = "";
        
        auto constant_ptr = var_node->getConstantValueNode();
        if (auto constant_ptr = var_node->getConstantValueNode()) {
            kind = SymbolKind::constant;
            attri = constant_ptr->getConstantValueCString(); // 取得常數值字串 (需確認 ConstantValueNode 有此函式)
        }

        bool success = m_symbol_manager.addSymbol(
            name,
            kind,
            var_node->m_type,
            attri
        );

        if (!success) {
            m_error_printer.print(SymbolRedeclarationError(var_node->getLocation(), name));
            m_has_error = true;
            continue; 
        }
        for (auto dim : var_node->m_type->getDimensions()) {
            if (dim <= 0) {
                m_error_printer.print(NonPositiveArrayDimensionError(var_node->getLocation(), name));
                m_has_error = true;

                SymbolEntry* entry = m_symbol_manager.lookup(name);
                if (entry) {
                    entry->setError();
                }
            }
        }
    }
}

void SemanticAnalyzer::visit(VariableNode &p_variable) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
}

void SemanticAnalyzer::visit(ConstantValueNode &p_constant_value) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_constant_value.setType(p_constant_value.getTypeSharedPtr());
}

void SemanticAnalyzer::visit(FunctionNode &p_function) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    std::string attr = "";
    bool first = true;
    std::vector<PTypeSharedPtr> params;
    for (const auto &decl : p_function.m_parameters) {
        for (const auto &var : decl->getVariables()) {
            if (!first) attr += ", ";
            attr += var->getTypeCString();
            first = false;
            params.push_back(var->getType());
        }
    }

    PTypeSharedPtr returnType = std::make_shared<PType>(*p_function.m_ret_type);

    bool success = m_symbol_manager.addSymbol(
        p_function.m_name,
        SymbolKind::function,
        returnType,
        attr
    );

    if (success) {
        // 參數型態列表存進 SymbolEntry
        SymbolEntry* entry = m_symbol_manager.lookup(p_function.m_name);
        if (entry) {
            entry->parameter_types = params;
        }
    } else {
        m_error_printer.print(SymbolRedeclarationError(p_function.getLocation(), p_function.getNameCString()));
        m_has_error = true;
    }

    m_return_type_stack.push(returnType);
    m_symbol_manager.pushScope();

    for (const auto &decl : p_function.m_parameters) {
        for (const auto &var : decl->getVariables()) {
            // 加入符號，Kind 強制設為 parameter
            bool param = m_symbol_manager.addSymbol(
                var->getNameCString(),
                SymbolKind::parameter,
                var->m_type,
                ""
            );
            
            if (!param) {
                m_error_printer.print(SymbolRedeclarationError(var->getLocation(), var->getNameCString()));
                m_has_error = true;
            }

            for (auto dim :  var->m_type->getDimensions()) {
                if (dim == 0) {
                    // 維度必須 > 0
                    m_error_printer.print(NonPositiveArrayDimensionError(var->getLocation(), var->getNameCString()));
                    m_has_error = true;
                    
                    SymbolEntry* entry = m_symbol_manager.lookup(var->getNameCString());
                    if (entry) entry->setError();
                }
            }
        }
    }

    // 要讓 Body 內的變數跟參數在同一層
    // 所以要直接呼叫 visitChildNodes，繞過 CompoundStatement 的 visit。
    
    if (p_function.m_body) { 
        p_function.m_body->visitChildNodes(*this);
    }

    m_return_type_stack.pop();
    m_symbol_manager.popScope();
    
}

void SemanticAnalyzer::visit(CompoundStatementNode &p_compound_statement) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    m_symbol_manager.pushScope();
    p_compound_statement.visitChildNodes(*this);
    m_symbol_manager.popScope();
}

void SemanticAnalyzer::visit(PrintNode &p_print) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_print.visitChildNodes(*this);
    // 回傳 ExpressionNode*
    auto target = p_print.getTarget(); 

    // 避免重複錯誤
    if (!target || !target->getType()) return;

    // 檢查是否為純量
    bool is_scalar = target->getType()->getDimensions().empty() && target->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kVoidType;

    if (!is_scalar) {
        m_error_printer.print(PrintOutNonScalarTypeError(target->getLocation()));
        m_has_error = true;
    }
}

void SemanticAnalyzer::visit(BinaryOperatorNode &p_bin_op) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_bin_op.visitChildNodes(*this);

    auto left = p_bin_op.getLeftOperand();
    auto right = p_bin_op.getRightOperand();
    
    // 如果子節點有會沒有型態，就直接跳過不然會有nullptr指向錯誤
    if (!left->getType() || !right->getType()) {
        return; 
    }

    size_t dim_left = left->getType()->getDimensions().size();
    size_t dim_right = right->getType()->getDimensions().size();
    
    bool is_left_scalar = (dim_left == 0);
    bool is_right_scalar = (dim_right == 0);
    PType::PrimitiveTypeEnum t1 = left->getType()->getPrimitiveType();
    PType::PrimitiveTypeEnum t2 = right->getType()->getPrimitiveType();
    std::string op = p_bin_op.getOpCString();
    
    PTypeSharedPtr resultType = nullptr; // 用來存計算後的結果型態

    // string + string -> string
    if (op == "+" && t1 == PType::PrimitiveTypeEnum::kStringType && t2 == PType::PrimitiveTypeEnum::kStringType) {
        resultType = std::make_shared<PType>(PType::PrimitiveTypeEnum::kStringType);
    }

 
    // 數字對數字。
    else if (op == "+" || op == "-" || op == "*" || op == "/") {
        bool is_left_num = (t1 == PType::PrimitiveTypeEnum::kIntegerType || t1 == PType::PrimitiveTypeEnum::kRealType) && is_left_scalar;
        bool is_right_num = (t2 == PType::PrimitiveTypeEnum::kIntegerType || t2 == PType::PrimitiveTypeEnum::kRealType) && is_right_scalar;

        if (is_left_num && is_right_num) {
            // 決定結果型態
            if (t1 == PType::PrimitiveTypeEnum::kRealType || t2 == PType::PrimitiveTypeEnum::kRealType) {
                resultType = std::make_shared<PType>(PType::PrimitiveTypeEnum::kRealType);
            } else {
                resultType = std::make_shared<PType>(PType::PrimitiveTypeEnum::kIntegerType);
            }
        }
    }

    // integer mod integer -> integer
    else if (op == "mod") {
        if (t1 == PType::PrimitiveTypeEnum::kIntegerType && t2 == PType::PrimitiveTypeEnum::kIntegerType && is_left_scalar && is_right_scalar) {
            resultType = std::make_shared<PType>(PType::PrimitiveTypeEnum::kIntegerType);
        }
    }

    // 規則: 數字比數字 -> boolean
    else if (op == "<" || op == "<=" || op == "=" || op == ">=" || op == ">" || op == "<>") {
        bool is_left_num = (t1 == PType::PrimitiveTypeEnum::kIntegerType || t1 == PType::PrimitiveTypeEnum::kRealType) && is_left_scalar;
        bool is_right_num = (t2 == PType::PrimitiveTypeEnum::kIntegerType || t2 == PType::PrimitiveTypeEnum::kRealType) && is_right_scalar;
        
        if (is_left_num && is_right_num) {
            resultType = std::make_shared<PType>(PType::PrimitiveTypeEnum::kBoolType);
        }
    }

    // 規則: boolean and/or boolean -> boolean
    else if (op == "and" || op == "or") {
        if (t1 == PType::PrimitiveTypeEnum::kBoolType && t2 == PType::PrimitiveTypeEnum::kBoolType && is_left_scalar && is_right_scalar) {
            resultType = std::make_shared<PType>(PType::PrimitiveTypeEnum::kBoolType);
        }
    }

    if (resultType) {
        // 確定合法後設定此節點的型態
        p_bin_op.setType(resultType);
    } else {
        m_error_printer.print(InvalidBinaryOperandError(p_bin_op.getLocation(), p_bin_op.getOp(), left->getType().get(), right->getType().get()));
        m_has_error = true;
    }
}

void SemanticAnalyzer::visit(UnaryOperatorNode &p_un_op) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_un_op.visitChildNodes(*this);

    auto operand = p_un_op.getOperand();
    
    // 如果子節點有錯不會有回傳型態，就要直接跳過
    if (!operand->getType()) return;

    auto type = operand->getType();
    PType::PrimitiveTypeEnum t = type->getPrimitiveType();
    
    bool is_scalar = type->getDimensions().empty();

    std::string op_str = p_un_op.getOpCString(); 
    PTypeSharedPtr resultType = nullptr;
    Operator op = p_un_op.getOp(); 

    if (op == Operator::kNegOp) {
        if (is_scalar && (t == PType::PrimitiveTypeEnum::kIntegerType || t == PType::PrimitiveTypeEnum::kRealType)) {
            resultType = std::make_shared<PType>(t);
        }
    }

    else if (op == Operator::kNotOp) {
        if (is_scalar && t == PType::PrimitiveTypeEnum::kBoolType) {
            resultType = std::make_shared<PType>(PType::PrimitiveTypeEnum::kBoolType);
        }
    }

    if (resultType) {
        p_un_op.setType(resultType);
    } else {     
        m_error_printer.print(InvalidUnaryOperandError(
            p_un_op.getLocation(), 
            p_un_op.getOp(),
            type.get()
        ));
        
        m_has_error = true;
    }
}

void SemanticAnalyzer::visit(FunctionInvocationNode &p_func_invocation) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    SymbolEntry* entry = m_symbol_manager.lookup(p_func_invocation.getNameCString());
    std::vector<PTypeSharedPtr> params;

    if (!entry) {
        m_error_printer.print(UndeclaredSymbolError(p_func_invocation.getLocation(), p_func_invocation.getNameCString()));
        m_has_error = true;
        return;
    }

    // 檢查種類 Function
    if (entry->kind != SymbolKind::function) {
        m_error_printer.print(NonFunctionSymbolError(p_func_invocation.getLocation(), p_func_invocation.getNameCString()));
        m_has_error = true;
        return;
    }

    // 參數檢查
    // 遍歷引數 (Arguments) 讓它們計算自己的型態
    for (auto &arg : p_func_invocation.getArgs()) {
        arg->accept(*this);
    }

    const auto& formal_params = entry->parameter_types; // 預期的參數列表
    const auto& actual_args = p_func_invocation.getArgs(); // 實際傳入的參數
    // 檢查數量
    if (actual_args.size() != formal_params.size()) {
        m_error_printer.print(ArgumentNumberMismatchError(
            p_func_invocation.getLocation(), 
            p_func_invocation.getNameCString()
        ));
        m_has_error = true;
        return; 
    }

    // B. 檢查型態
    for (size_t i = 0; i < actual_args.size(); ++i) {
        auto arg_expr = actual_args[i].get();
        
        if (!arg_expr->getType()) continue;

        PTypeSharedPtr expected_type = formal_params[i];
        PTypeSharedPtr actual_type = arg_expr->getType();

        std::string str_expected = expected_type->getPTypeCString();
        std::string str_actual = actual_type->getPTypeCString();

        bool is_compatible = false;
        // 精確比對
        if (str_expected == str_actual) {
            is_compatible = true;
        }
        // 例外: Int -> Real (僅限 Scalar)
        else if (expected_type->getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType &&
                 actual_type->getPrimitiveType() == PType::PrimitiveTypeEnum::kIntegerType &&
                 expected_type->getDimensions().empty() &&
                 actual_type->getDimensions().empty()) {
            is_compatible = true;
        }

        if (!is_compatible) {
            m_error_printer.print(IncompatibleArgumentTypeError(
                arg_expr->getLocation(), 
                expected_type.get(),
                actual_type.get()
            ));
            m_has_error = true;
            return;
        }
    }
    // 把函式的回傳型態 (Return Type) 設定給這個節點
    p_func_invocation.setType(entry->type);
}

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    SymbolEntry* entry = m_symbol_manager.lookup(p_variable_ref.getNameCString());

    if (!entry) {
        m_error_printer.print(UndeclaredSymbolError(p_variable_ref.getLocation(), p_variable_ref.getNameCString()));
        m_has_error = true;
        return; 
    }

    if (entry->kind == SymbolKind::program || entry->kind == SymbolKind::function) {
        m_error_printer.print(NonVariableSymbolError(p_variable_ref.getLocation(), p_variable_ref.getNameCString())); // 請確認你的 Error 類別名稱
        m_has_error = true;
        return;
    }

    // 處理陣列
    // 先遍歷所有的 index 表達式，計算它們的型態 (Type Propagation)
    for (auto &index : p_variable_ref.getIndices()) {
        index->accept(*this);
        
    }

    if (entry->hasError()) {
        return; // 如果宣告時有錯，後續檢查就不做
    }
    // 檢查索引型態
    for (auto &index : p_variable_ref.getIndices()) {
        // Index 內部的運算 已經出錯了就跳過檢查
        if (!index->getType()) {
            return; 
        }

        if (index->getTypeSharedPtr()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType) {
            m_error_printer.print(NonIntegerArrayIndexError(index->getLocation()));
            m_has_error = true;
            return;
        }
    }

    // 6. 檢查維度過多
    auto original_dims = entry->type->getDimensions();
    size_t decl_dim_count = entry->type->getDimensions().size();
    size_t ref_dim_count = p_variable_ref.getIndices().size();

    if (ref_dim_count > decl_dim_count) {
         m_error_printer.print(OverArraySubscriptError(p_variable_ref.getLocation(), p_variable_ref.getNameCString()));
         m_has_error = true;
         return;
    }

    // 維度縮減 (Type Propagation)

    PTypeSharedPtr new_type = std::make_shared<PType>(entry->type->getPrimitiveType());

    // 計算剩餘維度
    std::vector<uint64_t> new_dims;
    if (ref_dim_count < decl_dim_count) {
        for (size_t i = ref_dim_count; i < decl_dim_count; ++i) {
            new_dims.push_back(original_dims[i]);
        }
    }

    if (!new_dims.empty()) {
        new_type->setDimensions(new_dims);
    }

    p_variable_ref.setType(new_type);
}

void SemanticAnalyzer::visit(AssignmentNode &p_assignment) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_assignment.visitChildNodes(*this);

    auto lvalue = p_assignment.getLvalue(); // VariableReferenceNode*
    auto expr = p_assignment.getExpr();     // ExpressionNode*

    // Lvalue 不能是常數或迴圈
    SymbolEntry* entry = m_symbol_manager.lookup(lvalue->getNameCString());
    
    if (entry) {
        if (entry->kind == SymbolKind::constant) {
            m_error_printer.print(AssignToConstantError(lvalue->getLocation(), lvalue->getNameCString()));
            m_has_error = true;
            return;
        }
        else if (entry->kind == SymbolKind::loop_var) {
            m_error_printer.print(AssignToLoopVarError(lvalue->getLocation()));
            m_has_error = true;
            return;
        }
    }

    // Lvalue 不能是陣列型態
    
    if (lvalue->getType() && !lvalue->getType()->getDimensions().empty()) {
        m_error_printer.print(AssignToArrayTypeError(lvalue->getLocation()));
        m_has_error = true;
        return;
    }
    if (!lvalue->getType() || !expr->getType()) return;

    // Expression rvlue 不能是陣列型態
    if (!expr->getType()->getDimensions().empty()) {
        m_error_printer.print(AssignByArrayTypeError(expr->getLocation()));
        m_has_error = true;
        return;
    }

    // (Type Compatibility)
    PType::PrimitiveTypeEnum t_l = lvalue->getType()->getPrimitiveType();
    PType::PrimitiveTypeEnum t_r = expr->getType()->getPrimitiveType();
    
    bool is_compatible = false;

    if (t_l == t_r) {
        is_compatible = true;
    }
    //  Real := Integer
    else if (t_l == PType::PrimitiveTypeEnum::kRealType && t_r == PType::PrimitiveTypeEnum::kIntegerType) {
        is_compatible = true;
    }

    if (!is_compatible) {
        m_error_printer.print(IncompatibleAssignmentError(
            p_assignment.getLocation(),
            lvalue->getType().get(),
            expr->getType().get()
        ));
        m_has_error = true;
    }
}

void SemanticAnalyzer::visit(ReadNode &p_read) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_read.visitChildNodes(*this);

    auto target = p_read.getTarget();

    // 防呆：如果子節點已經報錯 (例如變數沒宣告)，型態通常是 null，直接跳過
    if (!target || !target->getType()) return;

    //  必須是純量 
    bool is_scalar = target->getType()->getDimensions().empty();
    
    if (!is_scalar) {
        m_error_printer.print(ReadToNonScalarTypeError(target->getLocation()));
        m_has_error = true;
    }

    // 查表來得知它是 constant 還是 loop_var
    SymbolEntry* entry = m_symbol_manager.lookup(target->getNameCString());
    
    if (entry) {
        if (entry->kind == SymbolKind::constant || entry->kind == SymbolKind::loop_var) {
            m_error_printer.print(ReadToConstantOrLoopVarError(target->getLocation()));
            m_has_error = true;
        }
    }
}

void SemanticAnalyzer::visit(IfNode &p_if) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_if.visitChildNodes(*this);

    // 取得條件式節點
    auto condition = p_if.getCondition();

    if (!condition->getType()) return;

    // 檢查是否為 Boolean
    if (condition->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType) {
        m_error_printer.print(NonBooleanConditionError(condition->getLocation()));
        m_has_error = true;
    }
}

void SemanticAnalyzer::visit(WhileNode &p_while) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_while.visitChildNodes(*this);
    auto condition = p_while.getCondition();
    if (!condition->getType()) return;

    // 3. 檢查條件式是否為 Boolean
    if (condition->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType) {
        m_error_printer.print(NonBooleanConditionError(condition->getLocation()));
        m_has_error = true;
    }
}

void SemanticAnalyzer::visit(ForNode &p_for) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    m_symbol_manager.pushScope();

    if (auto decl = p_for.m_loop_var_decl.get()) {
        for (auto &var : decl->getVariables()) {
            std::string name = var->getNameCString();
            
            // 加入符號表 (Kind = loop_var)
            bool success = m_symbol_manager.addSymbol(
                name,
                SymbolKind::loop_var,
                var->m_type,
                ""
            );
            
            if (!success) {
                m_error_printer.print(SymbolRedeclarationError(var->getLocation(), name));
                m_has_error = true;
            }
        }
    }


    if (auto init = p_for.m_init_stmt.get()) {
        auto lvalue = init->getLvalue();
        auto expr = init->getExpr();

        lvalue->accept(*this);
        expr->accept(*this);
        if (expr->getType()) {
             // 檢查右邊是否為 Integer
             if (expr->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType) {
                 m_error_printer.print(IncompatibleAssignmentError(
                     init->getLocation(),
                     lvalue->getType().get(),
                     expr->getType().get()
                 ));
             }
        }
    }

    if (auto condition = p_for.m_end_condition.get()) {
        condition->accept(*this);
    }

    if (p_for.m_init_stmt && p_for.m_end_condition) {
        auto init_expr = p_for.m_init_stmt->getExpr();
        auto end_expr = p_for.m_end_condition.get();

        auto start_const = dynamic_cast<ConstantValueNode*>(init_expr);
        auto end_const = dynamic_cast<ConstantValueNode*>(end_expr);

        if (start_const && end_const) {
            if (start_const->getType()->getPrimitiveType() == PType::PrimitiveTypeEnum::kIntegerType &&
                end_const->getType()->getPrimitiveType() == PType::PrimitiveTypeEnum::kIntegerType) {
                

                int start_val = std::stoi(start_const->getConstantValueCString());
                int end_val = std::stoi(end_const->getConstantValueCString());

                if (start_val > end_val) {
                    m_error_printer.print(NonIncrementalLoopVariableError(p_for.getLocation()));
                    m_has_error = true;
                }
            }
        }
    }
    if (auto body = p_for.m_body.get()) {
        body->accept(*this);
    }
    m_symbol_manager.popScope();
}

void SemanticAnalyzer::visit(ReturnNode &p_return) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_return.visitChildNodes(*this);

    auto retval = p_return.getReturnValue(); // ExpressionNode*

    if (!retval->getType()) return;
    if (m_return_type_stack.empty()) return; // 理論上不會發生

    // 取得回傳型態
    PTypeSharedPtr expected_type = m_return_type_stack.top();
    PTypeSharedPtr actual_type = retval->getType();

    bool is_compatible = false;
    
    PType::PrimitiveTypeEnum type_e = expected_type->getPrimitiveType();
    PType::PrimitiveTypeEnum type_a = actual_type->getPrimitiveType();
    const auto& dim_e = expected_type->getDimensions();
    const auto& dim_a = actual_type->getDimensions();

    if (type_e == PType::PrimitiveTypeEnum::kVoidType) {
        m_error_printer.print(ReturnFromVoidError(
            p_return.getLocation()
        ));
        is_compatible = false;
        return;
    }

    else if (dim_e == dim_a) {
        if (type_e == type_a) {
            is_compatible = true;
        }
        else if (type_e == PType::PrimitiveTypeEnum::kRealType &&
                 type_a == PType::PrimitiveTypeEnum::kIntegerType) {
            is_compatible = true;
        }
    }

    if (!is_compatible) {
        m_error_printer.print(IncompatibleReturnTypeError(
            retval->getLocation(),
            expected_type.get(),
            actual_type.get()
        ));
        m_has_error = true;
    }
}
