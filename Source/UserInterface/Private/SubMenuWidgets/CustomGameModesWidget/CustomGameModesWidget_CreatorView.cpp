// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/CustomGameModesWidget_CreatorView.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Activation.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Deactivation.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_General.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Preview.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_SpawnArea.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Spawning.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Start.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Target.h"
#include "WidgetComponents/Buttons/BSButton.h"

void UCustomGameModesWidget_CreatorView::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Next->OnBSButtonPressed.AddUniqueDynamic(this, &ThisClass::OnBSButtonPressed);
	Button_Previous->OnBSButtonPressed.AddUniqueDynamic(this, &ThisClass::OnBSButtonPressed);
	Button_Create->OnBSButtonPressed.AddUniqueDynamic(this, &ThisClass::OnBSButtonPressed);

	Button_Next->SetIsEnabled(false);
	Button_Previous->SetIsEnabled(false);
	Button_Create->SetIsEnabled(false);

	Widget_Start->OnCanTransitionForwardStateChanged.AddUObject(this, &ThisClass::OnCanTransitionForwardStateChanged);
	Widget_SpawnArea->OnCanTransitionForwardStateChanged.AddUObject(this, &ThisClass::OnCanTransitionForwardStateChanged);
	Widget_Spawning->OnCanTransitionForwardStateChanged.AddUObject(this, &ThisClass::OnCanTransitionForwardStateChanged);
	Widget_Activation->OnCanTransitionForwardStateChanged.AddUObject(this, &ThisClass::OnCanTransitionForwardStateChanged);
	Widget_Deactivation->OnCanTransitionForwardStateChanged.AddUObject(this, &ThisClass::OnCanTransitionForwardStateChanged);
	Widget_General->OnCanTransitionForwardStateChanged.AddUObject(this, &ThisClass::OnCanTransitionForwardStateChanged);
	Widget_Target->OnCanTransitionForwardStateChanged.AddUObject(this, &ThisClass::OnCanTransitionForwardStateChanged);

	TransitionMap.Add(Widget_Start, false);
	TransitionMap.Add(Widget_SpawnArea, false);
	TransitionMap.Add(Widget_Spawning, false);
	TransitionMap.Add(Widget_Activation, false);
	TransitionMap.Add(Widget_Deactivation, false);
	TransitionMap.Add(Widget_General, false);
	TransitionMap.Add(Widget_Target, false);

	CurrentWidget = Widget_Start;
	FirstWidget = Widget_Start;
	LastWidget = Widget_Target;

	AddComponent(Widget_Preview);
}

void UCustomGameModesWidget_CreatorView::OnBSButtonPressed(const UBSButton* BSButton)
{
	if (BSButton == Button_Next)
	{
		TransitionForward();
	}
	else if (BSButton == Button_Previous)
	{
		TransitionBackward();
	}
	else if (BSButton == Button_Create)
	{
		
	}
}

void UCustomGameModesWidget_CreatorView::Init(FBSConfig* InConfig, const TObjectPtr<UBSGameModeDataAsset> InGameModeDataAsset)
{
	CurrentConfigPtr = InConfig;
	GameModeDataAsset = InGameModeDataAsset;
	
	Widget_Start->InitComponent(CurrentConfigPtr, Widget_SpawnArea);
	Widget_SpawnArea->InitComponent(CurrentConfigPtr, Widget_Spawning);
	Widget_Spawning->InitComponent(CurrentConfigPtr, Widget_Activation);
	Widget_Activation->InitComponent(CurrentConfigPtr, Widget_Deactivation);
	Widget_Deactivation->InitComponent(CurrentConfigPtr, Widget_General);
	Widget_General->InitComponent(CurrentConfigPtr, Widget_Target);
	Widget_Target->InitComponent(CurrentConfigPtr, Widget_Start);
	Widget_Preview->InitComponent(CurrentConfigPtr, nullptr);
}

void UCustomGameModesWidget_CreatorView::TransitionForward()
{
	if (TransitionMap.FindChecked(CurrentWidget) == false)
	{
		return;
	}

	if (CurrentWidget && CurrentWidget->GetNext())
	{
		Update();
		CurrentWidget->PlayAnim_TransitionInLeft_Reverse(true);
		CurrentWidget->GetNext()->PlayAnim_TransitionInRight_Forward(false);
		ChangeCurrentWidget(CurrentWidget->GetNext());
	}
}

void UCustomGameModesWidget_CreatorView::TransitionBackward()
{
	if (CurrentWidget == FirstWidget)
	{
		return;
	}
	
	if (CurrentWidget && CurrentWidget->GetNext())
	{
		TObjectPtr<UCustomGameModesWidgetComponent> Previous = nullptr;
		TObjectPtr<UCustomGameModesWidgetComponent> Current = CurrentWidget->GetNext();

		// Find the previous widget in linked list
		while (Previous == nullptr)
		{
			if (Current->GetNext() == CurrentWidget)
			{
				Previous = Current;
				break;
			}
			Current = Current->GetNext();
		}
		
		if (Previous)
		{
			Update();
			CurrentWidget->PlayAnim_TransitionInRight_Reverse(true);
			Previous->PlayAnim_TransitionInLeft_Forward(false);
			ChangeCurrentWidget(Previous);
		}
	}
}

void UCustomGameModesWidget_CreatorView::OnCanTransitionForwardStateChanged(const TObjectPtr<UCustomGameModesWidgetComponent> Widget, const bool bCanTransition)
{
	TransitionMap.FindChecked(Widget) = bCanTransition;
	
	if (TransitionMap.FindChecked(CurrentWidget) == true)
	{
		Button_Next->SetIsEnabled(true);
	}
	else if (TransitionMap.FindChecked(CurrentWidget) == false)
	{
		Button_Next->SetIsEnabled(false);
	}
}

void UCustomGameModesWidget_CreatorView::ChangeCurrentWidget(const TObjectPtr<UCustomGameModesWidgetComponent> Widget)
{
	CurrentWidget = Widget;

	// Disable next button if CurrentWidget is LastWidget
	if (CurrentWidget == LastWidget)
	{
		Button_Next->SetIsEnabled(false);
	}
	else
	{
		Button_Next->SetIsEnabled(CurrentWidget->CanTransitionForward());
	}

	// Disable previous button if CurrentWidget is FirstWidget
	if (CurrentWidget == FirstWidget)
	{
		Button_Previous->SetIsEnabled(false);
	}
	else
	{
		Button_Previous->SetIsEnabled(true);
	}
}
