// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/CustomGameModesWidget.h"
#include "SubMenuWidgets/CustomGameModesWidget/CustomGameModesWidget_SpawnArea.h"
#include "SubMenuWidgets/CustomGameModesWidget/CustomGameModesWidget_Start.h"
#include "WidgetComponents/Buttons/BSButton.h"

void UCustomGameModesWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CurrentConfig = FBSConfig();
	CurrentConfigPtr = &CurrentConfig;

	Button_Next->OnBSButtonPressed.AddUniqueDynamic(this, &ThisClass::OnBSButtonPressed);
	Button_Previous->OnBSButtonPressed.AddUniqueDynamic(this, &ThisClass::OnBSButtonPressed);
	Button_Create->OnBSButtonPressed.AddUniqueDynamic(this, &ThisClass::OnBSButtonPressed);

	Button_Next->SetIsEnabled(false);
	Button_Previous->SetIsEnabled(false);
	Button_Create->SetIsEnabled(false);
	
	Widget_Start->Init(CurrentConfigPtr, Widget_SpawnArea);
	Widget_Start->OnCanTransitionForwardStateChanged.AddUObject(this, &ThisClass::OnCanTransitionForwardStateChanged);
	TransitionMap.Add(Widget_Start, false);

	Widget_SpawnArea->Init(CurrentConfigPtr, nullptr);
	Widget_SpawnArea->OnCanTransitionForwardStateChanged.AddUObject(this, &ThisClass::OnCanTransitionForwardStateChanged);
	TransitionMap.Add(Widget_SpawnArea, false);
	
	CurrentWidget = Widget_Start;
}

void UCustomGameModesWidget::OnBSButtonPressed(const UBSButton* BSButton)
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

void UCustomGameModesWidget::TransitionForward()
{
	if (TransitionMap.FindChecked(CurrentWidget) == false)
	{
		return;
	}
	/** TODO: Last widget don't allow to transition forward */
	if (CurrentWidget && CurrentWidget->GetNext())
	{
		CurrentWidget->PlayAnim_TransitionInLeft_Reverse();
		CurrentWidget->GetNext()->PlayAnim_TransitionInRight_Forward();
		CurrentWidget = CurrentWidget->GetNext();
	}
}

void UCustomGameModesWidget::TransitionBackward()
{
	if (CurrentWidget == Widget_Start)
	{
		return;
	}
	if (CurrentWidget && CurrentWidget->GetNext())
	{
		TObjectPtr<UCustomGameModesWidgetComponent> Previous = nullptr;
		TObjectPtr<UCustomGameModesWidgetComponent> Current = CurrentWidget->GetNext();
		
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
			CurrentWidget->PlayAnim_TransitionInRight_Reverse();
			Previous->GetNext()->PlayAnim_TransitionInLeft_Forward();
			CurrentWidget = Previous;
		}
	}
}

void UCustomGameModesWidget::OnCanTransitionForwardStateChanged(
	const TObjectPtr<UCustomGameModesWidgetComponent> Widget, const bool bCanTransition)
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
