%{
#include "AST/BinaryOperator.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/UnaryOperator.hpp"
#include "AST/VariableReference.hpp"
#include "AST/assignment.hpp"
#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/expression.hpp"
#include "AST/for.hpp"
#include "AST/function.hpp"
#include "AST/if.hpp"
#include "AST/print.hpp"
#include "AST/program.hpp"
#include "AST/read.hpp"
#include "AST/return.hpp"
#include "AST/variable.hpp"
#include "AST/while.hpp"
#include "AST/AstDumper.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#define YYLTYPE yyltype

typedef struct YYLTYPE {
    uint32_t first_line;
    uint32_t first_column;
    uint32_t last_line;
    uint32_t last_column;
} yyltype;

extern uint32_t line_num;   /* declared in scanner.l */
extern char current_line[]; /* declared in scanner.l */
extern FILE *yyin;          /* declared by lex */
extern char *yytext;        /* declared by lex */

static AstNode *root;

extern "C" int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);
%}

// This guarantees that headers do not conflict when included together.
%define api.token.prefix {TOK_}

%code requires {
    #include <vector>
    class AstNode;
    class CompoundStatementNode;

    class DeclNode;
    enum class VarType;
    enum class DeclType;
    using NodeList = std::vector<AstNode*>; //方便後面可以使用 NodeList
    using DeclList = std::vector<DeclNode*>;
    class VariableNode;
    

    class FunctionNode;
    using FunctionList = std::vector<FunctionNode*>;

    struct PType {
        VarType base;                  // e.g., IntType / RealType / StringType / BoolType
        std::vector<uint32_t> dims;    // e.g., array 3 of array 2 of <base>
    };
}

    /* For yylval */
%union {
    /* basic semantic value */
    char *identifier;
    CompoundStatementNode *compound_stmt_ptr;
    DeclNode *decl_ptr;
    VarType var_type;
    NodeList *node_list;
    VariableNode *variable_ptr;
    AstNode *node;
    PType *ptype;
    std::vector<uint32_t> *dim_list;

    FunctionNode *func_ptr;
    FunctionList *func_list;
    DeclList     *decl_list;

    int    ival;    /* INT_LITERAL */
    double fval;    /* REAL_LITERAL */
    char  *sval;    /* STRING_LITERAL */
};

%type <identifier> ProgramName ID FunctionName
%type <var_type> ReturnType
%type <compound_stmt_ptr> CompoundStatement
%type <decl_ptr> Declaration
%type <node_list> IdList DeclarationList Declarations StatementList Statements ArrRefList ArrRefs ExpressionList Expressions
%type <node> LiteralConstant IntegerAndReal StringAndBoolean
%type <ptype> Type ScalarType ArrType
%type <dim_list> ArrDecl

%type <func_list> FunctionList Functions 
%type <func_ptr>  Function FunctionDeclaration FunctionDefinition
%type <decl_list> FormalArgList FormalArgs 
%type <decl_ptr>  FormalArg 
%type <node> Statement Simple Condition ElseOrNot While For Return FunctionCall FunctionInvocation Expression
%type <node> VariableReference
%type <ival> NegOrNot
    /* Follow the order in scanner.l */

    /* Delimiter */
%token COMMA SEMICOLON COLON
%token L_PARENTHESIS R_PARENTHESIS
%token L_BRACKET R_BRACKET

    /* Operator */
%token ASSIGN
    /* TODO: specify the precedence of the following operators */
%token MULTIPLY DIVIDE MOD
%token LESS LESS_OR_EQUAL EQUAL GREATER GREATER_OR_EQUAL NOT_EQUAL
%token OR AND NOT
%token PLUS MINUS

    /* Keyword */
%token ARRAY BOOLEAN INTEGER REAL STRING
%token END BEGIN
%token DO ELSE FOR IF THEN WHILE
%token DEF OF TO RETURN VAR
%token FALSE TRUE
%token PRINT READ

    /* Identifier */
%token ID

    /* Literal */
