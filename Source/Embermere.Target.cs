using UnrealBuildTool;
using System.Collections.Generic;

public class EmbermereTarget : TargetRules
{
	public EmbermereTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("Embermere");
	}
}
