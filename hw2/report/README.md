# hw2 report

|||
|-:|:-|
|Name|李智璿|
|ID|111550059|

## How much time did you spend on this project

> 4 hours.

## Project overview

### Main structure
這次的作業以program爲主要架構,主要分成identifier,declaration,function和compound statement四個部分
```lex
program
  : ID SEMI global_declaration function_block compound_statement END
  ;
```
以下會針對各個部分作簡介：
### ID
這部分的實作比較簡單，可以看成是某個program的起始位置的名稱宣告
### declaration
這個區段會容納所有的declaration有可能的行爲，我將整個可能性都作為global_declaration的分支
```lex
global_declaration
  : /* 空 */
  | global_declaration declaration
  ;
```
declaration可以爲空，若不爲空則是繼續往下辨識其他declaration的格式，共有兩種不同的格式
```lex
declaration
    : VAR identifier_list COLON type SEMI
    | VAR identifier_list COLON const_value SEMI
    ;
```
其中 identifier_list裡面會包含著各種要宣告的id，後面則可以決定要指定ID的型別還是當作常數來看。
type和constant的類型也有根據作業的需求定義在相對應的rule裡面。
### function
這個區段則是實作如何辨識我的function的格式是否合理，這個地方一樣可以爲空，若不爲空也是要遵守function的格式。
```lex
function
  : function_header SEMI                         /* function_declaration */
  | function_header compound_statement END       /* function_definition  */
  ;
function_header
  : ID LPAREN opt_formal_args RPAREN opt_return
  ;
```
這裏決定了function的主幹，function的頭可以根據自己的需求看是否要傳入參數進去，不傳東西進去也就意味著這部分的function
其實是procedure，這是另一個function模式
```
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
```
而後面的return也是看要不要加入指定的function會傳出什麼型別的資料
```lex
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
```

### compound statement
這部分是program最後要執行的部分，由Begin爲頭End爲尾，中間部分是compound statement的主要架構
```lex 
compound_statement
    : BEGIN_T declaration_block statement_block END
    ;
```
declaration和statement跟前面的program差不多

### others
#### statement
statement會涵蓋著各種不同的statement類型，各個類型的細節在這裏不一一展開，僅展示statement會出現的種類
```lex
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
```
#### expression
這裏是expression有可能會出現的類型，這裏有運用遞迴的寫法，讓expression和expresstion間的相互運算也可以視為合法語法
```lex
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
```
### About scanner.l
我有將scanner當中各個可能出現的rule都回傳一個token類型給parser進行語法的分析，這部分的檔案是使用助教提供的

### About parser.y
這部分在前面declaration有將所有的token類型都宣告好，比較特別的部分應該是參考yacc slides寫的優先序的部分
```lex
/* 優先序 */
%left LT LE NE GE GT EQ  
%left AND OR NOT_T
%left MINUS
%left PLUS
%left SLASH MOD_T
%left STAR
%right UMINUS
```
這裏考慮到助教給的運算符優先序，並且要注意左結合或右結合

## What is the hardest you think in this project
這次作業較上次簡單，只是在判斷statement的部分比較麻煩，要把各種statement會出現的組合形式給考慮進去，因為有的時候並不會
只有單一expression或單一statement，而是會有不同的句子組成複合式的，那這部分就要考慮比較多，我有看到有些部分有寫到重複
的定義，因為我是一部分一部分陸續寫的，但沒發生衝突所以就不再改簡

## Feedback to T.A.s
無，但聽說LAB3會變難
