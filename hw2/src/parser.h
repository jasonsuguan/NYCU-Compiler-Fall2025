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

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    ID = 258,                      /* ID  */
    INTEGER_LITERAL = 259,         /* INTEGER_LITERAL  */
    REAL_LITERAL = 260,            /* REAL_LITERAL  */
    STRING_LITERAL = 261,          /* STRING_LITERAL  */
    BOOLEAN_LITERAL = 262,         /* BOOLEAN_LITERAL  */
    ASSIGN = 263,                  /* ASSIGN  */
    COMMA = 264,                   /* COMMA  */
    COLON = 265,                   /* COLON  */
    SEMI = 266,                    /* SEMI  */
    LPAREN = 267,                  /* LPAREN  */
    RPAREN = 268,                  /* RPAREN  */
    LBRACK = 269,                  /* LBRACK  */
    RBRACK = 270,                  /* RBRACK  */
    LT = 271,                      /* LT  */
    LE = 272,                      /* LE  */
    NE = 273,                      /* NE  */
    GE = 274,                      /* GE  */
    GT = 275,                      /* GT  */
    EQ = 276,                      /* EQ  */
    PLUS = 277,                    /* PLUS  */
    MINUS = 278,                   /* MINUS  */
    STAR = 279,                    /* STAR  */
    SLASH = 280,                   /* SLASH  */
    MOD_T = 281,                   /* MOD_T  */
    AND = 282,                     /* AND  */
    OR = 283,                      /* OR  */
    NOT_T = 284,                   /* NOT_T  */
    VAR = 285,                     /* VAR  */
    ARRAY = 286,                   /* ARRAY  */
    OF = 287,                      /* OF  */
    BOOLEAN_T = 288,               /* BOOLEAN_T  */
    INTEGER_T = 289,               /* INTEGER_T  */
    REAL_T = 290,                  /* REAL_T  */
    STRING_T = 291,                /* STRING_T  */
    WHILE = 292,                   /* WHILE  */
    DO = 293,                      /* DO  */
    IF = 294,                      /* IF  */
    THEN = 295,                    /* THEN  */
    ELSE = 296,                    /* ELSE  */
    FOR = 297,                     /* FOR  */
    TO = 298,                      /* TO  */
    BEGIN_T = 299,                 /* BEGIN_T  */
    END = 300,                     /* END  */
    PRINT = 301,                   /* PRINT  */
    READ = 302,                    /* READ  */
    RETURN = 303,                  /* RETURN  */
    UMINUS = 304                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_H_INCLUDED  */
