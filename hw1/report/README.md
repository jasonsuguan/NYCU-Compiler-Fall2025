# hw1 report

|Field|Value|
|-:|:-|
|Name|李智璿|
|ID|111550059|

## How much time did you spend on this project

About 7 hrs.

## Project overview

I added the code at the rule section mostly. And here is the structure:
### Delimiters
```lex
[,:;\(\)\[\]]   { listToken(yytext); }
```

### Arithmetic, Relational, and Logical Operators
```lex
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

### Reserved Word
"var"      { listToken("KWvar"); }
"def"      { listToken("KWdef"); }
"array"    { listToken("KWarray"); }
"of"       { listToken("KWof"); }
"boolean"  { listToken("KWboolean"); } etc...
```

### Identifiers
```lex
[a-zA-Z][a-zA-Z0-9]* { listLiteral("id", yytext);}
```

### Scientific Notations
```lex
/* First line is complemented to filter scientific */
((0|[1-9][0-9]*)\.([0-9]*[1-9]|0)|(0|[1-9][0-9]*))[eE][+-]?[0-9]+  {
    int i = 0;
    while (yytext[i] && yytext[i] != 'e' && yytext[i] != 'E') {
        i++;
    } /* check the position of e or E */
    int is_zero = 1; /* check the coefficient is zero or not */
    for (int j = 0; j < i; j++) {
        if (yytext[j] >= '1' && yytext[j] <= '9') {
            is_zero = 0;
            break;
        }
    }
    
    if (is_zero) { /*For checking if integer zero or float*/
        rewindCurrentLine((size_t)(yyleng - i));
        yyless(i);
        if (memchr(yytext, '.', i)) {
            listLiteral("float", yytext);   /* 0.0, 0.00 float */
        } else {
            listLiteral("integer", yytext); /* 0 integer */
        }
    } else {
        int exp_start = i + 1;                   /*exp start position ignore e/E */
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

```
Additionally, I also defined other function for rewind the string, because I found that if I didn't rewind,
the scientific string after "e/E" would be load in buffer again so that the string would be printed again.
eg. 0.0e1 will be printed as 0.0e1e1
The reason I think is that regex would load 0.00e1 first, and 0.00e1 is in buffer. And then I use yyless to 
address the situation when coefficent a is illegal in scientific and match the 0.0 to floating point. But yyless
will let scanner back to the position after "0.0" so that "e1" will be scanned one more time.

```lex
static void rewindCurrentLine(size_t n) {
    if (n == 0) return;
    size_t len = strlen(current_line);
    if (n > len) n = len;
    current_line[len - n] = '\0';
    if (col_num > n) col_num -= n;
    else col_num = 1;
}
```
### Floating-Point/Integer Constants
```lex
(0|[1-9]+[0-9]*)\.([0-9]*[1-9]|0) { listLiteral("float", yytext); }
0[0-7]+ { listLiteral("oct_integer" , yytext); }
0|[1-9][0-9]* { listLiteral("integer", yytext); }
```
### String Constants
```lex
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
```
### Whitespace
```lex
[ \t\r]+        {}
```
### Comments
```lex
"/*"            { BEGIN(comment); }
<comment>[^*\n]+    {}
<comment>\*+[^*/\n] {}
<comment>\n         {}
<comment>\*+\/      { BEGIN(INITIAL); }
```
### Pseudocomments
```lex
\/\/&[A-Z][+-][^\n]*  {
    char ST = yytext[3];
    char pm  = yytext[4];
    if (ST == 'S') opt_src = (pm == '+');
    else if (ST == 'T') opt_tok = (pm == '+');
}
```

## What is the hardest you think in this project

Addressing scientific and string. Because there are so many traps in different case. Hope that
hidden case will not be so difficult.

## Feedback to T.A.s
good good
