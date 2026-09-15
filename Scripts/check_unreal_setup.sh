#!/usr/bin/env zsh
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
PROJECT_FILE="$ROOT_DIR/Embermere.uproject"
FAILED=0

echo "Embermere setup check"
echo "Project: $PROJECT_FILE"
echo

if [[ -f "$PROJECT_FILE" ]]; then
  echo "[ok] Found Embermere.uproject"
else
  echo "[missing] Embermere.uproject"
  FAILED=1
fi

if [[ -d "/Applications/Epic Games Launcher.app" || -d "$HOME/Applications/Epic Games Launcher.app" ]]; then
  echo "[ok] Epic Games Launcher appears to be installed"
else
  echo "[warning] Epic Games Launcher not found; an existing Unreal installation can still be used"
fi

if [[ -d "/Applications/Xcode.app" ]]; then
  echo "[ok] Found /Applications/Xcode.app"
else
  echo "[missing] Full Xcode not found at /Applications/Xcode.app"
  FAILED=1
fi

DEVELOPER_DIR="$(xcode-select -p 2>/dev/null || true)"
if [[ "$DEVELOPER_DIR" == "/Applications/Xcode.app/Contents/Developer" ]]; then
  echo "[ok] xcode-select points at full Xcode"
else
  echo "[missing] xcode-select is '$DEVELOPER_DIR' but Unreal expects /Applications/Xcode.app/Contents/Developer"
  FAILED=1
fi

if ! zsh "$ROOT_DIR/Scripts/check_metal_toolchain.sh"; then
  FAILED=1
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
  FAILED=1
fi

if [[ -f "$PROJECT_FILE" ]] && grep -q '"ModelContextProtocol"' "$PROJECT_FILE"; then
  echo "[ok] Project requests Unreal MCP plugin"
else
  echo "[missing] Project does not request ModelContextProtocol plugin"
  FAILED=1
fi

if [[ -f "$PROJECT_FILE" ]] && grep -q '"AllToolsets"' "$PROJECT_FILE"; then
  echo "[ok] Project requests AllToolsets plugin"
else
  echo "[missing] Project does not request AllToolsets plugin"
  FAILED=1
fi

if [[ -f "$PROJECT_FILE" ]] && grep -q '"PythonScriptPlugin"' "$PROJECT_FILE"; then
  echo "[ok] Project requests PythonScriptPlugin"
else
  echo "[missing] Project does not request PythonScriptPlugin"
  FAILED=1
fi

if [[ -f "$ROOT_DIR/.mcp.json" || -f "$ROOT_DIR/.codex/config.toml" || -f "$ROOT_DIR/codex.toml" ]]; then
  echo "[ok] Found a generated MCP client config"
  if [[ -f "$ROOT_DIR/.codex/config.toml" ]]; then
    MCP_URL="$(grep -E '^[[:space:]]*url[[:space:]]*=' "$ROOT_DIR/.codex/config.toml" | head -n 1 | sed -E 's/.*"([^"]+)".*/\1/')"
    if [[ "$MCP_URL" == "http://127.0.0.1:8123/mcp" ]]; then
      echo "[ok] Codex MCP config points at $MCP_URL"
    else
      echo "[missing] Codex MCP config points at '$MCP_URL' but this project expects http://127.0.0.1:8123/mcp"
      FAILED=1
    fi
  fi
else
  echo "[todo] MCP client config not found yet. Generate it inside Unreal with: ModelContextProtocol.GenerateClientConfig Codex"
  echo "       Note: JSON clients commonly use .mcp.json; Codex may generate a TOML config."
  FAILED=1
fi

echo
if (( FAILED )); then
  echo "[blocked] Resolve required setup checks before a fresh build."
else
  echo "[ok] Local setup checks passed; live MCP readiness is a separate check."
fi
echo "If MCP is not already running, use Unreal Output Log in Cmd mode (not Python):"
echo "  ModelContextProtocol.StartServer 8123"
exit "$FAILED"
