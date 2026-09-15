"""Readiness regressions using fake xcrun, never installing or accepting anything."""
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest


SCRIPTS = Path(__file__).resolve().parent
LICENSE_ERROR = (
    "You have not agreed to the Xcode license agreements. Please run "
    "'sudo xcodebuild -license' from within a Terminal window to review "
    "and agree to the Xcode and Apple SDKs license."
)


class UnrealSetupTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="embermere setup test ")
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        scripts = self.root / "Scripts"
        scripts.mkdir()
        for name in ("check_unreal_setup.sh", "check_metal_toolchain.sh"):
            shutil.copy2(SCRIPTS / name, scripts / name)
        shutil.copy2(SCRIPTS.parent / "Embermere.uproject", self.root)
        (self.root / ".codex").mkdir()
        (self.root / ".codex/config.toml").write_text(
            '[mcp_servers.unreal-mcp]\nurl = "http://127.0.0.1:8123/mcp"\n'
        )
        self.bin = self.root / "bin"
        self.bin.mkdir()
        xcrun = self.bin / "xcrun"
        xcrun.write_text(
            '#!/bin/sh\n'
            'printf "%s\\n" "$*" >> "$EMBERMERE_TEST_CALLS"\n'
            'printf "%s\\n" "$EMBERMERE_TEST_OUTPUT" >&2\n'
            'exit "$EMBERMERE_TEST_STATUS"\n'
        )
        xcrun.chmod(0o755)

    def run_check(self, script, code=0, diagnostic="Apple metal version fixture"):
        calls = self.root / "calls"
        env = dict(os.environ, PATH=f"{self.bin}:/usr/bin:/bin:/usr/sbin:/sbin",
                   EMBERMERE_TEST_CALLS=str(calls), EMBERMERE_TEST_STATUS=str(code),
                   EMBERMERE_TEST_OUTPUT=diagnostic)
        result = subprocess.run(
            ["/bin/zsh", str(self.root / "Scripts" / script)],
            cwd=self.root, env=env, text=True, capture_output=True, timeout=10,
        )
        self.assertEqual(calls.read_text().splitlines(), ["metal -v"])
        return result.returncode, result.stdout + result.stderr

    def test_license_blocks_overall_readiness(self):
        code, output = self.run_check("check_unreal_setup.sh", 69, LICENSE_ERROR)
        self.assertNotEqual(code, 0)
        self.assertIn("[blocked] Xcode license", output)
        self.assertNotIn("downloadComponent", output)

    def test_missing_project_fails_readiness(self):
        (self.root / "Embermere.uproject").unlink()
        code, output = self.run_check("check_unreal_setup.sh")
        self.assertNotEqual(code, 0)
        self.assertIn("[missing] Embermere.uproject", output)
        self.assertNotIn("No such file", output)

    def test_wrong_mcp_endpoint_fails_readiness(self):
        (self.root / ".codex/config.toml").write_text(
            '[mcp_servers.unreal-mcp]\nurl = "http://127.0.0.1:9999/mcp"\n'
        )
        code, output = self.run_check("check_unreal_setup.sh")
        self.assertNotEqual(code, 0)
        self.assertIn("expects http://127.0.0.1:8123/mcp", output)

    def test_missing_client_config_fails_readiness(self):
        (self.root / ".codex/config.toml").unlink()
        code, output = self.run_check("check_unreal_setup.sh")
        self.assertNotEqual(code, 0)
        self.assertIn("MCP client config not found", output)

    def test_missing_plugin_fails_readiness(self):
        project = self.root / "Embermere.uproject"
        data = json.loads(project.read_text())
        data["Plugins"] = [p for p in data["Plugins"] if p["Name"] != "AllToolsets"]
        project.write_text(json.dumps(data))
        code, output = self.run_check("check_unreal_setup.sh")
        self.assertNotEqual(code, 0)
        self.assertIn("Project does not request AllToolsets", output)

    def test_healthy_metal(self):
        code, output = self.run_check("check_metal_toolchain.sh")
        self.assertEqual(code, 0)
        self.assertIn("[ok] Metal compiler executes", output)
        self.assertNotIn("license", output)

    def test_license_diagnostic_not_missing_component(self):
        code, output = self.run_check("check_metal_toolchain.sh", 69, LICENSE_ERROR)
        self.assertNotEqual(code, 0)
        self.assertIn("[blocked] Xcode license", output)
        self.assertIn("review", output)
        self.assertNotIn("downloadComponent", output)

    def test_exit_69_alone_is_not_a_license_diagnosis(self):
        code, output = self.run_check("check_metal_toolchain.sh", 69, "Tool unavailable.")
        self.assertNotEqual(code, 0)
        self.assertIn("exit 69", output)
        self.assertIn("Tool unavailable.", output)
        self.assertNotIn("[blocked] Xcode license", output)
        self.assertNotIn("downloadComponent", output)

    def test_reported_missing_component(self):
        code, output = self.run_check(
            "check_metal_toolchain.sh", 72,
            "cannot execute tool 'metal' due to missing Metal Toolchain; "
            "use: xcodebuild -downloadComponent MetalToolchain",
        )
        self.assertNotEqual(code, 0)
        self.assertIn("[missing] Metal Toolchain", output)
        self.assertIn("downloadComponent MetalToolchain", output)
        self.assertNotIn("[blocked] Xcode license", output)

    def test_unknown_failure_preserves_diagnostic(self):
        code, output = self.run_check("check_metal_toolchain.sh", 1, "SDK path is unavailable.")
        self.assertNotEqual(code, 0)
        self.assertIn("exit 1", output)
        self.assertIn("SDK path is unavailable.", output)
        self.assertNotIn("downloadComponent", output)


if __name__ == "__main__":
    unittest.main()
