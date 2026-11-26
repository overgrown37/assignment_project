using UnrealBuildTool;
using System.Collections.Generic;

public class assignment_projectEditorTarget : TargetRules
{
	public assignment_projectEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("assignment_project");
	}
}
