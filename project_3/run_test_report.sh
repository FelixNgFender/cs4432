#!/usr/bin/env bash

set -euo pipefail

# === Parse arguments ===
KEEP_INTERMEDIATES=false
if [[ "${1:-}" == "--keep" ]]; then
  KEEP_INTERMEDIATES=true
fi

# === Setup directories ===
OUT_DIR="./out"
mkdir -p "$OUT_DIR"

TMP_OUT="$OUT_DIR/test_output.txt"
TMP_LOG="$OUT_DIR/test_log.txt"

# === Run test ===
make test >"$TMP_OUT"

# === Split program output into per-query files ===
split_output() {
  awk -v outdir="$OUT_DIR" '
    BEGIN { q = 0 }
    /Program is ready and waiting for user command./ { q++ }
    { print > (outdir "/query_" q ".txt") }
  ' "$TMP_OUT"
}

# === Extract relevant metrics from each query ===
extract_metrics() {
  local file="$1"
  grep -E "Index type|Number of data files read|Time taken" "$file"
}

# === Compare outputs before and after indexing ===
compare_query_outputs() {
  echo "" >>"$TMP_LOG"
  for i in 0 1 2; do
    local before_raw="$OUT_DIR/query_$((i + 1)).txt"
    local after_raw="$OUT_DIR/query_$((i + 5)).txt"

    local before_norm="$OUT_DIR/query_$((i + 1))_norm.txt"
    local after_norm="$OUT_DIR/query_$((i + 5))_norm.txt"

    # Extract only lines that look like record lines, then sort
    grep -E '^F[0-9]{2}-Rec[0-9]{3}' "$before_raw" | sort >"$before_norm"
    grep -E '^F[0-9]{2}-Rec[0-9]{3}' "$after_raw" | sort >"$after_norm"

    if diff -u "$before_norm" "$after_norm" >/dev/null; then
      echo "✅ Query $((i + 1)) and $((i + 5)) outputs match (ignoring order)" >>"$TMP_LOG"
    else
      echo "❌ Query $((i + 1)) and $((i + 5)) outputs differ (after sorting records)" >>"$TMP_LOG"
    fi

    # Clean up normalized files unless --keep
    if ! $KEEP_INTERMEDIATES; then
      rm -f "$before_norm" "$after_norm"
    fi
  done
}

# === Main Execution ===
split_output

echo "===== QUERY PERFORMANCE REPORT =====" >"$TMP_LOG"

for i in 1 2 3; do
  echo "--- Query $i (Before Index) ---" >>"$TMP_LOG"
  extract_metrics "$OUT_DIR/query_${i}.txt" >>"$TMP_LOG"
  echo "--- Query $((i + 4)) (After Index) ---" >>"$TMP_LOG"
  extract_metrics "$OUT_DIR/query_$((i + 4)).txt" >>"$TMP_LOG"
done

echo "" >>"$TMP_LOG"
echo "===== QUERY OUTPUT COMPARISON =====" >>"$TMP_LOG"
compare_query_outputs >>"$TMP_LOG"

cat "$TMP_LOG"

# === Cleanup if not keeping ===
if ! $KEEP_INTERMEDIATES; then
  rm -f "$OUT_DIR"/query_*.txt
fi
