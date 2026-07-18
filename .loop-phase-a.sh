#!/bin/bash
# Phase A ticket loop — emits a wake sentinel every 15 minutes.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
PROMPT_FILE="$ROOT/.loop-phase-a-prompt.json"

while true; do
  sleep 900
  payload="$(cat "$PROMPT_FILE")"
  echo "AGENT_LOOP_TICK_phase_a ${payload}"
done
