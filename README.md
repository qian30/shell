# Shell

A Unix shell implementation in C supporting builtin commands, external commands, and pipes.

## Build & Run

```bash
make        # build shell
./shell     # run interactive shell
```

## Builtin Commands

| Command | Usage | Description |
|---|---|---|
| `setenv` | `setenv <key> <value>` | Set environment variable |
| `printenv` | `printenv <key>` | Print environment variable |
| `cd` | `cd <dir>` | Change directory |
| `help` | `help` | Print available commands |
| `quit` | `quit` | Exit the shell |

## Pipes

```
shell> ls | grep src
shell> ls | cat | grep src      # multi-stage
shell> ls |2                    # numbered pipe: stdout sent to stdin 2 lines later
```

## Testing

### Unit Tests

```bash
make env      # test setenv / printenv
make helper   # test strip_string / split_string
make parse    # test command parser
```

### Integration Tests
```bash
make test
```

Builds a non-interactive shell, regenerates expected outputs, then diffs each `test/inputs/*.in` against `test/outputs/*.out`.
```
[SUCCESS] 01_env
[SUCCESS] 02_echo
[SUCCESS] 03_single
[SUCCESS] 04_pipe
[SUCCESS] 05_mix

--- Test Report Summary ---
Passed: 5 / 5
STATUS: ALL PASSED
```

### Adding a Test Case

1. Create `test/inputs/XX_name.in` with commands, ending with `quit`
2. Run `make test` — outputs are auto-generated and compared
3. If the test uses builtins (`setenv`, `printenv`, etc.), add the name to `BUILTINS` in `script/gen_out.sh`
