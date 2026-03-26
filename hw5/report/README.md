# hw5 report

|||
|-:|:-|
|Name|李智璿|
|ID|111550059|

## How much time did you spend on this project

> 32 hours.

## Project overview
本次作業實作編譯器的Code Generator，並根據 RISC-V 的ISA產生對應的目標代碼，以實現source code的功能。
這次的作業訪問節點的部分和前面做的ast visitor很像，另外由於暫存器有限，因此我主要的實作方式爲以下：
1. Visitor Pattern: 本次實作Visitor 模式，遍歷 AST 節點，針對不同的語法結構即時產生對應的 RISC-V 指令。
2. Stack-based Machine Model: 本實作所用的Stack Machine把所有的運算中間值皆暫存於 Stack 中，透過 lw、sw 與 addi sp, sp, -4 等指令進行管理。

### Stack Frame
每個函數進入時皆會建立獨立的 Stack Frame。
1. Prologue: 保存ra與舊的s0，並預留足夠空間存放區域變數。
2. Epilogue: 恢復 ra 與 s0，並將 sp 指回呼叫前的位置。
3. Offset Management: 透過符號表記錄每個變數在 Stack 中的(Offset，以 s0 為基準點進行定址。

### Codegenerator
這部分新增了幾個需要用到的變數：
```int m_current_offset = 12;``` 用來追蹤目前局部變數分配到哪裡
```bool m_is_lvalue = false;```  標記目前是否在處理lhs或rhs
```std::string m_current_function_name;``` 用來紀錄目前的函數名稱 這是為了之後返回函數用的
    
```uint32_t m_label_count = 0;```
```std::string nextLabel() ``` 這裏則是實作if while for 的 Label 產生器所需要的變數

這裏的重點是，因為需要預留 8btyes 來存放ra和s0，所以current offset初始化爲12

### 各個node的實作
這部分會針對各個node比較重要的部分敘述

#### Program Node (程式進入點)
1. 這部分負責產出檔案標頭（.file, .option）、初始化符號表作用域，以及main 函數的 Prologue。
2. 另外這裡預留了 128 bytes 的 Stack 空間並保存了 ra 與 s0。

#### CompoundStatementNode
1. 這部分處理成對的 begin...end statement，
2. 實作了 Implicit Scoping，在這裏，如果區塊內宣告了一個與外部同名的變數，應該優先使用內部的，另外在進入節點時 pushScope，離開時 popScope確保區域變數的生命週期正確。


#### 變數管理 (Declaration, Reference)
VariableNode:
這裏實作區分全域與區域變數。
1. 全域：使用 .comm 分配未初始化空間，或 .word 初始化常數。
2. 區域：計算相對於 s0 的 Offset，並在符號表中記錄。特別是區域常數需在執行期產生 li 與 sw 指令將數值寫入 Stack。

VariableReferenceNode:
這裏實作 L-value 與 R-value 的判斷與切換。（透過 m_is_lvalue 旗標決定)
>true：產生變數的記憶體位址（addi t0, s0, offset）。
>false：產生變數的數值（lw t0, offset(s0)）。

#### 表達式與運算 (Expression, Constants, Operators)
ConstantValueNode:
1. 實作load integer。
2. 為了達成boolean測資，將true/false 整為整數 1/0。

Binary/UnaryOperatorNode:
1. 採用 Stack Machine 模型。
2. Binary：從 Stack 彈出兩個運算元（注意左右順序），運算後推回。
3. Unary：實作取負（sub t0, x0, t0）與取反（seqz）。

#### Control Flow
IfNode / WhileNode:
這地方的重點是Label管理。
1. 利用 m_label_count 產生唯一的跳轉標籤。
2. 實作條件分支邏輯：若條件為假（beqz），則跳轉到 else 或迴圈出口。

ForNode:
包含初始化、邊界檢查、迴圈主體執行、以及遞增 (i := i + 1)。


#### 函數呼叫與參數傳遞 (Function, Return, Invocation)
FunctionNode:
實作 Calling Convention。
參數接收：前 8 個從 a0-a7 搬到 Stack；第 9 個以後透過 正向偏移量 (Positive Offset) 從 Caller 的空間讀取。

FunctionInvocationNode:
參數傳遞策略 (Caller & Callee)：
1. 暫存器：前 8 個參數透過 a0-a7 暫存器傳遞以提升效能。
2. 溢位處理 (Stack Arguments)：當參數超過 8 個時，Caller將額外參數存於 Stack 頂端。而Callee透過正向偏移量（如 0(s0), 4(s0)）從父層中讀取這些參數。

堆疊處理與狀態保存：
1. Callee 端：在 Prologue 階段保存 ra 與 s0。
2. Caller 端：在 jal 呼叫返回後，負責收回溢位參數佔用的 Stack 空間（addi sp, sp, overflow_bytes），以維持堆疊指標 sp 的正確性。

回傳方法：
ReturnNode 負責將回傳表達式的結果加載至 a0，並無條件跳轉至函數末尾的 Epilogue 標籤，確保資源被正確回收後返回呼叫處。

ReturnNode:
將結果存入 a0 並跳至函數結尾標籤（j [func]_end），確保 Epilogue 被執行。

#### 輸入輸出 (Read, Print)
PrintNode: 呼叫外部 C 函式 printInt。

ReadNode: 取得變數地址，呼叫 readInt 後將 a0 的結果存入該地址。

## What is the hardest you think in this project
這次比較困難的部分是處理多個參數的時候，會需要動用到caller自己的stack，但要小心不能隨變動到當前stack pointer的位置，而這份作業要熟悉risc-v的指令集操作以及每次推stack pop stack的邏輯也要二次check，不然很容易一直讀到空的資料產生錯誤，和前幾次作業在存取ast的時候有點像，只是這次從處理指標變處理記憶體位址。

## Feedback to T.A.s
Finish:)
