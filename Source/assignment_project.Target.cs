using UnrealBuildTool;
using System.Collections.Generic;

public class assignment_projectTarget : TargetRules
{
	public assignment_projectTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("assignment_project");
	}
}
