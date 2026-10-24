#!/usr/bin/env zsh
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
PROJECT_FILE="$ROOT_DIR/Embermere.uproject"
EDITOR_APP="/Users/Shared/Epic Games/UE_5.8/Engine/Binaries/Mac/UnrealEditor.app"
export DEVELOPER_DIR="${DEVELOPER_DIR:-/Applications/Xcode_26.1.1.app/Contents/Developer}"

if [[ ! -f "$PROJECT_FILE" || ! -d "$EDITOR_APP" || ! -d "$DEVELOPER_DIR" ]]; then
  echo "Embermere, Unreal 5.8, or the selected Xcode is missing." >&2
  exit 1
fi

if ! xcrun metal -v >/dev/null 2>&1; then
  echo "Refreshing xcrun's Metal tool lookup..."
  xcrun --kill-cache
fi
if ! xcrun metal -v >/dev/null 2>&1; then
  echo "Metal still cannot execute under $DEVELOPER_DIR." >&2
  echo "Inspect: DEVELOPER_DIR='$DEVELOPER_DIR' xcodebuild -showComponent MetalToolchain -json" >&2
  exit 1
fi

set +e
EDITOR_PROCESSES="$(pgrep -fl "$EDITOR_APP/Contents/MacOS/UnrealEditor" 2>&1)"
PROCESS_CHECK=$?
set -e
if (( PROCESS_CHECK == 0 )); then
  echo "Unreal Editor is already running. Quit it before launching another instance:" >&2
  echo "$EDITOR_PROCESSES" >&2
  exit 1
elif (( PROCESS_CHECK != 1 )); then
  echo "Could not verify whether Unreal Editor is already running:" >&2
  echo "$EDITOR_PROCESSES" >&2
  exit 1
fi

if lsof -nP -iTCP:8123 -sTCP:LISTEN >/dev/null 2>&1; then
  echo "Port 8123 is already in use; check its owner before launching Unreal." >&2
  exit 1
fi

open -n --fresh --env "DEVELOPER_DIR=$DEVELOPER_DIR" "$EDITOR_APP" --args \
  "$PROJECT_FILE" -ModelContextProtocolStartServer -ModelContextProtocolPort=8123
echo "Launched Embermere with $DEVELOPER_DIR and Unreal MCP on port 8123."
