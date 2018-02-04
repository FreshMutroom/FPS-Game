// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FPS_Project : ModuleRules
{
	public FPS_Project(ReadOnlyTargetRules Target) : base (Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" , "UMG" , "GameplayTasks" , "Foliage" , "LevelSequence" , "MovieScene" , "Landscape" , "MediaAssets" , "Paper2D" });

        // This line has been added for the HUD
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
	}
}