// %token INT_LITERAL
// %token REAL_LITERAL
// %token STRING_LITERAL
/* Literal */
%token <ival>   INT_LITERAL
%token <fval>   REAL_LITERAL
%token <sval>   STRING_LITERAL

%left OR
%left AND
%left EQUAL NOT_EQUAL LESS LESS_OR_EQUAL GREATER GREATER_OR_EQUAL
%left PLUS MINUS
%left MULTIPLY DIVIDE MOD
%right NOT
%right UMINUS
%%

ProgramUnit:
    Program
    |
    Function
;

Program:
    ProgramName SEMICOLON
    /* ProgramBody */
    DeclarationList FunctionList CompoundStatement
    /* End of ProgramBody */
    END {
        root = new ProgramNode(@1.first_line, @1.first_column,
                               $1, $3, $4, $5);
        free($1);
    }
;

ProgramName:
    ID {
        $$ = $1;
    }
;

DeclarationList:
    Epsilon {
        $$ = new NodeList();
    }
    |
    Declarations{
        $$ = $1;
    }
;

Declarations:
    Declaration{
        $$ = new NodeList();
        $$->push_back($1);
    }
    |
    Declarations Declaration{
        $$ = $1;
        $$->push_back($2);
    }
;

FunctionList:
    Epsilon{
        $$ = new FunctionList();
    }
    |
    Functions{
        $$ = $1;
    }
;

Functions:
    Function{
        $$ = new FunctionList();
        $$->push_back($1);
    }
    |
    Functions Function{
        $$ = $1;
        $$->push_back($2);
    }
;

Function:
    FunctionDeclaration
    |
    FunctionDefinition
;

FunctionDeclaration:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType SEMICOLON
        {
        // $1: char*, $3: NodeList*(DeclNode*...), $5: VarType
        $$ = new FunctionNode(@1.first_line, @1.first_column,
                              $1, $5,
                              $3, nullptr);
        free($1);
    }
;

FunctionDefinition:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType
    CompoundStatement
    END {
        $$ = new FunctionNode(@1.first_line, @1.first_column,
                              $1, $5,
                              $3, $6);
        free($1);
    }
;

FunctionName:
    ID {
        $$ = $1;
    }
;

FormalArgList:
    Epsilon{
        $$ = new DeclList();
    }
    |
    FormalArgs{
        $$ = $1;
    }
;

FormalArgs:
    FormalArg{
        $$ = new DeclList();
        $$->push_back($1);
    }
    |
    FormalArgs SEMICOLON FormalArg{
        $$ = $1;
        $$->push_back($3); // 功能：把多個 FormalArg 加入同一個列表
    }
;

FormalArg:
    IdList COLON Type{
        for (auto *n : *$1){
            auto *var_node = static_cast<VariableNode*>(n);
            var_node->setType($3->base);
            var_node->setArrayDims($3->dims);
        }
        $$ = new DeclNode(@1.first_line, @1.first_column, DeclType::Var, *$1);
        delete $1;
    }
;

IdList:
    ID{
        $$ = new NodeList();
        $$->push_back(new VariableNode(@1.first_line, @1.first_column, $1, VarType::Unknown, nullptr, false));
        free($1);
    } //這裏一開始忘記也賦予$$的值 導致出現爆錯 declaration list 為空
    |
    IdList COMMA ID{
        $$ = $1;
        $$->push_back(new VariableNode(@3.first_line, @3.first_column, $3, VarType::Unknown, nullptr, false));
        free($3);
    }
;

ReturnType:
    COLON ScalarType {
        $$ = $2->base;
        delete $2;
    }
    |
    Epsilon {
        $$ = VarType::VoidType;
    }
;

    /*
       Data Types and Declarations
                                   */

