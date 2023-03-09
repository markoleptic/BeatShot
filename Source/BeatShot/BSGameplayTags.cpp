// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "BSGameplayTags.h"
#include "GameplayTagsManager.h"

FBSGameplayTags FBSGameplayTags::GameplayTags;

void FBSGameplayTags::InitializeTags()
{
	GameplayTags.AddAllTags();
}

void FBSGameplayTags::AddAllTags()
{
	AddTag(GameplayCue_Sprint, "GameplayCue.Sprint");
	
	AddTag(Event, "Event");
	AddTag(Event_Montage, "Event.Montage");
	AddTag(Event_Montage_SpawnProjectile, "Event.Montage.SpawnProjectile");

	AddTag(Input, "Input");
	AddTag(Input_Move, "Input.Move");
	AddTag(Input_Look, "Input.Look");
	AddTag(Input_Crouch, "Input.Crouch");
	AddTag(Input_Fire, "Input.Fire");
	AddTag(Input_Sprint, "Input.Sprint");
	AddTag(Input_Jump, "Input.Jump");
	
	AddTag(State, "State");
	AddTag(State_Crouching, "State.Crouching");
	AddTag(State_Firing, "State.Firing");
	AddTag(State_Jumping, "State.Jumping");
	AddTag(State_Moving, "State.Moving");
	AddTag(State_Sprinting, "State.Sprinting");
	AddTag(State_PlayingBSGameMode, "State.PlayingBSGameMode");
	
	AddTag(Target, "Target");
	AddTag(Target_State, "Target.State");
	AddTag(Target_State_Grid, "Target.State.Grid");
	AddTag(Target_State_Single, "Target.State.Single");
	AddTag(Target_State_Multi, "Target.State.Multi");
	AddTag(Target_State_Tracking, "Target.State.Tracking");
}

void FBSGameplayTags::AddTag(FGameplayTag& OutTag, const FString& TagName)
{
	OutTag = FGameplayTag::RequestGameplayTag(FName(TagName));
}