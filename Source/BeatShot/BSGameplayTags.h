// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"

/**
 *	Singleton containing native gameplay tags.
 */
struct FBSGameplayTags
{
	static const FBSGameplayTags& Get() { return GameplayTags; }
	static void InitializeTags();
	
	FGameplayTag GameplayCue_Sprint;
	
	FGameplayTag Event;
	FGameplayTag Event_Montage;
	FGameplayTag Event_Montage_SpawnProjectile;
	
	FGameplayTag Input;
	FGameplayTag Input_Move;
	FGameplayTag Input_Look;
	FGameplayTag Input_Crouch;
	FGameplayTag Input_Fire;
	FGameplayTag Input_Sprint;
	FGameplayTag Input_Jump;

	FGameplayTag State;
	FGameplayTag State_Crouching;
	FGameplayTag State_Firing;
	FGameplayTag State_Jumping;
	FGameplayTag State_Moving;
	FGameplayTag State_Sprinting;
	FGameplayTag State_PlayingBSGameMode;
	
	FGameplayTag Target;
	FGameplayTag Target_State;
	FGameplayTag Target_State_Grid;
	FGameplayTag Target_State_Single;
	FGameplayTag Target_State_Multi;
	FGameplayTag Target_State_Tracking;

protected:

	void AddAllTags();
	void AddTag(FGameplayTag& OutTag, const FString& TagName);

private:

	static FBSGameplayTags GameplayTags;
};