Declaration:
    VAR IdList COLON Type SEMICOLON {
        for (auto *n : *$2){
            auto *var_node = static_cast<VariableNode*>(n);
            var_node->setType($4->base);
            var_node->setArrayDims($4->dims);
        }
        $$ = new DeclNode (@1.first_line, @1.first_column, DeclType::Var, *$2);
        delete $2;
        delete $4;
    }
    |
    VAR IdList COLON LiteralConstant SEMICOLON
    {
        fflush(stdout);
        auto *cv = dynamic_cast<ConstantValueNode*>($4);
        VarType t = VarType::Unknown;
        if (cv) {
        const std::string &val = cv->getValue();
        if (val == "true" || val == "false") t = VarType::BoolType;
        else if (!val.empty() && val.front()=='"' && val.back()=='"') t = VarType::StringType;
        else if (val.find('.') != std::string::npos) t = VarType::RealType;
        else t = VarType::IntType;
        }
        for (auto *n : *$2) {
        auto *v = static_cast<VariableNode*>(n);
        v->setType(t);
        v->setInitValue($4);        /* $4: AstNode* (ConstantValueNode*) */
        v->setIsConst(true);
        v->setArrayDims({});
        }
    $$ = new DeclNode(@1.first_line, @1.first_column, DeclType::Const, *$2);
    delete $2;
    }
;

Type:
    ScalarType{
        $$ = $1;
    }
    |
    ArrType{
        $$ = $1;
    }
;

ScalarType:
    INTEGER { $$ = new PType{ VarType::IntType, {} }; }
    |
    REAL { $$ = new PType{ VarType::RealType, {} }; }
    |
    STRING {
        $$ = new PType{ VarType::StringType, {} };
    }
    |
    BOOLEAN {
        $$ = new PType{ VarType::BoolType, {} };
    }
;

ArrType:
    ArrDecl ScalarType{
        // 把 scalar 與 ArrDecl 維度結合
        $$ = new PType{ $2->base, *$1 };
        delete $1;
        delete $2;
    }
;

ArrDecl:
    ARRAY INT_LITERAL OF{
        $$ = new std::vector<uint32_t>();
        $$->push_back(static_cast<uint32_t>($2));  // first dim
    }
    |
    ArrDecl ARRAY INT_LITERAL OF{
        $$ = $1;
        $$->push_back(static_cast<uint32_t>($3));  // add second dim
    }
;

LiteralConstant:
    NegOrNot INT_LITERAL{
         // $1: +1/-1, $2: int ival
        int v = static_cast<int>($2);
        if ($1 == -1) v = -v;
        uint32_t line = @2.first_line;
        uint32_t col  = ($1 == -1) ? @1.first_column : @2.first_column;
        $$ = new ConstantValueNode(line, col, std::to_string(v));
    }
    |
    NegOrNot REAL_LITERAL{
        double v = $2;
          if ($1 == -1) v = -v;
        uint32_t line = @2.first_line;
        uint32_t col  = ($1 == -1) ? @1.first_column : @2.first_column;
        $$ = new ConstantValueNode(line, col, std::to_string(v));
    }
    |
    StringAndBoolean {
        $$ = $1;
    }
;

NegOrNot:
    Epsilon{
        $$ = 1;
    }
    |
    MINUS{
        $$ = -1;
    }
;

StringAndBoolean:
    STRING_LITERAL{
      std::string tmp($1);
      $$ = new ConstantValueNode(@1.first_line, @1.first_column, tmp);
      free($1);
    }
    |
    TRUE{
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, std::string("true"));
    }
    |
    FALSE{
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, std::string("false"));
    }
;

IntegerAndReal:
    INT_LITERAL{
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, std::to_string($1));
    }
    |
    REAL_LITERAL{
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, std::to_string($1));
    }
;

    /*
       Statements
                  */

Statement:
    CompoundStatement{
        $$ = $1;
    }
    |
    Simple{
        $$ = $1;
    }
    |
    Condition{
        $$ = $1;     
    }
    |
    While{
        $$ = $1;
    }
    |
    For{
        $$ = $1;
    }
    |
    Return{
        $$ = $1;
    }
    |
    FunctionCall{
        $$ = $1;
    }
;

CompoundStatement:
    BEGIN
    DeclarationList
    StatementList
    END {
        $$ = new CompoundStatementNode(@1.first_line, @1.first_column, $2, $3);
    }
;

