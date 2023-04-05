// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

namespace Constants
{
	constexpr float MinValue_BandFrequency = 0;
	constexpr float MaxValue_BandFrequency = 22720;
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
	constexpr int32 MaxValue_NumBeatGridHorizontalTargets = 40;
	constexpr int32 SnapSize_NumBeatGridHorizontalTargets = 1;

	constexpr int32 MinValue_NumBeatGridVerticalTargets = 2;
	constexpr int32 MaxValue_NumBeatGridVerticalTargets = 20;
	constexpr int32 SnapSize_NumBeatGridVerticalTargets = 1;
}
