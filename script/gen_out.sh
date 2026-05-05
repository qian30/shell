#!/usr/bin/env bash

INDIR="test/inputs"
OUTDIR="test/outputs"

# add test names here that use builtins
BUILTINS=(
    "01_env"
    "06_numpipe"
    "07_multi_numpipe"
)

is_builtin() {
    local name="$1"
    for b in "${BUILTINS[@]}"; do
        [ "$b" = "$name" ] && return 0
    done
    return 1
}

for input in "$INDIR"/*.in; do
    name=$(basename "$input" .in)

    if ! is_builtin "$name"; then
        while IFS= read -r line; do
            [ "$line" = "quit" ] && continue
            [ -z "$line" ] && continue
            eval "$line"
        done < "$input" > "$OUTDIR/$name.out" 2>/dev/null
        echo "Generated (system): $OUTDIR/$name.out"
    fi
done