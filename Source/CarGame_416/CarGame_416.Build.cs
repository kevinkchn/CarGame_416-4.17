// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CarGame_416 : ModuleRules
{
	public CarGame_416(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "PhysXVehicles", "HeadMountedDisplay", "OnlineSubsystem", "OnlineSubsystemUtils", "UMG", "Slate", "SlateCore" });

        DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

        Definitions.Add("HMD_MODULE_INCLUDED=1");
	}
}
