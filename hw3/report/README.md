# hw3 report

|      |                 |
| ---: | :-------------- |
| Name | 李智璿           |
|   ID | 111550059       |

## How much time did you spend on this project

> 56 hours.

## Project overview
這次作業使用visitor pattern version

### Overall Program Structure
根據 grammar，我的程式最上層是：
```lex
Program:
    ProgramName SEMICOLON
    DeclarationList
    FunctionList
    CompoundStatement
    END
```
這代表一個完整的 program 包含：
1. ProgramName：程式名稱與位置 (line, col)
2. DeclarationList：所有全域變數 / 常數的宣告
3. FunctionList：所有 function / procedure 的宣告或定義
4. CompoundStatement：主程式的 body
在 parser.y 中，我在這裡建立 ProgramNode，並把上面四個部分的 AST node list 儲存在 ProgramNode 內部。

### Declaration Node
DeclNode 用來表示「一整行的宣告」這種語法結構，例如：
```lex
var a, b : integer;
var c : 10; 
```
我用 ```enum class DeclType { Var, Const };``` 來記錄這一行宣告是「變數」還是「常數」。
在Decl Node的內部變數中，我新增了兩個變數分別是```m_type```和``` m_decl_list```：
1. m_type是用來紀錄這一行宣告是 Var 還是 Const，在 AST dumper 印出時可以知道要不要特別處理常數
2. m_decl_list則是用來存的是這一行裡面的所有「被宣告的東西」，每個元素實際上是 VariableNode*

例如 var a, b : integer; 會變成一個 DeclNode，裡面 m_decl_list 有兩個 VariableNode
因此在parser中$$賦值的形式會爲：```  new DeclNode (@1.first_line, @1.first_column, DeclType, decl list的指標) ```
會將所宣告的變數和type都傳遞進去。

比較特別的是，我在decl中有特別新增一個``` getVariables()```函數，方便從 DeclNode 拿出裡面所有的 VariableNode，給 FunctionNode或其他地方使用

### Function Node
FunctionNode 用來表達一個完整的函式定義或函式宣告
```lex
func1(x, y: integer): boolean
begin
end
end
```
而在function node中的內部變數，我新增了四個變數
```lex
private:
    std::string name;
    VarType return_type;
    std::vector<DeclNode*> *declarations; // declaration list 參數
    CompoundStatementNode *body;
```

1. name:儲存函式名稱，例如 "func1"。
2. return_type:紀錄函式的傳回型別（integer, real, string, boolean，或 procedure 的 void）。
3. declarations:保存參數是用 DeclNode list 表示，每個 DeclNode 就是一組 x, y : integer 的參數宣告
例如：```func1(x, y: integer; z: string): boolean```
會變成：
```lex
FunctionNode
  ├── DeclNode  (x, y: integer)
  └── DeclNode  (z: string)
```
這樣就可以利用你之前為變數宣告寫好的 DeclNode，不需建立新的資料結構。
4. body:
  如果是函式定義，有 body：
```lex
begin
   ...
end
```
  如果是函式宣告（尾端是 ;），body = nullptr。
  因此在parser中$$賦值的形式會爲
```lex
$$ = new FunctionNode(@1.first_line, @1.first_column,$1(function name), 
$5 (return type), $3 (variable list), body or nullptr);
```

### Compound Statement Node
Statement 用來表達declararion 和 function 宣告完後，之後的複合子句，由begin作為前導，end爲結尾
```lex
begin
    DeclarationList
    StatementList
end
```
它是所有程式區塊（block）的共同形式，如：if 的 then body、else body while 迴圈 body等等
而在statement node中的內部變數，我新增了兩個變數
```lex
private:
    NodeList m_declarations;
    NodeList m_statements;
```
1. m_declararions:這裏存放所有的decl node 所有在 begin ... end 裡的 var ...; / var ... : constant都會被蒐集在這裏
2. m_statements: 這裏存放所有後續 statements (assignment, print, read, if, while, for, return...)

所以parser 在產生 compound statement 時會傳進兩個 NodeList*：
CompoundStatementNode(....,const NodeList *decls, const NodeList *stmts);

### Variable Node
VariableNode 用來表示程式中的一個變數宣告
它對應的部分，例如：
```lex
var a : integer;
var b, c : real;
var d : array 3 of boolean;
var e : 10;
```
每一個在 IdList 中的變數，都會建立一個 VariableNode。
因為按照要求在，在變數宣告（DeclNode）下面，需要印出每個變數的資訊，像是變數名稱,型別（integer / real  array 以及是否為const等等
為了保存所有資訊，因此需要一個 VariableNode 來代表「單一變數宣告」。

Variable Node的內部變數包含：
```lex
private:
    std::string name;
    VarType var_type;
    std::vector<uint32_t> array_dims;
    AstNode *m_init;
    bool is_constant;
```
這部分的意義非常直觀，保存變數名和其型別，另外我也在這裏爲可能的多維陣列新增了dims來存放這部分的信息，而m_init和is_constant就是用來看
宣告變數的後面有沒有指定型別或數值，藉以爲之後要不要建立constant node的節點。



### Statement Nodes
在這次作業裡，只要是statement的語法，我都做成一個對應的 *Node，例如：
```lex
AssignmentNode：a := 1;
PrintNode：print 123;
ReadNode：read a;
IfNode：if ... then ... [else ...] end if
WhileNode：while ... do ... end do
ForNode：for ... do ... end do
ReturnNode：return ...;
CompoundStatementNode：begin ... end
```
他們共同的設計思路是在每個 statement node 都繼承自 AstNode
這樣在 CompoundStatementNode 裡就可以用同一個 NodeList (std::vector<AstNode*>) 把不同種類的 statement 混在一起存，例如：
```lex
class CompoundStatementNode : public AstNode {
  NodeList m_declarations; // DeclNode*
  NodeList m_statements;   // AssignmentNode* / PrintNode* / IfNode* ...
};
```

