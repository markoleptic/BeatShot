// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidgetComponent.h"

void UCustomGameModesWidgetComponent::NativeConstruct()
{
	Super::NativeConstruct();

	OnTransitionInLeftFinish.BindDynamic(this, &ThisClass::SetCollapsed);
	OnTransitionInRightFinish.BindDynamic(this, &ThisClass::SetCollapsed);
}

void UCustomGameModesWidgetComponent::InitComponent(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	ConfigPtr = InConfigPtr;
	SetNext(InNext);
	UpdateOptions();
}

void UCustomGameModesWidgetComponent::UpdateOptions()
{
}

void UCustomGameModesWidgetComponent::PlayAnim_TransitionInLeft_Forward(const bool bCollapseOnFinish)
{
	if (bCollapseOnFinish)
	{
		BindToAnimationFinished(TransitionInLeft, OnTransitionInLeftFinish);
	}
	if (GetVisibility() == ESlateVisibility::Collapsed)
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	PlayAnimationForward(TransitionInLeft);
}

void UCustomGameModesWidgetComponent::PlayAnim_TransitionInLeft_Reverse(const bool bCollapseOnFinish)
{
	if (bCollapseOnFinish)
	{
		BindToAnimationFinished(TransitionInLeft, OnTransitionInLeftFinish);
	}
	if (GetVisibility() == ESlateVisibility::Collapsed)
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	PlayAnimationReverse(TransitionInLeft);
}

void UCustomGameModesWidgetComponent::PlayAnim_TransitionInRight_Forward(const bool bCollapseOnFinish)
{
	if (bCollapseOnFinish)
	{
		BindToAnimationFinished(TransitionInRight, OnTransitionInRightFinish);
	}
	if (GetVisibility() == ESlateVisibility::Collapsed)
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	PlayAnimationForward(TransitionInRight);
}

void UCustomGameModesWidgetComponent::PlayAnim_TransitionInRight_Reverse(const bool bCollapseOnFinish)
{
	if (bCollapseOnFinish)
	{
		BindToAnimationFinished(TransitionInRight, OnTransitionInRightFinish);
	}
	if (GetVisibility() == ESlateVisibility::Collapsed)
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
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

void UCustomGameModesWidgetComponent::SetCollapsed()
{
	SetVisibility(ESlateVisibility::Collapsed);
	UnbindFromAnimationFinished(TransitionInLeft, OnTransitionInLeftFinish);
	UnbindFromAnimationFinished(TransitionInRight, OnTransitionInRightFinish);
}

