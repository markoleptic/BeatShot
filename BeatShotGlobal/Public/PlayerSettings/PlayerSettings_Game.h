// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSConstants.h"
#include "PlayerSettings_Game.generated.h"

/** Game settings. */
USTRUCT(BlueprintType)
struct BEATSHOTGLOBAL_API FPlayerSettings_Game
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FLinearColor StartTargetColor;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor PeakTargetColor;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor EndTargetColor;

	UPROPERTY(BlueprintReadOnly)
	bool bUseSeparateOutlineColor;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor TargetOutlineColor;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor InactiveTargetColor;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor TakingTrackingDamageColor;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor NotTakingTrackingDamageColor;

	UPROPERTY(BlueprintReadOnly)
	bool bShowStreakCombatText;

	UPROPERTY(BlueprintReadOnly)
	int32 CombatTextFrequency;

	UPROPERTY(BlueprintReadWrite)
	bool bShouldRecoil;

	UPROPERTY(BlueprintReadWrite)
	bool bAutomaticFire;

	UPROPERTY(BlueprintReadWrite)
	bool bShowBulletDecals;

	UPROPERTY(BlueprintReadWrite)
	bool bShowBulletTracers;

	UPROPERTY(BlueprintReadWrite)
	bool bShowMuzzleFlash;

	UPROPERTY(BlueprintReadWrite)
	bool bShowCharacterMesh;

	UPROPERTY(BlueprintReadWrite)
	bool bShowWeaponMesh;

	UPROPERTY(BlueprintReadWrite)
	bool bShowHitTimingWidget;

	UPROPERTY(BlueprintReadWrite)
	bool bNightModeSelected;

	UPROPERTY(BlueprintReadWrite)
	bool bShowLightVisualizers;

	UPROPERTY(BlueprintReadWrite)
	bool bShow_LVFrontBeam;

	UPROPERTY(BlueprintReadWrite)
	bool bShow_LVLeftBeam;

	UPROPERTY(BlueprintReadWrite)
	bool bShow_LVRightBeam;

	UPROPERTY(BlueprintReadWrite)
	bool bShow_LVTopBeam;

	UPROPERTY(BlueprintReadWrite)
	bool bShow_LVLeftCube;

	UPROPERTY(BlueprintReadWrite)
	bool bShow_LVRightCube;

	FPlayerSettings_Game()
	{
		bShowStreakCombatText = true;
		CombatTextFrequency = Constants::DefaultCombatTextFrequency;
		StartTargetColor = Constants::DefaultStartTargetColor;
		PeakTargetColor = Constants::DefaultPeakTargetColor;
		EndTargetColor = Constants::DefaultEndTargetColor;
		bUseSeparateOutlineColor = false;
		TargetOutlineColor = Constants::DefaultTargetOutlineColor;
		InactiveTargetColor = Constants::DefaultInactiveTargetColor;
		TakingTrackingDamageColor = Constants::DefaultTakingTrackingDamageColor;
		NotTakingTrackingDamageColor = Constants::DefaultNotTakingTrackingDamageColor;
		bShouldRecoil = false;
		bAutomaticFire = true;
		bShowBulletDecals = true;
		bShowBulletTracers = true;
		bShowMuzzleFlash = true;
		bShowCharacterMesh = true;
		bShowWeaponMesh = true;
		bShowHitTimingWidget = true;
		bNightModeSelected = false;
		bShowLightVisualizers = true;
		bShow_LVFrontBeam = false;
		bShow_LVLeftBeam = false;
		bShow_LVRightBeam = false;
		bShow_LVTopBeam = true;
		bShow_LVLeftCube = true;
		bShow_LVRightCube = true;
	}

	/** Resets all game settings not on the wall menu. */
	void ResetToDefault()
	{
		bShowStreakCombatText = true;
		CombatTextFrequency = Constants::DefaultCombatTextFrequency;
		StartTargetColor = Constants::DefaultStartTargetColor;
		PeakTargetColor = Constants::DefaultPeakTargetColor;
		EndTargetColor = Constants::DefaultEndTargetColor;
		bUseSeparateOutlineColor = false;
		TargetOutlineColor = Constants::DefaultTargetOutlineColor;
		InactiveTargetColor = Constants::DefaultInactiveTargetColor;
		TakingTrackingDamageColor = Constants::DefaultTakingTrackingDamageColor;
		NotTakingTrackingDamageColor = Constants::DefaultNotTakingTrackingDamageColor;
		bShouldRecoil = false;
		bAutomaticFire = true;
		bShowBulletDecals = true;
		bShowBulletTracers = true;
		bShowMuzzleFlash = true;
		bShowCharacterMesh = true;
		bShowWeaponMesh = true;
		bShowHitTimingWidget = true;
	}
};
