%{
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern int32_t line_num;    /* declared in scanner.l */
extern char current_line[]; /* declared in scanner.l */
extern FILE *yyin;          /* declared by lex */
extern char *yytext;        /* declared by lex */

extern int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);
%}

%token ID
%token INTEGER_LITERAL REAL_LITERAL STRING_LITERAL BOOLEAN_LITERAL
%token ASSIGN
%token COMMA COLON SEMI LPAREN RPAREN LBRACK RBRACK
%token LT LE NE GE GT EQ
%token PLUS MINUS STAR SLASH MOD_T
%token AND OR NOT_T
%token VAR ARRAY OF BOOLEAN_T INTEGER_T REAL_T STRING_T
%token WHILE DO IF THEN ELSE FOR TO BEGIN_T END PRINT READ RETURN

/* 優先序 */
%left LT LE NE GE GT EQ  
%left AND OR NOT_T
%left MINUS
%left PLUS
%left SLASH MOD_T
%left STAR
%right UMINUS

%start program


%%
program
  : ID SEMI global_declaration function_block compound_statement END
  ;


global_declaration
  : /* 空 */
  | global_declaration declaration
  ;

function_block
  : /* 空 */
  | function_block function
  ;

function
  : function_header SEMI                         /* function_declaration */
  | function_header compound_statement END       /* function_definition  */
  ;

function_header
  : ID LPAREN opt_formal_args RPAREN opt_return
  ;

/* 參數串：空／一個或多個 formal_argument */
opt_formal_args
  : /* 空 */
  | formal_arg_list
  ;

formal_arg_list
  : formal_argument
  | formal_arg_list SEMI formal_argument
  ;


/* identifier_list : type （type 可為 scalar 或 array）*/
formal_argument
  : identifier_list COLON type
  ;

/* procedure or scalar_type */
/* 後面可以街scalar type 和 array type 但本作業從簡所以指考慮scalar type */ 
opt_return
  : /* 空 */
  | COLON scalar_type
  ;

scalar_type
    : INTEGER_T
    | REAL_T
    | STRING_T
    | BOOLEAN_T
    ;

statement
  : assignment
  | print_statement
  | read_statement
  | return_statement
  | function_call SEMI
  | conditional_statement
  | while_statement
  | for_statement
  | compound_statement
  ;

print_statement   : PRINT expression SEMI ;
read_statement    : READ  variable_reference SEMI ;
return_statement  : RETURN expression SEMI ;

conditional_statement
  : IF expression THEN compound_statement ELSE compound_statement END IF
  | IF expression THEN compound_statement END IF
  ;

while_statement
  : WHILE expression DO compound_statement END DO
  ;

for_statement
  : FOR ID ASSIGN INTEGER_LITERAL TO INTEGER_LITERAL DO compound_statement END DO
  ;

compound_statement
    : BEGIN_T declaration_block statement_block END
    ;

declaration_block
    : 
    | declaration_block declaration
    ;

declaration
    : VAR identifier_list COLON type SEMI
    | VAR identifier_list COLON const_value SEMI
    ;

identifier_list
    : ID
    | identifier_list COMMA ID
    ;

type
    : scalar_type
    | array_type
    ;

array_type
    : ARRAY INTEGER_LITERAL OF type
    ;

const_value
    : INTEGER_LITERAL
    | REAL_LITERAL
    | STRING_LITERAL
    | BOOLEAN_LITERAL
    ;

statement_block
    : /* 空 */
    | statement_block statement
    ;

assignment
    : variable_reference ASSIGN expression SEMI
    ;

variable_reference
    : ID
    | variable_reference LBRACK expression RBRACK
    ;

function_call
    : ID LPAREN argument_list RPAREN
    ;

argument_list
    : /* 空 */
    | expr_list
    ;

expr_list
    : expression
    | expr_list COMMA expression
    ;

expression
    : primary
    | expression PLUS  expression
    | expression MINUS expression
    | expression STAR  expression
    | expression SLASH expression
    | expression MOD_T expression
    | expression LT expression
    | expression LE expression
    | expression NE expression
    | expression GE expression
    | expression GT expression
    | expression EQ expression
    | expression AND expression
    | expression OR  expression
    | MINUS expression %prec UMINUS
    | NOT_T expression
    | LPAREN expression RPAREN
    ;

primary
    : literal_constant
    | variable_reference
    | function_call
    ;

literal_constant
    : INTEGER_LITERAL
    | REAL_LITERAL
    | STRING_LITERAL
    | BOOLEAN_LITERAL
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
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("fopen() failed");
        exit(-1);
    }

    yyparse();

    fclose(yyin);
    yylex_destroy();

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");
    return 0;
}
