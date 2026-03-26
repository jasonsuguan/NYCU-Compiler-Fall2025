/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_H_INCLUDED
# define YY_YY_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 58 "parser.y"

    #include "AST/utils.hpp"
    #include "AST/PType.hpp"

    #include <vector>
    #include <memory>

    class AstNode;
    class DeclNode;
    class ConstantValueNode;
    class CompoundStatementNode;
    class FunctionNode;
    class ExpressionNode;

#line 64 "parser.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TOK_YYEMPTY = -2,
    TOK_YYEOF = 0,                 /* "end of file"  */
    TOK_YYerror = 256,             /* error  */
    TOK_YYUNDEF = 257,             /* "invalid token"  */
    TOK_COMMA = 258,               /* COMMA  */
    TOK_SEMICOLON = 259,           /* SEMICOLON  */
    TOK_COLON = 260,               /* COLON  */
    TOK_L_PARENTHESIS = 261,       /* L_PARENTHESIS  */
    TOK_R_PARENTHESIS = 262,       /* R_PARENTHESIS  */
    TOK_L_BRACKET = 263,           /* L_BRACKET  */
    TOK_R_BRACKET = 264,           /* R_BRACKET  */
    TOK_ASSIGN = 265,              /* ASSIGN  */
    TOK_OR = 266,                  /* OR  */
    TOK_AND = 267,                 /* AND  */
    TOK_NOT = 268,                 /* NOT  */
    TOK_LESS = 269,                /* LESS  */
    TOK_LESS_OR_EQUAL = 270,       /* LESS_OR_EQUAL  */
    TOK_EQUAL = 271,               /* EQUAL  */
    TOK_GREATER = 272,             /* GREATER  */
    TOK_GREATER_OR_EQUAL = 273,    /* GREATER_OR_EQUAL  */
    TOK_NOT_EQUAL = 274,           /* NOT_EQUAL  */
    TOK_PLUS = 275,                /* PLUS  */
    TOK_MINUS = 276,               /* MINUS  */
    TOK_MULTIPLY = 277,            /* MULTIPLY  */
    TOK_DIVIDE = 278,              /* DIVIDE  */
    TOK_MOD = 279,                 /* MOD  */
    TOK_UNARY_MINUS = 280,         /* UNARY_MINUS  */
    TOK_ARRAY = 281,               /* ARRAY  */
    TOK_BOOLEAN = 282,             /* BOOLEAN  */
    TOK_INTEGER = 283,             /* INTEGER  */
    TOK_REAL = 284,                /* REAL  */
    TOK_STRING = 285,              /* STRING  */
    TOK_END = 286,                 /* END  */
    TOK_BEGIN = 287,               /* BEGIN  */
    TOK_DO = 288,                  /* DO  */
    TOK_ELSE = 289,                /* ELSE  */
    TOK_FOR = 290,                 /* FOR  */
    TOK_IF = 291,                  /* IF  */
    TOK_THEN = 292,                /* THEN  */
    TOK_WHILE = 293,               /* WHILE  */
    TOK_DEF = 294,                 /* DEF  */
    TOK_OF = 295,                  /* OF  */
    TOK_TO = 296,                  /* TO  */
    TOK_RETURN = 297,              /* RETURN  */
    TOK_VAR = 298,                 /* VAR  */
    TOK_FALSE = 299,               /* FALSE  */
    TOK_TRUE = 300,                /* TRUE  */
    TOK_PRINT = 301,               /* PRINT  */
    TOK_READ = 302,                /* READ  */
    TOK_ID = 303,                  /* ID  */
    TOK_INT_LITERAL = 304,         /* INT_LITERAL  */
    TOK_REAL_LITERAL = 305,        /* REAL_LITERAL  */
    TOK_STRING_LITERAL = 306       /* STRING_LITERAL  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 74 "parser.y"

    /* basic semantic value */
    char *identifier;
    uint32_t integer;
    double real;
    char *string;
    bool boolean;

    int32_t sign;

    AstNode *node;
    PType *type_ptr;
    DeclNode *decl_ptr;
    CompoundStatementNode *compound_stmt_ptr;
    ConstantValueNode *constant_value_node_ptr;
    FunctionNode *func_ptr;
    ExpressionNode *expr_ptr;

    std::vector<std::unique_ptr<DeclNode>> *decls_ptr;
    std::vector<IdInfo> *ids_ptr;
    std::vector<uint64_t> *dimensions_ptr;
    std::vector<std::unique_ptr<FunctionNode>> *funcs_ptr;
    std::vector<std::unique_ptr<AstNode>> *nodes_ptr;
    std::vector<std::unique_ptr<ExpressionNode>> *exprs_ptr;

#line 158 "parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;

int yyparse (void);


#endif /* !YY_YY_PARSER_H_INCLUDED  */
