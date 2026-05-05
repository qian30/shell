#!/usr/bin/env bash

SHELL_BIN="./bin/test_shell"
PASS=0
FAIL=0

run_test() {
    local name="$1"
    local input="$2"
    local expected="$3"

    local actual
    actual=$(< "$input" $SHELL_BIN 2>/dev/null | sed 's/^shell> //')

    # normalize both: strip trailing blank lines
    local actual_norm expected_norm
    actual_norm=$(printf '%s' "$actual" | sed -e 's/[[:space:]]*$//')
    expected_norm=$(sed -e 's/[[:space:]]*$//' "$expected")

    if diff <(printf '%s\n' "$actual_norm") <(printf '%s\n' "$expected_norm") > /dev/null 2>&1; then
        echo -e "\033[32m[SUCCESS]\033[0m $name"
        PASS=$((PASS + 1))
    else
        echo -e "\033[31m[ERROR]\033[0m   $name"
        diff -u --color=always \
             --label "expected" <(printf '%s\n' "$expected_norm") \
             --label "got"      <(printf '%s\n' "$actual_norm") \
        | tail -n +3 \
        | sed 's/^/          /'
        FAIL=$((FAIL + 1))
    fi
}

# --- discover and run all cases ---
for input_file in test/inputs/*.in; do
    name=$(basename "$input_file" .in)
    expected_file="test/outputs/$name.out"

    if [ ! -f "$expected_file" ]; then
        echo "MISSING expected: $expected_file"
        continue
    fi

    run_test "$name" "$input_file" "$expected_file"
done

# --- summary ---
echo ""
echo "--- Test Report Summary ---"
echo "Passed: $PASS / $((PASS + FAIL))"
if [ $FAIL -eq 0 ]; then
    echo -e "\033[32mSTATUS: ALL PASSED\033[0m"
    exit 0
else
    echo -e "\033[31mSTATUS: FAILED ($FAIL errors)\033[0m"
    exit 1
fi
