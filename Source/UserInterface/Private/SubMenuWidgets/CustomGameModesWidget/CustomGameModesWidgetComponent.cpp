// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/CustomGameModesWidgetComponent.h"

void UCustomGameModesWidgetComponent::Init(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	ConfigPtr = InConfigPtr;
	SetNext(InNext);
}

void UCustomGameModesWidgetComponent::UpdateOptions()
{
}

void UCustomGameModesWidgetComponent::PlayAnim_TransitionInLeft_Forward()
{
	PlayAnimationForward(TransitionInLeft);
}

void UCustomGameModesWidgetComponent::PlayAnim_TransitionInLeft_Reverse()
{
	PlayAnimationReverse(TransitionInLeft);
}

void UCustomGameModesWidgetComponent::PlayAnim_TransitionInRight_Forward()
{
	PlayAnimationForward(TransitionInRight);
}

void UCustomGameModesWidgetComponent::PlayAnim_TransitionInRight_Reverse()
{
	PlayAnimationReverse(TransitionInRight);
}

void UCustomGameModesWidgetComponent::SetCanTransitionForward(const bool bInCanTransitionForward)
{
	if (bInCanTransitionForward == bCanTransitionForward)
	{
		return;
	}
	bCanTransitionForward = bInCanTransitionForward;
	if (OnCanTransitionForwardStateChanged.IsBound())
	{
		OnCanTransitionForwardStateChanged.Broadcast(this, bCanTransitionForward);
	}
}

TObjectPtr<UCustomGameModesWidgetComponent> UCustomGameModesWidgetComponent::GetNext() const
{
	return Next;
}

void UCustomGameModesWidgetComponent::SetNext(const TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	Next = InNext;
}

