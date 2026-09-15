#!/usr/bin/env zsh
set -euo pipefail
setopt extendedglob
autoload -Uz is-at-least

if (( $# != 2 )); then
  echo "Usage: $0 Xcode-Info.plist Unreal-Apple_SDK.json" >&2
  exit 1
fi

if ! XCODE_VERSION="$(plutil -extract CFBundleShortVersionString raw -o - "$1" 2>/dev/null)" ||
   ! MIN_VERSION="$(plutil -extract MinVersion raw -o - "$2" 2>/dev/null)" ||
   ! MAX_VERSION="$(plutil -extract MaxVersion raw -o - "$2" 2>/dev/null)" ||
   ! MAIN_VERSION="$(plutil -extract MainVersion raw -o - "$2" 2>/dev/null)"; then
  echo "[blocked] Cannot read Xcode version or installed Unreal Apple SDK rules"
  exit 1
fi

for value in "$XCODE_VERSION" "$MIN_VERSION" "$MAX_VERSION" "$MAIN_VERSION"; do
  if [[ "$value" != <->(.<->)# ]]; then
    echo "[blocked] Malformed Xcode/Unreal version: $value"
    exit 1
  fi
done
if ! is-at-least "$MIN_VERSION" "$MAX_VERSION"; then
  echo "[blocked] Unreal Apple SDK version range is reversed"
  exit 1
fi

if ! is-at-least "$MIN_VERSION" "$XCODE_VERSION" ||
   ! is-at-least "$XCODE_VERSION" "$MAX_VERSION"; then
  echo "[blocked] Xcode $XCODE_VERSION is outside installed Unreal's declared range $MIN_VERSION through $MAX_VERSION"
  echo "          Engine-preferred Xcode: $MAIN_VERSION. Use a compatible side-by-side installation."
  echo "          Do not change engine SDK limits to bypass validation."
  exit 1
fi

echo "[ok] Xcode $XCODE_VERSION is within Unreal's declared range $MIN_VERSION through $MAX_VERSION"
echo "     Engine-preferred: $MAIN_VERSION. A successful fresh build is still required."
