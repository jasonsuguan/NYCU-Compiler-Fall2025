%option never-interactive
%option nounput
%option noinput

%{
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LEN 512
/* Code runs each time a token is matched. */
#define YY_USER_ACTION updateCurrentLine(yytext);
/* string addressing */
#define STR_SIZE 4096 /* 512*8 */

uint32_t line_num = 1;
uint32_t col_num = 1;
static uint32_t opt_src = 1;
static uint32_t opt_tok = 1;
static char current_line[MAX_LINE_LEN];
/*string addressing */
static char strbuffer[STR_SIZE];
static size_t strposition = 0;

static void updateCurrentLine(const char *source);
static void listToken(const char *name);
static void listLiteral(const char *name, const char *literal);
static void rewindCurrentLine(const size_t);

%}
%x comment
%x STR

%%
"\n" {}

":=" { listToken(":="); }
[,:;\(\)\[\]]   { listToken(yytext); }

"<=" { listToken("<="); }
"<>" { listToken("<>"); }
">=" { listToken(">="); }
"<"  { listToken("<"); }
">"  { listToken(">"); }
"="  { listToken("="); }

"+"  { listToken("+"); }
"-"  { listToken("-"); }
"*"  { listToken("*"); }
"/"  { listToken("/"); }
"mod" { listToken("mod"); }

"and" { listToken("and"); }
"or"  { listToken("or"); }
"not" { listToken("not"); }

"var"      { listToken("KWvar"); }
"def"      { listToken("KWdef"); }
"array"    { listToken("KWarray"); }
"of"       { listToken("KWof"); }
"boolean"  { listToken("KWboolean"); }
"integer"  { listToken("KWinteger"); }
"real"     { listToken("KWreal"); }
"string"   { listToken("KWstring"); }
"true"     { listToken("KWtrue"); }
"false"    { listToken("KWfalse"); }
"while"    { listToken("KWwhile"); }
"do"       { listToken("KWdo"); }
"if"       { listToken("KWif"); }
"then"     { listToken("KWthen"); }
"else"     { listToken("KWelse"); }
"for"      { listToken("KWfor"); }
"to"       { listToken("KWto"); }
"begin"    { listToken("KWbegin"); }
"end"      { listToken("KWend"); }
"print"    { listToken("KWprint"); }
"read"     { listToken("KWread"); }
"return"   { listToken("KWreturn"); }

[a-zA-Z][a-zA-Z0-9]* { listLiteral("id", yytext);}
    /* ((0|[1-9][0-9]*)\.([0-9]*[1-9]|0)|(0|[1-9][0-9]*))[eE][+-]?[0-9]+ { listLiteral("scientific", yytext); } */
((0|[1-9][0-9]*)\.([0-9]*[1-9]|0)|(0|[1-9][0-9]*))[eE][+-]?[0-9]+  {
    int i = 0;
    while (yytext[i] && yytext[i] != 'e' && yytext[i] != 'E') {
        i++;
    }
    int is_zero = 1;
    for (int j = 0; j < i; j++) {
        if (yytext[j] >= '1' && yytext[j] <= '9') {
            is_zero = 0;
            break;
        }
    }

    if (is_zero) {
        rewindCurrentLine((size_t)(yyleng - i));
        yyless(i);
        if (memchr(yytext, '.', i)) {
            listLiteral("float", yytext);   /* 0.0, 0.00 float */
        } else {
            listLiteral("integer", yytext); /* 0 integer */
        }
    } else {
        int exp_start = i + 1;                   /* ignore e/E */
        if (yytext[exp_start] == '+' || yytext[exp_start] == '-')
            exp_start++;

        const char *exp_digits = yytext + exp_start;
        int exp_len = strlen(exp_digits);

        if (exp_len > 1 && exp_digits[0] == '0') {
            int cutoff = exp_start + 1;
            rewindCurrentLine((size_t)(yyleng - cutoff));
            yyless(cutoff);
            listLiteral("scientific", yytext);
        } else {
            listLiteral("scientific", yytext);
        }
    }
}

(0|[1-9]+[0-9]*)\.([0-9]*[1-9]|0) { listLiteral("float", yytext); }
0[0-7]+ { listLiteral("oct_integer" , yytext); }
0|[1-9][0-9]* { listLiteral("integer", yytext); }


\"  { strposition = 0; strbuffer[0] = '\0'; BEGIN(STR); }
<STR>\"\" { if (strposition + 1 < STR_SIZE) { strbuffer[strposition++] = '"'; }
            strbuffer[strposition] = '\0';
            }

<STR>[^\"\n]+ {size_t n = (size_t) yyleng; /*after read the string, yyleng = string length*/
               if (strposition + n >= STR_SIZE) n = STR_SIZE - 1 - strposition;
               memcpy(strbuffer + strposition, yytext, n);
               strposition += n;
               strbuffer[strposition] = '\0';
                                   }

<STR>\" { BEGIN(INITIAL); listLiteral("string", strbuffer); }

<STR>\n {printf("Error at line %d: bad character \"%s\"\n", line_num, yytext);
         exit(-1); }
[ \t\r]+        {}

"/*"            { BEGIN(comment); }
<comment>[^*\n]+    {}
<comment>\*+[^*/\n] {}
<comment>\n         {}
<comment>\*+\/      { BEGIN(INITIAL); }

\/\/&[A-Z][+-][^\n]*  {
    char ST = yytext[3];
    char pm  = yytext[4];
    if (ST == 'S') opt_src = (pm == '+');
    else if (ST == 'T') opt_tok = (pm == '+');
}

\/\/[^\n]*      {}

    /* Catch the character which is not accepted by rules above */
. {
    printf("Error at line %d: bad character \"%s\"\n", line_num, yytext);
    exit(-1);
}

%%

/** @note The line is printed out and flushed when a newline character is encountered. */
static void updateCurrentLine(const char *source) {
    /* col_num is one-based */
    for (const char *c = source; *c; ++c) {
        if (*c == '\n') {
            if (opt_src) {
                printf("%d: %s\n", line_num, current_line);
            }
            ++line_num;
            col_num = 1;
            current_line[0] = '\0';
        } else {
            if (col_num < MAX_LINE_LEN) {
                current_line[col_num - 1] = *c;
                current_line[col_num] = '\0';
            } else {
                /* Truncate silently; doesn't affect the program's correctness. */
            }
            ++col_num;
        }
    }
}

static void listToken(const char *name) {
    if (opt_tok) {
        printf("<%s>\n", name);
    }
}

static void listLiteral(const char *name, const char *literal) {
    if (opt_tok) {
        printf("<%s: %s>\n", name, literal);
    }
}

/** @note This function is not required if the input file is guaranteed to end
 * with a newline. However, students may find it useful to handle the case where
 * the input file does not end with a newline, as it has been reported several
 * times in the past.
 */
int yywrap(void) {
    /* If the file is not ended with a newline, fake it to print out the last line. */
    if (col_num > 1) {
        updateCurrentLine("\n");
    }
    /* no more input file */
    return 1;
}

/* new function */
static void rewindCurrentLine(size_t n) {
    if (n == 0) return;
    size_t len = strlen(current_line);
    if (n > len) n = len;
    current_line[len - n] = '\0';
    if (col_num > n) col_num -= n;
    else col_num = 1;
}

int main(int argc, const char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [filename]\n", argv[0]);
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        fprintf(stderr, "Open file error\n");
        exit(-1);
    }

    yylex();

    fclose(yyin);
    /* Free resources used by the scanner; otherwise, unused data structures
    will persist throughout the successive compiler phases. */
    yylex_destroy();

    return 0;
}