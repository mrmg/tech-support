#!/bin/bash
# Phase B ticket loop — emits a wake sentinel every 5 minutes.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
PROMPT_FILE="$ROOT/.loop-phase-b-prompt.json"

while true; do
  sleep 300
  payload="$(cat "$PROMPT_FILE")"
  echo "AGENT_LOOP_TICK_phase_b ${payload}"
done
