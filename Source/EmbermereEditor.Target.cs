using UnrealBuildTool;
using System.Collections.Generic;

public class EmbermereEditorTarget : TargetRules
{
	public EmbermereEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("Embermere");
	}
}
