// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"

class UGameplayTagsManager;

/**
 *	Singleton containing native gameplay tags.
 */
struct FBSGameplayTags
{
	static const FBSGameplayTags& Get() { return GameplayTags; }
	static void InitializeTags();

	FGameplayTag GameplayCue_Sprint;
	FGameplayTag GameplayCue_FireGun_Impact;

	FGameplayTag Data;
	FGameplayTag Data_Damage;

	FGameplayTag Event;
	FGameplayTag Event_Montage;
	FGameplayTag Event_Montage_EndAbility;
	FGameplayTag Event_Montage_SpawnProjectile;

	FGameplayTag Input;
	FGameplayTag Input_Move;
	FGameplayTag Input_Look;
	FGameplayTag Input_Crouch;
	FGameplayTag Input_Fire;
	FGameplayTag Input_Sprint;
	FGameplayTag Input_Jump;
	FGameplayTag Input_Interact;
	FGameplayTag Input_ShiftInteract;

	FGameplayTag Ability;
	FGameplayTag Ability_Fire;
	FGameplayTag Ability_Jump;
	FGameplayTag Ability_Sprint;
	FGameplayTag Ability_Crouch;
	FGameplayTag Ability_Interact;
	FGameplayTag Ability_ShiftInteract;

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
	void AddAllTags(UGameplayTagsManager& Manager);
	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);

private:
	static FBSGameplayTags GameplayTags;
};
