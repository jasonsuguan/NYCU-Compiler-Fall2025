#ifndef SEMA_SYMBOL_TABLE_H
#define SEMA_SYMBOL_TABLE_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include "AST/PType.hpp"

enum class SymbolKind {
    program,
    function,
    parameter,
    variable,
    loop_var,
    constant
};


class SymbolEntry {
public:
    std::string name;
    SymbolKind kind;
    int level;
    PTypeSharedPtr type;
    std::string attribute;
    bool is_error = false;

public:
    SymbolEntry(const std::string &p_name, SymbolKind p_kind, int p_level,
                PTypeSharedPtr p_type, const std::string &p_attribute)
        : name(p_name), kind(p_kind), level(p_level), type(std::move(p_type)),
          attribute(p_attribute) {}

    // 多重錯誤標記
    void setError() { is_error = true; }
    bool hasError() const { return is_error; }

    // 儲存參數型態列表 (Function 用)
    std::vector<PTypeSharedPtr> parameter_types;

    const char* getKindString() const;
    std::string getTypeString() const;
};

class SymbolTable {
public:
    std::vector<SymbolEntry> entries;

    void addSymbol(const SymbolEntry& entry);
    // 在當前這個 Table 找符號 (用於檢查重複宣告)
    bool hasSymbol(const std::string& name); 
};

class SymbolManager {
private:
    std::vector<SymbolTable> tables;// stack作符號表堆積
    int current_level = -1; // scope level初始化

    void dumpDemarcation(const char chr);

public:
    SymbolManager(); 
    
    void pushScope();
    void popScope(); 

    bool addSymbol(const std::string& name, SymbolKind kind, PTypeSharedPtr type, const std::string& attr);
    
    SymbolEntry* lookup(const std::string& name);
    
    int getCurrentLevel() const { return current_level; }
};

#endif