#!/usr/bin/env zsh
set -euo pipefail

# A failed probe is not necessarily a missing compiler. Keep the original
# diagnostic, and never accept a license or install a component from a check.
if METAL_OUTPUT="$(xcrun metal -v 2>&1)"; then
  echo "[ok] Metal compiler executes"
  exit 0
else
  METAL_EXIT=$?
fi

case "$METAL_OUTPUT" in
  *"not agreed to the Xcode license"*)
    echo "[blocked] Xcode license has not been accepted"
    echo "          Open Xcode to review its license, or run sudo xcodebuild -license in Terminal."
    echo "          This check will not accept it on your behalf."
    ;;
  *"downloadComponent MetalToolchain"*)
    echo "[missing] Metal Toolchain: xcrun explicitly requests the component"
    echo "          Review the diagnostic before installing from your normal Terminal."
    ;;
  *)
    echo "[blocked] Metal compiler probe failed (exit $METAL_EXIT)"
    echo "          Resolve the diagnostic before rebuilding Unreal."
    ;;
esac
printf '%s\n' "$METAL_OUTPUT" >&2
exit 1
