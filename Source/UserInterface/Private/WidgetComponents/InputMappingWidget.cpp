// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/InputMappingWidget.h"
#include "Components/InputKeySelector.h"
#include "Components/TextBlock.h"
#include "WidgetComponents/BSButton.h"

void UInputMappingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	InputKeySelectorSlot1->OnIsSelectingKeyChanged.AddDynamic(this, &ThisClass::OnIsSelectingKeyChanged_Slot1);
	InputKeySelectorSlot2->OnIsSelectingKeyChanged.AddDynamic(this, &ThisClass::OnIsSelectingKeyChanged_Slot2);

	InputKeySelectorSlot1->OnKeySelected.AddDynamic(this, &ThisClass::OnKeySelected_Slot1);
	InputKeySelectorSlot2->OnKeySelected.AddDynamic(this, &ThisClass::OnKeySelected_Slot2);
}

void UInputMappingWidget::Init(const TArray<FEnhancedActionKeyMapping>& Mappings)
{
	ActionKeyMappings = Mappings;
	TextBlock_KeyDescription->SetText(ActionKeyMappings[0].PlayerMappableOptions.DisplayName);
	if (Mappings.Num() == 2)
	{
		InputKeySelectorSlot1->SetSelectedKey(Mappings[0].Key);

		InputKeySelectorSlot2->SetSelectedKey(Mappings[1].Key);
	}
	else
	{
		InputKeySelectorSlot1->SetSelectedKey(Mappings[0].Key);
	}
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
	if (ActionKeyMappings.Num() > 1 && InputKeySelectorSlot2->GetSelectedKey().Key == InKey)
	{
		return ActionKeyMappings[1].GetMappingName();
	}
	return NAME_None;
}

FName UInputMappingWidget::SetKey(const FKey OldKey, const FKey NewKey)
{
	if (InputKeySelectorSlot1->GetSelectedKey().Key == OldKey)
	{
		InputKeySelectorSlot1->SetSelectedKey(NewKey);
		return ActionKeyMappings[0].GetMappingName();
	}
	if (ActionKeyMappings.Num() > 1 && InputKeySelectorSlot2->GetSelectedKey().Key == OldKey)
	{
		InputKeySelectorSlot2->SetSelectedKey(NewKey);
		return ActionKeyMappings[1].GetMappingName();
	}
	return NAME_None;
}

void UInputMappingWidget::OnIsSelectingKeyChanged_Slot1()
{
}

void UInputMappingWidget::OnIsSelectingKeyChanged_Slot2()
{
}

void UInputMappingWidget::OnKeySelected_Slot1(FInputChord SelectedKey)
{
	OnKeySelected.Broadcast(ActionKeyMappings[0].GetMappingName(), SelectedKey);
}

void UInputMappingWidget::OnKeySelected_Slot2(FInputChord SelectedKey)
{
	if (ActionKeyMappings.Num() > 1)
	{
		OnKeySelected.Broadcast(ActionKeyMappings[1].GetMappingName(), SelectedKey);
	}
}
