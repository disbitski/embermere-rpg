#!/usr/bin/env zsh
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
PROJECT_FILE="$ROOT_DIR/Embermere.uproject"

echo "Embermere setup check"
echo "Project: $PROJECT_FILE"
echo

if [[ -f "$PROJECT_FILE" ]]; then
  echo "[ok] Found Embermere.uproject"
else
  echo "[missing] Embermere.uproject"
fi

if [[ -d "/Applications/Epic Games Launcher.app" || -d "$HOME/Applications/Epic Games Launcher.app" ]]; then
  echo "[ok] Epic Games Launcher appears to be installed"
else
  echo "[missing] Epic Games Launcher not found in /Applications or ~/Applications"
fi

if [[ -d "/Applications/Xcode.app" ]]; then
  echo "[ok] Found /Applications/Xcode.app"
else
  echo "[missing] Full Xcode not found at /Applications/Xcode.app"
fi

DEVELOPER_DIR="$(xcode-select -p 2>/dev/null || true)"
if [[ "$DEVELOPER_DIR" == "/Applications/Xcode.app/Contents/Developer" ]]; then
  echo "[ok] xcode-select points at full Xcode"
else
  echo "[missing] xcode-select is '$DEVELOPER_DIR' but Unreal expects /Applications/Xcode.app/Contents/Developer"
fi

if xcrun metal -v >/dev/null 2>&1; then
  echo "[ok] Metal compiler executes"
else
  echo "[missing] Metal compiler cannot execute yet"
  echo "          Run from normal Terminal: xcodebuild -downloadComponent MetalToolchain"
fi

UE_CANDIDATES=(
  "/Users/Shared/Epic Games/UE_5.8/Engine/Binaries/Mac/UnrealEditor.app"
  "/Users/Shared/Epic Games/UE_5.8/Engine/Binaries/Mac/UnrealEditor"
  "/Applications/UE_5.8/Engine/Binaries/Mac/UnrealEditor.app"
  "/Applications/UE_5.8/Engine/Binaries/Mac/UnrealEditor"
)

FOUND_UE=""
for candidate in "${UE_CANDIDATES[@]}"; do
  if [[ -e "$candidate" ]]; then
    FOUND_UE="$candidate"
    break
  fi
done

if [[ -n "$FOUND_UE" ]]; then
  echo "[ok] Found Unreal Editor 5.8 candidate: $FOUND_UE"
else
  echo "[missing] Unreal Editor 5.8 not found in common install locations"
fi

if grep -q '"ModelContextProtocol"' "$PROJECT_FILE"; then
  echo "[ok] Project requests Unreal MCP plugin"
else
  echo "[missing] Project does not request ModelContextProtocol plugin"
fi

if grep -q '"AllToolsets"' "$PROJECT_FILE"; then
  echo "[ok] Project requests AllToolsets plugin"
else
  echo "[missing] Project does not request AllToolsets plugin"
fi

if grep -q '"PythonScriptPlugin"' "$PROJECT_FILE"; then
  echo "[ok] Project requests PythonScriptPlugin"
else
  echo "[missing] Project does not request PythonScriptPlugin"
fi

if [[ -f "$ROOT_DIR/.mcp.json" || -f "$ROOT_DIR/.codex/config.toml" || -f "$ROOT_DIR/codex.toml" ]]; then
  echo "[ok] Found a generated MCP client config"
else
  echo "[todo] MCP client config not found yet. Generate it inside Unreal with: ModelContextProtocol.GenerateClientConfig Codex"
  echo "       Note: JSON clients commonly use .mcp.json; Codex may generate a TOML config."
fi

echo
echo "Next Unreal console commands after opening the project:"
echo "  ModelContextProtocol.StartServer 8123"
echo "  ModelContextProtocol.GenerateClientConfig Codex"
