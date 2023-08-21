// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidgetComponent.h"

void UCustomGameModesWidgetComponent::NativeConstruct()
{
	Super::NativeConstruct();

	OnTransitionInLeftFinish.BindDynamic(this, &ThisClass::SetCollapsed);
	OnTransitionInRightFinish.BindDynamic(this, &ThisClass::SetCollapsed);
}

bool UCustomGameModesWidgetComponent::UpdateAllOptionsValid()
{
	return false;
}

void UCustomGameModesWidgetComponent::SetAllOptionsValid(const bool bUpdateAllOptionsValid)
{
	if (bUpdateAllOptionsValid == bAllOptionsValid)
	{
		return;
	}
	bAllOptionsValid = bUpdateAllOptionsValid;
	
	if (OnValidOptionsStateChanged.IsBound())
	{
		OnValidOptionsStateChanged.Broadcast(this, bAllOptionsValid);
	}
}

void UCustomGameModesWidgetComponent::InitComponent(FBSConfig* InConfigPtr, const TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	ConfigPtr = InConfigPtr;
	SetNext(InNext);
	UpdateOptions();
}

void UCustomGameModesWidgetComponent::UpdateOptions()
{
}

bool UCustomGameModesWidgetComponent::GetAllOptionsValid() const
{
	return bAllOptionsValid;
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

