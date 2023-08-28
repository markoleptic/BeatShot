// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/CustomGameModesWidget_CreatorView.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Activation.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Deactivation.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_General.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Preview.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_SpawnArea.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Spawning.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Start.h"
#include "WidgetComponents/Buttons/BSButton.h"

void UCustomGameModesWidget_CreatorView::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Next->OnBSButtonPressed.AddUniqueDynamic(this, &ThisClass::OnBSButtonPressed);
	Button_Previous->OnBSButtonPressed.AddUniqueDynamic(this, &ThisClass::OnBSButtonPressed);
	Button_RefreshPreview->OnBSButtonPressed.AddUniqueDynamic(this, &ThisClass::OnBSButtonPressed);
	// GameModesWidget binds to Button_Create

	Button_Next->SetIsEnabled(true);
	Button_Previous->SetIsEnabled(false);
	Button_Create->SetIsEnabled(false);
	Button_RefreshPreview->SetIsEnabled(false);

	CurrentWidget = Widget_Start;
	FirstWidget = Widget_Start;
	LastWidget = Widget_Deactivation;

	AddChildWidget(Widget_Preview);
}

void UCustomGameModesWidget_CreatorView::Init(FBSConfig* InConfig, const TObjectPtr<UBSGameModeDataAsset> InGameModeDataAsset)
{
	// Doesn't call parent function so it can set correct Next widget values in InitComponent
	
	BSConfig = InConfig;
	GameModeDataAsset = InGameModeDataAsset;
	
	Widget_Preview->InitComponent(BSConfig, nullptr);
	
	Widget_Start->InitComponent(BSConfig, Widget_General);
	Widget_General->InitComponent(BSConfig, Widget_SpawnArea);
	Widget_SpawnArea->InitComponent(BSConfig, Widget_Spawning);
	Widget_Spawning->InitComponent(BSConfig, Widget_Activation);
	Widget_Activation->InitComponent(BSConfig, Widget_Deactivation);
	Widget_Deactivation->InitComponent(BSConfig, Widget_Start);
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
}

void UCustomGameModesWidget_CreatorView::TransitionForward()
{
	if (CurrentWidget && CurrentWidget->GetNext())
	{
		UpdateOptionsFromConfig();
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
			UpdateOptionsFromConfig();
			CurrentWidget->PlayAnim_TransitionInRight_Reverse(true);
			Previous->PlayAnim_TransitionInLeft_Forward(false);
			ChangeCurrentWidget(Previous);
		}
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
		Button_Next->SetIsEnabled(true);
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

void UCustomGameModesWidget_CreatorView::OnValidOptionsStateChanged(const TObjectPtr<UCustomGameModesWidgetComponent> Widget, const bool bAllOptionsValid)
{
	Super::OnValidOptionsStateChanged(Widget, bAllOptionsValid);
}
