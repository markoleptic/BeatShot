﻿// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/InputMappingWidget.h"
#include "Components/InputKeySelector.h"
#include "Components/TextBlock.h"
#include "WidgetComponents/Buttons/BSButton.h"

void UInputMappingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	InputKeySelectorSlot1->OnIsSelectingKeyChanged.AddDynamic(this, &ThisClass::OnIsSelectingKeyChanged_Slot1);
	InputKeySelectorSlot2->OnIsSelectingKeyChanged.AddDynamic(this, &ThisClass::OnIsSelectingKeyChanged_Slot2);

	InputKeySelectorSlot1->OnKeySelected.AddDynamic(this, &ThisClass::OnKeySelected_Slot1);
	InputKeySelectorSlot2->OnKeySelected.AddDynamic(this, &ThisClass::OnKeySelected_Slot2);

	ActionKeyMappings = TArray<FEnhancedActionKeyMapping>();
	ActionKeyMappings.Init(FEnhancedActionKeyMapping(), 2);
}

void UInputMappingWidget::Init(const TArray<FEnhancedActionKeyMapping>& Mappings)
{
	ActionKeyMappings = Mappings;
	TextBlock_KeyDescription->SetText(ActionKeyMappings[0].PlayerMappableOptions.DisplayName);
	InputKeySelectorSlot1->SetSelectedKey(Mappings[0].Key);
	InputKeySelectorSlot2->SetSelectedKey(Mappings[1].Key);
}

FName UInputMappingWidget::GetMappingNameForKey(const FKey InKey)
{
	if (InKey == FKey())
	{
		return NAME_None;
	}
	if (InputKeySelectorSlot1->GetSelectedKey().Key == InKey)
	{
		return ActionKeyMappings[0].GetMappingName();
	}
	if (InputKeySelectorSlot2->GetSelectedKey().Key == InKey)
	{
		return ActionKeyMappings[1].GetMappingName();
	}
	return NAME_None;
}

void UInputMappingWidget::SetKey(const FName MappingName, const FKey NewKey)
{
	if (ActionKeyMappings[0].GetMappingName() == MappingName)
	{
		InputKeySelectorSlot1->SetSelectedKey(NewKey);
	}
	else if (ActionKeyMappings[1].GetMappingName() == MappingName)
	{
		InputKeySelectorSlot2->SetSelectedKey(NewKey);
	}
}

void UInputMappingWidget::OnIsSelectingKeyChanged_Slot1()
{
	OnIsSelectingKey.Broadcast(InputKeySelectorSlot1);
}

void UInputMappingWidget::OnIsSelectingKeyChanged_Slot2()
{
	OnIsSelectingKey.Broadcast(InputKeySelectorSlot2);
}

void UInputMappingWidget::OnKeySelected_Slot1(FInputChord SelectedKey)
{
	ActionKeyMappings[0].Key = SelectedKey.Key;
	OnKeySelected.Broadcast(ActionKeyMappings[0].GetMappingName(), SelectedKey);
}

void UInputMappingWidget::OnKeySelected_Slot2(FInputChord SelectedKey)
{
	ActionKeyMappings[1].Key = SelectedKey.Key;
	OnKeySelected.Broadcast(ActionKeyMappings[1].GetMappingName(), SelectedKey);
}
