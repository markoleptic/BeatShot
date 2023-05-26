// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "BSGameplayTags.h"
#include "GameplayTagsManager.h"

FBSGameplayTags FBSGameplayTags::GameplayTags;

void FBSGameplayTags::InitializeTags()
{
	UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
	GameplayTags.AddAllTags(GameplayTagsManager);
	GameplayTagsManager.DoneAddingNativeTags();
}

void FBSGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	AddTag(GameplayCue_Sprint, "GameplayCue.Sprint","");
	AddTag(GameplayCue_MuzzleFlash, "GameplayCue.MuzzleFlash","");
	AddTag(GameplayCue_FireGun_Impact, "GameplayCue.FireGun.Impact","");

	AddTag(Data, "Data","");
	AddTag(Data_Damage, "Data.Damage","");

	AddTag(Event, "Event","");
	AddTag(Event_Montage, "Event.Montage","");
	AddTag(Event_Montage_EndAbility, "Event.Montage.EndAbility","");
	AddTag(Event_Montage_SpawnProjectile, "Event.Montage.SpawnProjectile","");

	AddTag(Input, "Input","");
	AddTag(Input_Move, "Input.Move","");
	AddTag(Input_Move_Forward, "Input.Move.Forward","");
	AddTag(Input_Move_Backward, "Input.Move.Backward","");
	AddTag(Input_Move_Left, "Input.Move.Left","");
	AddTag(Input_Move_Right, "Input.Move.Right","");

	AddTag(Input_Look, "Input.Look","");
	AddTag(Input_Crouch, "Input.Crouch","");
	AddTag(Input_Fire, "Input.Fire","");
	AddTag(Input_Sprint, "Input.Sprint","");
	AddTag(Input_Walk, "Input.Walk","");
	AddTag(Input_Jump, "Input.Jump","");
	AddTag(Input_Interact, "Input.Interact","");
	AddTag(Input_ShiftInteract, "Input.ShiftInteract","");
	AddTag(Input_Inspect, "Input.Inspect","");
	AddTag(Input_KnifeAttack, "Input.KnifeAttack","");
	AddTag(Input_BackStab, "Input.BackStab","");
	AddTag(Input_Disabled, "Input.Disabled","");

	AddTag(Input_EquipmentSlot, "Input.EquipmentSlot","");
	AddTag(Input_EquipmentSlot_1, "Input.EquipmentSlot.1","");
	AddTag(Input_EquipmentSlot_2, "Input.EquipmentSlot.2","");
	AddTag(Input_EquipmentSlot_3, "Input.EquipmentSlot.3","");
	AddTag(Input_EquipmentSlot_LastEquipped, "Input.EquipmentSlot.LastEquipped","");
	
	AddTag(Ability, "Ability","");
	AddTag(Ability_Fire, "Ability.Fire","");
	AddTag(Ability_Inspect, "Ability.Inspect","");
	AddTag(Ability_KnifeAttack, "Ability.KnifeAttack","");
	AddTag(Ability_BackStab, "Ability.BackStab","");
	AddTag(Ability_Track, "Ability.Track","");
	AddTag(Ability_Jump, "Ability.Jump","");
	AddTag(Ability_Sprint, "Ability.Sprint","");
	AddTag(Ability_Crouch, "Ability.Crouch","");
	AddTag(Ability_Interact, "Ability.Interact","");
	AddTag(Ability_ShiftInteract, "Ability.ShiftInteract","");

	AddTag(State, "State","");
	AddTag(State_Crouching, "State.Crouching","");
	AddTag(State_Firing, "State.Firing","");
	AddTag(State_Inspecting, "State.Inspecting","");
	AddTag(State_KnifeAttacking, "State.KnifeAttacking","");
	AddTag(State_BackStabbing, "State.BackStabbing","");
	AddTag(State_Jumping, "State.Jumping","");
	AddTag(State_Moving, "State.Moving","");
	AddTag(State_Sprinting, "State.Sprinting","");
	AddTag(State_PlayingBSGameMode, "State.PlayingBSGameMode","");

	AddTag(State_Weapon_AutomaticFire, "State.Weapon.AutomaticFire","");
	AddTag(State_Weapon_ShowDecals, "State.Weapon.ShowDecals","");
	AddTag(State_Weapon_ShowTracers, "State.Weapon.ShowTracers","");
	AddTag(State_Weapon_Recoil, "State.Weapon.Recoil","");

	AddTag(Target, "Target","");
	AddTag(Target_State, "Target.State","");
	AddTag(Target_State_PreGameModeStart, "Target.State.PreGameModeStart","");
	AddTag(Target_State_Damageable, "Target.State.Damageable","");
	AddTag(Target_State_Immune, "Target.State.Immune","");
	AddTag(Target_State_Grid, "Target.State.Grid","");
	AddTag(Target_State_Single, "Target.State.Single","");
	AddTag(Target_State_Multi, "Target.State.Multi","");
	AddTag(Target_State_Tracking, "Target.State.Tracking","");
	AddTag(Target_State_Charged, "Target.State.Charged","");

	AddTag(Cheat, "Cheat","");
	AddTag(Cheat_AimBot, "Cheat.AimBot","");
}

void FBSGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}