Simple:
    VariableReference ASSIGN Expression SEMICOLON{
        $$ = new AssignmentNode(@2.first_line, @2.first_column,
                                 static_cast<VariableReferenceNode*>($1),
                                 static_cast<ExpressionNode*>($3));
    }
    |
    PRINT Expression SEMICOLON
    {
        $$ = new PrintNode(@1.first_line, @1.first_column, static_cast<ExpressionNode*>($2));
    }
    |
    READ VariableReference SEMICOLON{
        $$ = new ReadNode(@1.first_line, @1.first_column,
                           static_cast<VariableReferenceNode*>($2));
    }
;

VariableReference:
    ID ArrRefList{
        NodeList *list = $2;
        std::vector<ExpressionNode*> indices;

        if (list) {
            for (auto it = list->begin(); it != list->end(); ++it) {
                auto *expr = dynamic_cast<ExpressionNode*>(*it);
                indices.push_back(expr);
            }
            delete list;

        $$ = new VariableReferenceNode(
                @1.first_line, @1.first_column,
                $1, indices);
        free($1);
        }
    }
;

ArrRefList:
    Epsilon{
        $$ = new NodeList();
    }
    |
    ArrRefs{
        $$ = $1;
    }
;

ArrRefs:
    L_BRACKET Expression R_BRACKET{
        $$ = new NodeList();
        $$->push_back($2); // first index
    }
    |
    ArrRefs L_BRACKET Expression R_BRACKET{
        $$ = $1;             
        $$->push_back($3); 
    }
;

Condition:
    IF Expression THEN
    CompoundStatement
    ElseOrNot
    END IF{
        $$ = new IfNode(@1.first_line, @1.first_column,
                         static_cast<ExpressionNode*>($2),
                         static_cast<CompoundStatementNode*>($4),
                         static_cast<CompoundStatementNode*>($5));
    }
;

ElseOrNot:
    ELSE
    CompoundStatement{
        $$ = $2;
    }
    |
    Epsilon{
        $$ = nullptr;
    }
;

While:
    WHILE Expression DO
    CompoundStatement
    END DO{
        $$ = new WhileNode(@1.first_line, @1.first_column,
                           static_cast<ExpressionNode*>($2),
                           static_cast<CompoundStatementNode*>($4));
    }
;

For:
    FOR ID ASSIGN INT_LITERAL TO INT_LITERAL DO
    CompoundStatement
    END DO{
        auto *var = new VariableNode(@2.first_line, @2.first_column,
                                     $2, VarType::IntType, nullptr, false);
        std::vector<AstNode*> vars;
        vars.push_back(var);
        auto *decl = new DeclNode(@2.first_line, @2.first_column,
                                  DeclType::Var, vars);

        auto *var_ref = new VariableReferenceNode(@2.first_line, @2.first_column,
                                                  $2, {});

        auto *init_const = new ConstantValueNode(@4.first_line, @4.first_column,
                                                 std::to_string($4));

        auto *assign = new AssignmentNode(@3.first_line, @3.first_column,
                                          var_ref, init_const);

        auto *end_const = new ConstantValueNode(@6.first_line, @6.first_column,
                                                std::to_string($6));
        $$ = new ForNode(@1.first_line, @1.first_column,
                         decl, assign, end_const,
                         $8);

        free($2);
    }
;

Return:
    RETURN Expression SEMICOLON{
        $$ = new ReturnNode(@1.first_line, @1.first_column,
                             static_cast<ExpressionNode*>($2));
    }
;

FunctionCall:
    FunctionInvocation SEMICOLON{
        $$ = $1;
    }
;

FunctionInvocation:
    ID L_PARENTHESIS ExpressionList R_PARENTHESIS{
        auto *list = $3; // NodeList*
        std::vector<ExpressionNode*> args;
        for (auto *n : *list) {
            args.push_back(dynamic_cast<ExpressionNode*>(n));
        }
        $$ = new FunctionInvocationNode(@1.first_line, @1.first_column,
                                         $1, std::move(args));
        delete list;
        free($1);
    }
;


ExpressionList:
    Epsilon{
        $$ = new NodeList();
    }
    |
    Expressions{
        $$ = $1;
    }
;

