// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"

class UGameplayTagsManager;

/** Singleton containing native gameplay tags */
struct FBSGameplayTags
{
	static const FBSGameplayTags& Get() { return GameplayTags; }
	static void InitializeTags();

	FGameplayTag GameplayCue_Sprint;
	FGameplayTag GameplayCue_MuzzleFlash;
	FGameplayTag GameplayCue_FireGun_Impact;
	FGameplayTag GameplayCue_TrackGun_Hit;
	FGameplayTag GameplayCue_TrackGun_Miss;

	FGameplayTag Data;
	FGameplayTag Data_Damage;
	FGameplayTag Data_Cooldown;

	FGameplayTag Event;
	FGameplayTag Event_Montage;
	FGameplayTag Event_Montage_EndAbility;
	FGameplayTag Event_Montage_SpawnProjectile;

	FGameplayTag Input;
	FGameplayTag Input_Move;
	FGameplayTag Input_Move_Forward;
	FGameplayTag Input_Move_Backward;
	FGameplayTag Input_Move_Left;
	FGameplayTag Input_Move_Right;
	FGameplayTag Input_Look;
	FGameplayTag Input_Crouch;
	FGameplayTag Input_Fire;
	FGameplayTag Input_Walk;
	FGameplayTag Input_Sprint;
	FGameplayTag Input_Jump;
	FGameplayTag Input_Interact;
	FGameplayTag Input_ShiftInteract;
	FGameplayTag Input_Inspect;
	FGameplayTag Input_KnifeAttack;
	FGameplayTag Input_BackStab;
	FGameplayTag Input_EquipmentSlot;
	FGameplayTag Input_EquipmentSlot_1;
	FGameplayTag Input_EquipmentSlot_2;
	FGameplayTag Input_EquipmentSlot_3;
	FGameplayTag Input_EquipmentSlot_LastEquipped;
	FGameplayTag Input_Pause;
	FGameplayTag Input_LeftClick;

	FGameplayTag Ability;
	FGameplayTag Ability_Fire;
	FGameplayTag Ability_KnifeAttack;
	FGameplayTag Ability_BackStab;
	FGameplayTag Ability_Inspect;
	FGameplayTag Ability_Track;
	FGameplayTag Ability_Jump;
	FGameplayTag Ability_Sprint;
	FGameplayTag Ability_Crouch;
	FGameplayTag Ability_Interact;
	FGameplayTag Ability_ShiftInteract;
	FGameplayTag Ability_InputBlocked;

	FGameplayTag State;
	FGameplayTag State_Crouching;
	FGameplayTag State_Inspecting;
	FGameplayTag State_KnifeAttacking;
	FGameplayTag State_BackStabbing;
	FGameplayTag State_Firing;
	FGameplayTag State_Jumping;
	FGameplayTag State_Moving;
	FGameplayTag State_Sprinting;
	FGameplayTag State_PlayingBSGameMode;

	FGameplayTag State_Weapon_AutomaticFire;
	FGameplayTag State_Weapon_ShowDecals;
	FGameplayTag State_Weapon_ShowTracers;
	FGameplayTag State_Weapon_ShowMuzzleFlash;
	FGameplayTag State_Weapon_ShowMesh;
	FGameplayTag State_Weapon_Recoil;

	FGameplayTag Target;
	FGameplayTag Target_State;
	FGameplayTag Target_State_PreGameModeStart;
	FGameplayTag Target_State_Immune;
	FGameplayTag Target_State_Immune_TrackingDamage;
	FGameplayTag Target_State_Immune_HitDamage;
	FGameplayTag Target_State_Grid;
	FGameplayTag Target_State_Single;
	FGameplayTag Target_State_Multi;
	FGameplayTag Target_State_Tracking;
	FGameplayTag Target_State_Charged;
	FGameplayTag Target_ResetHealth;
	FGameplayTag Target_TreatAsExternalDamage;

	FGameplayTag Cheat;
	FGameplayTag Cheat_AimBot;

	FGameplayTag GameModeCategory;
	FGameplayTag GameModeCategory_Spawning;
	FGameplayTag GameModeCategory_Activation;
	FGameplayTag GameModeCategory_Deactivation;
	FGameplayTag GameModeCategory_Destruction;
	FGameplayTag GameModeCategory_SpawnArea;
	FGameplayTag GameModeCategory_Scale;
	FGameplayTag GameModeCategory_Movement;
	FGameplayTag GameModeCategory_Position;
	FGameplayTag GameModeCategory_Velocity;
	FGameplayTag GameModeCategory_Direction;
	FGameplayTag GameModeCategory_Health;
	FGameplayTag GameModeCategory_Damage;
	FGameplayTag GameModeCategory_Time;
	FGameplayTag GameModeCategory_Dynamic;
	FGameplayTag GameModeCategory_Target;
	FGameplayTag GameModeCategory_AI;
	FGameplayTag GameModeCategory_Grid;
	FGameplayTag GameModeCategory_Effects;
	FGameplayTag GameModeCategory_Static;
	FGameplayTag GameModeCategory_MultiSelect;

protected:
	void AddAllTags(UGameplayTagsManager& Manager);
	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);

private:
	static FBSGameplayTags GameplayTags;
};
