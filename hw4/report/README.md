# hw4 report

|||
|-:|:-|
|Name|李智璿|
|ID|111550059|

## How much time did you spend on this project

30 hours.

## Project overview
這次作業主要是實作了symbol table的架構以及將偵測到的code進行語義分析，會需要針對多個不同的語法規則傳出error message。
### Symbol Table 功能實作
為了達成作業目標，Symbol Table我分別實作了Symbol Entry和Symbol Manager
並且這次是以Stack-based 的架構來管理多層次的Scope。
1. Symbol Entry 這是符號表中的基本儲存單位，欄位包含了 Name, Kind, Level, Type 與 Attribute。
參數列表處理：為了進行function的語意檢查，除了儲存用於 Dump 的字串屬性外，我額外維護了一個 std::vector<PTypeSharedPtr> parameter_types，用以保存函式參數的原始型態。
多重錯誤屏蔽：我多設計了 is_error 旗標。當符號宣告失敗（如陣列維度錯誤）時會標記此旗標，在後續引用時可直接跳過檢查，避免產生連鎖的錯誤信息。

2. Symbol Manager則是負責處理 Scope 的存在週期與符號的增刪與查找。
Data Structure：使用 std::vector<SymbolTable> 來模擬 Stack。可以方便 Push/Pop，也可以進行由內而外的遍歷查詢。

Scope 管理：
pushScope()：進入新區塊時建立新的。
popScope()：離開區塊時，根據 opt_dump 決定是否將當前 Scope 的內容輸出，隨後將其從 Stack 中移除。

符號操作：
addSymbol()：負責檢查當前 Scope 是否有重複宣告。
lookup()：由當前 Scope 往 Global搜尋，回傳最接近的 SymbolEntry。

### Symbol Analyzer
#### Program & Declaration (program會建立scope symbol table)
ProgramNode為程式起點，負責建立全域 Scope，並將程式本身標為void。
```lex
auto voidType = std::make_shared<PType>(PType::PrimitiveTypeEnum::kVoidType);
```
同時維護 Return Type Stack，推入 void 以確保program不允許回傳值。

DeclNode & VariableNode則是負責變數與常數的宣告。
開創declararion node時，會向下遍歷variable節點取得variable的type和name等等資訊，並加入到symbol table中
symbol註冊: 在decl node中addsymbol會檢查重複宣告的錯誤。
陣列檢查: 針對陣列型態，decl node會檢查每一維度的大小是否大於 0。若不合法，會有dim error，我在這裏也會標記符號為錯誤狀態，以防止後續引用時產生連鎖錯誤。

#### Function Node 處理 (function會建立scope symbol table) 與 function invocation
FunctionNode參數處理: 我先將參數型態收集並生成function symbol，接著建立新 Scope 並將參數以 SymbolKind::parameter 加入，然後再遍歷函式本體。確保了參數與函式內變數處於同一層 Scope。

上下文stack處理: 進入函式時，將回傳型態推入 Return Type Stack，給 ReturnNode 檢查使用。

FunctionInvocationNode參數 : 這裏比對 Symbol Table 中儲存的 parameter_types 與傳入引數的型態。支援純粹的比對和針對可通融的形式 (Integer $\to$ Real)。

FunctionInvocation Node funtion return type傳遞: 將函式的return type賦給當前節點，供上層運算使用。

#### Variable Reference 和 Operation Node
VariableReferenceNode:
維度Reduction: 用來辨識我所用的變數和它所對應原本的變數形態是否有差別，這裏根據引用時使用的index數量，計算剩餘的維度並產生新的型態。例如 int[10][20] 引用一次索引後，型態會變為 int[20]，並透過索引檢查確保所有運算式皆為 integer 型態。

Binary/Unary Operator:
Scalar Check: 檢查運算元必須為純量（透過維度檢查：維度為 0），禁止對陣列進行算術運算。

type的推導: 我實作了各種運算規則（如 mod 僅限整數、邏輯運算僅限布林），並支援把一些可通融的形式也例外處理。

#### Control Flow & Assignment
AssignmentNode:
會先檢查Lvalue 是否為常數或迴圈變數，確保錯誤訊息的優先順序正確。接著檢查是否為陣列賦值，最後才比對左右兩邊的型態相容性。
ForNode:
這裏直接對initial value進行分析，避開了 AssignmentNode 對 Loop Variable 的修改禁令（不然會有禁止動到for內variable的規定問題）。
範圍檢查: 若初始值與終止值皆為常數，會檢查是否符合遞增規則。
ReturnNode:型態檢查: 從 Return Type Stack 取得當前函式的預期回傳型態，並與實際回傳值進行比對。而針對 Void 函式（或 Program）回傳值的情況進行攔截。

#### D- D+實作
在scanner額外新增D的辨識，並在parser新增opt_dump，extern給scanner和symbol table，每次呼叫pop會確認有沒有D-發生。


## What is the hardest you think in this project
這次比較困難的第方式處理例外的情形特別多，需要多設定條件去處理，但每次的變動也有可能會讓其他測試資料不會過，其中我這次遇到比較多的狀況是處理nullptr的問題，例如假使子節點沒有正確的expression，那他就不會回傳正確的type，那這樣就會導致gettype拿不到東西，那假使他的父點會用到他的回傳值，就會出現存取null的問題導致崩潰。另外我還有遇到的問題是type比較的問題，因為error message有時候是要傳入type ptr，那我所遇到的問題是假使兩個指標都是integer但他們ptr指向的位置卻不一樣，就會沒辦法直接ptr與ptr做比較來看彼此的異同，那就要另外用ptr裡面資訊去轉成string去比對。

## Feedback to T.A.s

> Not required, but bonus point may be given.