Expressions:
    Expression{
        $$ = new NodeList();
        $$->push_back($1);
    }
    |
    Expressions COMMA Expression{
        $$ = $1;
        $$->push_back($3);
    }
;

StatementList:
    Epsilon
    { $$ = new NodeList(); }
    |
    Statements
    { $$ = $1; }
;

Statements:
    Statement{
        $$ = new NodeList();
        $$->push_back($1);
    }
    |
    Statements Statement{
        $$ = $1;
        $$->push_back($2);
    }
;

Expression:
    L_PARENTHESIS Expression R_PARENTHESIS{
        $$ = $2;
    }
    |
    MINUS Expression %prec UMINUS{
        $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, "neg", dynamic_cast<ExpressionNode*>($2));
    }
    |
    Expression MULTIPLY Expression{
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "*", dynamic_cast<ExpressionNode*>($1), dynamic_cast<ExpressionNode*>($3));
    }
    |
    Expression DIVIDE Expression{
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "/", dynamic_cast<ExpressionNode*>($1), dynamic_cast<ExpressionNode*>($3));
    }
    |
    Expression MOD Expression{
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "mod", dynamic_cast<ExpressionNode*>($1), dynamic_cast<ExpressionNode*>($3));
    }
    |
    Expression PLUS Expression{
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "+", dynamic_cast<ExpressionNode*>($1), dynamic_cast<ExpressionNode*>($3));
    }
    |
    Expression MINUS Expression{
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "-", dynamic_cast<ExpressionNode*>($1), dynamic_cast<ExpressionNode*>($3));
    }
    |
    Expression LESS Expression{
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "<", dynamic_cast<ExpressionNode*>($1), dynamic_cast<ExpressionNode*>($3));
    }
    |
    Expression LESS_OR_EQUAL Expression{
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "<=", dynamic_cast<ExpressionNode*>($1), dynamic_cast<ExpressionNode*>($3));
    }
    |
    Expression GREATER Expression{
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, ">", dynamic_cast<ExpressionNode*>($1), dynamic_cast<ExpressionNode*>($3));
    }
    |
    Expression GREATER_OR_EQUAL Expression{
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, ">=", dynamic_cast<ExpressionNode*>($1), dynamic_cast<ExpressionNode*>($3));
    }
    |
    Expression EQUAL Expression{
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "=", dynamic_cast<ExpressionNode*>($1), dynamic_cast<ExpressionNode*>($3));
    }
    |
    Expression NOT_EQUAL Expression{
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "<>", dynamic_cast<ExpressionNode*>($1), dynamic_cast<ExpressionNode*>($3));
    }
    |
    NOT Expression{
        $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, "not", dynamic_cast<ExpressionNode*>($2));
    }
    |
    Expression AND Expression{
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "and", dynamic_cast<ExpressionNode*>($1), dynamic_cast<ExpressionNode*>($3));
    }
    |
    Expression OR Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "or", dynamic_cast<ExpressionNode*>($1), dynamic_cast<ExpressionNode*>($3));
    }
    |
    IntegerAndReal {
        $$ = $1;
    }
    |
    StringAndBoolean {
        $$ = $1;
    }
    |
    VariableReference{
        $$ = $1;    
    }
    |
    FunctionInvocation{
        $$ = $1;
    }
;

    /*
       misc
            */
Epsilon:
;

%%

void yyerror(const char *msg) {
    fprintf(stderr,
            "\n"
            "|-----------------------------------------------------------------"
            "---------\n"
            "| Error found in Line #%d: %s\n"
            "|\n"
            "| Unmatched token: %s\n"
            "|-----------------------------------------------------------------"
            "---------\n",
            line_num, current_line, yytext);
    exit(-1);
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename> [--dump-ast]\n", argv[0]);
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("fopen() failed");
        exit(-1);
    }

    yyparse();

    if (argc >= 3 && strcmp(argv[2], "--dump-ast") == 0) {
        AstDumper ast_dumper;
        root->accept(ast_dumper);
    }

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");

    delete root;
    fclose(yyin);
    yylex_destroy();
    return 0;
}
