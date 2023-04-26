// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

namespace Constants
{
	constexpr float MinValue_BandFrequency = 0;
	constexpr float MaxValue_BandFrequency = 22720;

	/** The length of the countdown timer */
	constexpr int32 CountdownTimerLength = 3;

	/** The ratio between the scale of the SphereMesh to OutlineMesh, used to apply a constant proportioned outline regardless of the scale of the target */
	constexpr float BaseToOutlineRatio = 0.9;

	/** The value to divide the game sensitivity by to convert to Csgo sensitivity */
	constexpr float CsgoMultiplier = 3.18;

	const FVector VisualizerLocation = {4000, -1935, 210};
	const FVector Visualizer2Location = {4000, 1935, 210};
	const FRotator VisualizerRotation = {0, 90, 90};

	const FVector BeamVisualizerLocation = {0, 1920, 1320};
	const FRotator BeamRotation = {0, 0, 0};

	constexpr float MinCubeHeightScale = 1;

	constexpr float MaxCubeHeightScale = 4;

	constexpr int32 CustomDepthStencilValue = 0;

	const FVector CubeOffset = FVector(0, -120, 0);

	/** The distance to trace the line */
	constexpr float TraceDistance = 999999;

	/** The max value of the Spotlight intensity */
	constexpr float MaxSpotlightIntensity = 16000000;

	/** The max value of the BeamEndLight intensity */
	constexpr float MaxBeamEndLightIntensity = 80000;

	const FVector DefaultSpotlightLimbOffset = {0, 0, -18};
	const FVector DefaultSpotlightHeadOffset = {0, 0, -41};
	const FRotator DefaultSpotlightHeadRotation = {-90, 0, 0};
	const FVector DefaultSpotlightOffset = {22, 0, 0};
	const FVector InitialBeamLightLocation = {0, 0, 1340};
	const FRotator BeamLightRotation = {0, 0, 0};
	const FVector BeamLightOffset = {0, 100, 0};

	const FVector MoonMeshOffset = {250000, 0, 250000};
	const FVector MoonGlowMeshScale = {2, 2, 2};
	const FVector MoonMeshScale = {400, 400, 400};
	const FVector MoonLightScale = {0.0025, 0.0025, 0.0025};
	constexpr float SphereCompRadius = 400000;
	const FVector MaxFloorHeight = {-4000, 0, 500};
	const FVector MinFloorHeight = {-4000, 0, 0};

	/** How much to shrink the target scale to during BeatGrid after successful hit */
	constexpr float MinShrinkTargetScale = 0.1f;

	constexpr float DayNightCycleSpeed = 20;

	constexpr float VideoSettingsTimeoutLength = 10.f;

	/** The maximum width of the beam, used to scale the alpha value */
	constexpr float MaxBeamWidth = 50;

	/** Offset to apply to the location of the BeamEndLight */
	constexpr float BeamEndLightOffset = 5;
	
	constexpr float SphereDiameter = 100.f;
	constexpr float SphereRadius = 50.f;
	
	constexpr float MinValue_PlayerDelay = 0;
	constexpr float MaxValue_PlayerDelay = 0.5;
	constexpr float SnapSize_PlayerDelay = 0.01;

	constexpr float MinValue_Lifespan = 0.25;
	constexpr float MaxValue_Lifespan = 2;
	constexpr float SnapSize_Lifespan = 0.01;

	constexpr float MinValue_TargetSpawnCD = 0.05;
	constexpr float MaxValue_TargetSpawnCD = 2;
	constexpr float SnapSize_TargetSpawnCD = 0.01;

	constexpr float MinValue_MinTargetDistance = 0;
	constexpr float MaxValue_MinTargetDistance = 600;
	constexpr float SnapSize_MinTargetDistance = 5;

	constexpr float MinValue_HorizontalSpread = 200;
	constexpr float MaxValue_HorizontalSpread = 3200;
	constexpr float SnapSize_HorizontalSpread = 100;
	
	constexpr float MinValue_VerticalSpread = 200;
	constexpr float MaxValue_VerticalSpread = 1000;
	constexpr float SnapSize_VerticalSpread = 100;
	
	constexpr float MinValue_ForwardSpread = 100;
	constexpr float MaxValue_ForwardSpread = 5000;
	
	constexpr float MinValue_TargetScale = 0.1;
	constexpr float MaxValue_TargetScale = 2;
	constexpr float SnapSize_TargetScale = 0.01;

	constexpr float MinValue_TargetSpeed = 300;
	constexpr float MaxValue_TargetSpeed = 1000;
	constexpr float SnapSize_TargetSpeed = 10;

	constexpr float MinValue_FloorDistance = 110;
	constexpr float MaxValue_FloorDistance = 1000;
	constexpr float SnapSize_FloorDistance = 10;
	
	// AI
	
	constexpr float MinValue_Alpha = 0.1;
	constexpr float MaxValue_Alpha = 1;
	constexpr float SnapSize_Alpha = 0.01;
	
	constexpr float MinValue_Epsilon = 0;
	constexpr float MaxValue_Epsilon = 1;
	constexpr float SnapSize_Epsilon = 0.01;
	
	constexpr float MinValue_Gamma = 0.1;
	constexpr float MaxValue_Gamma = 1;
	constexpr float SnapSize_Gamma = 0.01;
	
	// BeatGrid
	
	constexpr float MinValue_BeatGridHorizontalSpacing = 10;
	constexpr float MaxValue_BeatGridHorizontalSpacing = 3200;
	constexpr float SnapSize_BeatGridHorizontalSpacing = 10;

	constexpr float MinValue_BeatGridVerticalSpacing = 10;
	constexpr float MaxValue_BeatGridVerticalSpacing = 1000;
	constexpr float SnapSize_BeatGridVerticalSpacing = 10;
	
	constexpr int32 MinValue_NumBeatGridHorizontalTargets = 2;
	constexpr int32 MaxValue_NumBeatGridHorizontalTargets = 35;
	constexpr int32 SnapSize_NumBeatGridHorizontalTargets = 1;

	constexpr int32 MinValue_NumBeatGridVerticalTargets = 2;
	constexpr int32 MaxValue_NumBeatGridVerticalTargets = 15;
	constexpr int32 SnapSize_NumBeatGridVerticalTargets = 1;

	/** The default location to spawn the SpawnBox */
	const FVector DefaultTargetSpawnerLocation = {3700.f, 0.f, 160.f};

	/** Default distance between floor and bottom of the SpawnBox */
	constexpr float DistanceFromFloor = 110.f;

	/** Distance between floor and HeadshotHeight */
	constexpr float HeadshotHeight = 160.f;
	
	constexpr FLinearColor LightMenuBrushColor = FLinearColor(0, 0, 0, 0.1);

	constexpr FLinearColor DarkMenuBrushColor = FLinearColor(0, 0, 0, 0.2);

	/** The color used to change the GameModeButton color to when selected */
	constexpr FLinearColor BeatShotBlue = FLinearColor(0.049707, 0.571125, 0.83077, 1.0);
}
