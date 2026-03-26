#include "sema/SymbolTable.hpp"
#include <iostream>
#include <iomanip>
#include <cstdio>

extern int opt_dump;

const char* SymbolEntry::getKindString() const {
    switch (kind) {
        case SymbolKind::program:   return "program";
        case SymbolKind::function:  return "function";
        case SymbolKind::parameter: return "parameter";
        case SymbolKind::variable:  return "variable";
        case SymbolKind::loop_var:  return "loop_var";
        case SymbolKind::constant:  return "constant";
        default: return "error";
    }
}

std::string SymbolEntry::getTypeString() const {
    if (type) {
        return std::string(type->getPTypeCString());
    }
    return "void";
}


void SymbolTable::addSymbol(const SymbolEntry& entry) {
    entries.push_back(entry);
}

bool SymbolTable::hasSymbol(const std::string& name) {
    for (const auto& entry : entries) {
        if (entry.name == name) {
            return true;
        }
    }
    return false;
}


SymbolManager::SymbolManager() {
}

void SymbolManager::pushScope() {
    tables.emplace_back();
    current_level++;
}

void SymbolManager::popScope() {
    if (tables.empty()) return;

    if (opt_dump){
    const auto& currentTable = tables.back();
    
    dumpDemarcation('=');
    printf("%-33s%-11s%-11s%-17s%-11s\n", "Name", "Kind", "Level", "Type", "Attribute");
    dumpDemarcation('-');

    for (const auto& entry : currentTable.entries) {
        printf("%-33s", entry.name.c_str());
        printf("%-11s", entry.getKindString());

        if (entry.level == 0) {
            printf("%d%-10s", 0, "(global)");
        } else {
            printf("%d%-10s", entry.level, "(local)");
        }

        printf("%-17s", entry.getTypeString().c_str());
        printf("%-11s", entry.attribute.c_str());
        puts("");
    }
    
    dumpDemarcation('-');
    } 
    tables.pop_back();
    current_level--;

}

bool SymbolManager::addSymbol(const std::string& name, SymbolKind kind, PTypeSharedPtr type, const std::string& attr) {
    if (tables.empty()) return false;

    SymbolTable& currentScope = tables.back();
    if (currentScope.hasSymbol(name)) {
        return false; 
    }

    for (const auto& table : tables) {
        for (const auto& entry : table.entries) {
            if (entry.name == name && entry.kind == SymbolKind::loop_var) {
                return false;
            }
        }
    }

    currentScope.addSymbol(SymbolEntry(name, kind, current_level, type, attr));
    return true;
}

SymbolEntry* SymbolManager::lookup(const std::string& name) {
    for (auto it = tables.rbegin(); it != tables.rend(); ++it) {
        for (auto& entry : it->entries) {
            if (entry.name == name) {
                return &entry;
            }
        }
    }
    return nullptr;
}

void SymbolManager::dumpDemarcation(const char chr) {
    for (size_t i = 0; i < 110; ++i) {
        printf("%c", chr);
    }
    puts("");
}