在 parser.y 的 Statement / Simple / Condition / While / For / Return / FunctionCall 這幾個 non-terminal 裡，
我會直接 new 對應的 AST node，例如：
```lex
Simple:
    VariableReference ASSIGN Expression SEMICOLON {
        $$ = new AssignmentNode(@2.first_line, @2.first_column, $1, $3);
    }
  | PRINT Expression SEMICOLON {
        $$ = new PrintNode(@1.first_line, @1.first_column, $2);
    }
  | READ VariableReference SEMICOLON {
        $$ = new ReadNode(@1.first_line, @1.first_column, $2);
    }
;
```

這樣 StatementList 只要把所有這些 $$ 裝進 NodeList，最後交給 CompoundStatementNode 保存即可。

### Expression Nodes
任何會產生值的節點，我都繼承ExpressionNode，而 ExpressionNode 再繼承自 AstNode。但目前這一層額外的 ExpressionNode只負責分類，有看到助教有對這部分提示後面的lab會有額外的功能，所以我先對部分的節點作分類。
這次作業中屬於 expression 的 node 有：
```lex
ConstantValueNode：整數、實數、字串、布林常數
VariableReferenceNode：變數或陣列取值（含 indices）
BinaryOperatorNode：+ - * / mod < > <= >= = <> and or 等二元運算
UnaryOperatorNode：neg（一元負號）和 not
FunctionInvocationNode：foo(a, b + 1) 這種呼叫
ExpressionNode 結構
```
ExpressionNode的結構爲：
```lex
class ExpressionNode : public AstNode {
  public:
    ExpressionNode(const uint32_t line, const uint32_t col);
    ~ExpressionNode() = default;

  protected:
    // 之後作業可以在這裡加上 expression 的型別
};
```
而有關運算子的做法：
#### BinaryOperatorNode
在BinaryOperatorNode成員大致上是：
1. 運算子字串（例如 "+", "and", "mod", "=", "<> 等）
2. 左/右 operand：兩個 ExpressionNode*

在parser 的 Expression 文法裡，對於每一種二元運算，我都建立對應的 BinaryOperatorNode：
```lex
Expression:
    Expression PLUS Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "+", $1, $3);
    }
  | Expression AND Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "and", $1, $3);
    }
  | ...
;
```

#### UnaryOperatorNode
只有一個 operator 字串（"neg" 或 "not"）+ 一個 operand ExpressionNode*

例如處理-x時就是看這個operator node：
```lex
Expression:
    MINUS Expression {
        $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, "neg", $2);
    }
  | NOT Expression {
        $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, "not", $2);
    }
  | ...
;
```

#### ConstantValueNode
在這裏，內部只保存一個string m_value;，所有 literal（整數、real num、字串、bool）最後都會轉成字串塞進來。
像 LiteralConstant non-terminal 會處理正負號、數值合併，再用 std::to_string 或直接用字串建立 ConstantValueNode。
這讓 AST dumper 只要呼叫 getValueCString() 就能印出對應的常數。

#### VariableReferenceNode
保存變數名稱 + 一串 index expression：
```lex
class VariableReferenceNode : public ExpressionNode {
  std::string name;
  std::vector<ExpressionNode*> indices;
};
```
parser 在 VariableReference 和 ArrRefs 中先把 indices 存成 NodeList，最後轉成 std::vector<ExpressionNode*> 丟進 VariableReferenceNode。

#### FunctionInvocationNode
在 FunctionInvocation 的 production 中，把 ExpressionList 收集成一個 NodeList，轉成 ExpressionNode* vector 建立 FunctionInvocationNode；在 AST dumper 中會依序印出每個參數。

### ASTdumper核心
在本次作業中，我有用AstDumper來將AST結構印出來的工具，用來檢查 parser 是否正確建立AST。
所有 AST 節點（Program、Decl、Function、BinaryOp、If…等）都會透過 Visitor Pattern 交由 AstDumper 處理並印成指定格式。

#### 使用 Visitor Pattern
助教提供的 AST 基底類別 AstNode 已經內建：
```lex
accept(AstNodeVisitor&)
visitChildNodes(AstNodeVisitor&)
```
每個 ASTNode 會在 accept() 中呼叫```p_visitor.visit(*this);```，而 AstDumper 就是這個 visitor，負責印出該節點的標頭（如 variable、binary operator、if statement）

#### 流程

每種節點都會照這模式：
```lex
visit():
    印標頭
    m_indenter.increaseLevel()
    visitChildNodes()   // 節點決定如何訪問子節點
    m_indenter.decreaseLevel()
```

例如：
```lex
void AstDumper::visit(DeclNode &p_decl) {
    printIndent();
    printf("declaration <line: ..., col: ...>\n");

    m_indenter.increaseLevel();
    p_decl.visitChildNodes(*this);
    m_indenter.decreaseLevel();
}
```
indentation 負責控制樹狀縮排而 visitChildNodes() 決定要印哪些子樹
這樣每個節點只需專注於印出自己的資訊。

## What is the hardest you think in this project
在這次的作業中最難的部分應該是前面要先讀懂整個程式的架構和每個hpp和cpp之間的關係，而這次很多功能都是每個不同的node之間繼承來繼承去，
那這部分就要針對每個不同類別但具有相同功能的節點進行整合，不過搞清楚後發現其實實現功能的部分還蠻簡單的，但我也花很多時間在trace code

## Feedback to T.A.s
這次作業的example有加快我理解整個作業的基本概念 希望下次作業也會有類似的小hint
