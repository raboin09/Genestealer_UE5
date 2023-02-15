// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Genestealer : ModuleRules
{
	public Genestealer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {"RuntimeDataTable", "Http", "Json", "JsonUtilities", "Core", "CoreUObject", "Engine", "InputCore", "GameplayTags", "AIModule", "UMG", "PhysicsCore", "Niagara", "NavigationSystem", "EnhancedInput", "SMSystem", "GameplayTasks" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
