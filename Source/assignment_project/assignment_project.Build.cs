using UnrealBuildTool;

public class assignment_project : ModuleRules
{
	public assignment_project(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.Add(ModuleDirectory);

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore",
			"AIModule", "GameplayTasks", "NavigationSystem",
			"UMG", "Slate", "SlateCore"
		});

		// 실습 11.04: Enhanced Input 플러그인 사용
		PrivateDependencyModuleNames.AddRange(new string[] { "EnhancedInput" });
	}
}
