# 教學指南

本教學依序介紹 Shell 的各個實作步驟，每個元件完成後都會進行測試，再繼續下一步。

## 前置作業

```bash
make        # 編譯 shell
./shell     # 確認可以正常啟動
```

---

## 步驟一： Helper Functions

`helper.c` 提供 `strip_string` 和 `split_string`，是所有解析功能的基礎。

**`strip_string`** 去除字串頭尾的空白（原地修改）：
```
"  hello  " → "hello"
```

**`split_string`** 在分隔符號處切割字串，將左側 null 終止，並回傳右側的指標：
```
"hello world" 以 ' ' 切割 → 左側="hello"，回傳 "world"
```

測試：
```bash
make helper
```

---

## 步驟二： Parser

`parse.c` 將原始輸入字串解析成 `struct command` 的鏈結串列。

輸入 `ls -al | grep foo` 時：
```
cmd1: name="ls"   argv[0]="-al"  next=→
cmd2: name="grep" argv[0]="foo"  next=NULL
```

每個節點包含：
- `name` — 指令名稱
- `argv` — 參數列表
- `argc` — 參數數量
- `numpipe` — `|N` 中的 N 值（一般管道為 0）
- `builtin_fn` — 若為內建指令則為函式指標，否則為 NULL
- `next` — 管道中的下一個指令

測試：
```bash
make parse
```

---

## 步驟三：環境變數

`env.c` 實作 `setenv`、`printenv` 與 `init_env`。

`init_env` 將預設 `PATH` 設為 `bin:.` 加上系統路徑，讓 `bin/` 與當前目錄的執行檔優先被找到。

```
shell> setenv FOO bar
shell> printenv FOO
bar
shell> printenv QQ
        # 變數不存在時不輸出任何內容
```

測試：
```bash
make env
```

---

## 步驟四：內建指令

內建指令在 `builtin.c` 的 `init_builtin_fn_list` 中註冊，每個內建指令以函式指標儲存在 `builtin_fn_list` 中。

`parse` 建立指令節點時，會在 `builtin_fn_list` 中查找指令名稱並設定 `cmd->builtin_fn`。`execute` 執行時，若 `cmd->builtin_fn != NULL`，直接呼叫該函式，不需要 fork。

可用的內建指令：

| 指令 | 行為 |
|---|---|
| `quit` | 呼叫 `exit(0)` 結束 shell |
| `help` | 列印可用指令 |
| `setenv <key> <value>` | 設定環境變數 |
| `printenv <key>` | 印出環境變數的值 |
| `cd <dir>` | 呼叫 `chdir()`，不需要 fork |

試試看：
```
shell> help
shell> setenv GREETING hello
shell> printenv GREETING
hello
shell> cd /tmp
shell> pwd
/tmp
shell> quit
```

---

## 步驟五：單一外部指令

外部指令透過 `find_executable` 搜尋 `PATH` 找到執行檔路徑，再以 `fork` + `execv` 執行。

```
find_executable("ls") → "/bin/ls"
fork()
  子行程：execv("/bin/ls", ["ls", "-al", NULL])
  父行程：waitpid()
```

試試看：
```
shell> ls
shell> ls -al
shell> echo hello world
shell> grep root /etc/passwd
shell> cat /etc/shells
```

找不到指令時：
```
shell> unknowncmd
Unknown command: [unknowncmd].
```

---

## 步驟六：多個指令（Pipes）

管道透過 `pipe()`、`fork()` 與 `dup2()` 將前一個指令的 stdout 接到下一個指令的 stdin。

以 `ls | grep src | cat` 為例：
```
pipe0：ls 的 stdout → grep 的 stdin
pipe1：grep 的 stdout → cat 的 stdin

第 0 次迭代：fork ls，  stdout → pipe0[w]，prev_read = pipe0[r]
第 1 次迭代：fork grep，stdin  ← pipe0[r]，stdout → pipe1[w]，prev_read = pipe1[r]
第 2 次迭代：fork cat，stdin  ← pipe1[r]
```

試試看：
```
shell> ls | cat
shell> echo hello world | grep hello
shell> cat /etc/shells | sort | head -3
shell> echo a b c | grep b | cat | cat
```

---

## 步驟七：Number Pipe（`|N`）

在行尾加上 `|N`，會將該指令的 stdout 傳給**N 行後**的指令作為 stdin。

```
shell> ls |2
shell> echo hello
shell> cat
```

第 1 行（`ls |2`）建立管道並儲存讀取端。第 3 行（`cat`）接收其作為 stdin，第 2 行（`echo hello`）正常執行不受影響。

管道由 `main` 中的 `pending[]` 陣列管理，以行號為索引：
```
pending[目前行號 + N] = 管道讀取端 fd
```

當執行到第 `目前行號 + N` 行時，該指令的 stdin 會接上儲存的讀取端。

試試看：
```
shell> echo hello world |2
shell> echo 這是第二行
shell> cat
```

預期輸出：
```
這是第二行
hello world
```

---

## 執行所有測試

```bash
make test
```

依序執行 `helper`、`parse`、`env` 的單元測試，接著執行整合測試，將 shell 輸出與 `test/outputs/` 中的預期輸出檔案進行比對。
