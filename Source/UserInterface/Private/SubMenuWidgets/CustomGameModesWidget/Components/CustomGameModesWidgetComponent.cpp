// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidgetComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "WidgetComponents/Boxes/BSComboBoxString.h"
#include "WidgetComponents/GameModeCategoryTagWidget.h"
#include "WidgetComponents/MenuOptionWidgets/CheckBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/EditableTextBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxOptionWidget.h"

void UCustomGameModesWidgetComponent::NativeConstruct()
{
	Super::NativeConstruct();
	
	WidgetTree->ForEachWidget([this] (UWidget* Widget)
	{
		if (UMenuOptionWidget* MenuOption = Cast<UMenuOptionWidget>(Widget))
		{
			if (MenuOption->ShouldShowTooltip() && !MenuOption->GetTooltipImageText().IsEmpty())
			SetupTooltip(MenuOption->GetTooltipImage(), MenuOption->GetTooltipImageText());
			MenuOptionWidgets.Add(MenuOption);

			AddGameModeCategoryTagWidgets(MenuOption);
			
			if (UComboBoxOptionWidget* ComboBoxOptionWidget = Cast<UComboBoxOptionWidget>(MenuOption))
			{
				ComboBoxOptionWidget->SetGameplayTagWidgetMap(GameplayTagWidgetMap);
				ComboBoxOptionWidget->SetEnumTagMap(EnumTagMap);
			}
		}
	});
}

void UCustomGameModesWidgetComponent::NativeDestruct()
{
	Super::NativeDestruct();
	BSConfig = nullptr;
}

void UCustomGameModesWidgetComponent::UpdateAllOptionsValid()
{
	if (!UpdateWarningTooltips())
	{
		RequestComponentUpdate.Broadcast();
	}
}

void UCustomGameModesWidgetComponent::InitComponent(FBSConfig* InConfigPtr, const TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	BSConfig = InConfigPtr;
	SetNext(InNext);
	UpdateOptionsFromConfig();
	bIsInitialized = true;
}

void UCustomGameModesWidgetComponent::UpdateOptionsFromConfig()
{
}

TObjectPtr<UCustomGameModesWidgetComponent> UCustomGameModesWidgetComponent::GetNext() const
{
	return Next;
}

void UCustomGameModesWidgetComponent::SetNext(const TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	Next = InNext;
}

void UCustomGameModesWidgetComponent::AddGameModeCategoryTagWidgets(UMenuOptionWidget* MenuOptionWidget)
{
	FGameplayTagContainer Container;
	MenuOptionWidget->GetGameModeCategoryTags(Container);
	TArray<UGameModeCategoryTagWidget*> GameModeCategoryTagWidgets;
			
	for (const FGameplayTag& Tag : Container)
	{
		const TSubclassOf<UGameModeCategoryTagWidget>* SubClass = GameplayTagWidgetMap.Find(Tag);
		if (!SubClass)
		{
			continue;
		}
		UGameModeCategoryTagWidget* TagWidget = CreateWidget<UGameModeCategoryTagWidget>(this, *SubClass);
		GameModeCategoryTagWidgets.Add(TagWidget);
	}
	if (GameModeCategoryTagWidgets.Num() > 0)
	{
		MenuOptionWidget->AddGameModeCategoryTagWidgets(GameModeCategoryTagWidgets);
	}
}

bool UCustomGameModesWidgetComponent::UpdateValueIfDifferent(const USliderTextBoxOptionWidget* Widget, const float Value)
{
	if (!FMath::IsNearlyEqual(Widget->GetSliderValue(), Value) || !FMath::IsNearlyEqual(Widget->GetEditableTextBoxValue(), Value))
	{
		Widget->SetValue(Value);
		return true;
	}
	return false;
}

bool UCustomGameModesWidgetComponent::UpdateValueIfDifferent(const UComboBoxOptionWidget* Widget, const FString& NewOption)
{
	if (NewOption.IsEmpty())
	{
		if (Widget->ComboBox->GetSelectedOptionCount() == 0)
		{
			return false;
		}
		Widget->ComboBox->ClearSelection();
		return true;
	}
	if (Widget->ComboBox->GetSelectedOption() == NewOption)
	{
		return false;
	}
	Widget->ComboBox->SetSelectedOption(NewOption);
	return true;
}

bool UCustomGameModesWidgetComponent::UpdateValueIfDifferent(const UComboBoxOptionWidget* Widget, const TArray<FString>& NewOptions)
{
	const TArray<FString> SelectedOptions = Widget->ComboBox->GetSelectedOptions();
	
	if (NewOptions.IsEmpty())
	{
		if (SelectedOptions.IsEmpty())
		{
			return false;
		}
		Widget->ComboBox->ClearSelection();
		return true;
	}
	
	if (SelectedOptions == NewOptions && SelectedOptions.Num() == NewOptions.Num())
	{
		return false;
	}
	
	Widget->ComboBox->SetSelectedOptions(NewOptions);
	return true;
}

bool UCustomGameModesWidgetComponent::UpdateValueIfDifferent(const UCheckBoxOptionWidget* Widget, const bool bIsChecked)
{
	if (Widget->CheckBox->IsChecked() == bIsChecked)
	{
		return false;
	}
	Widget->CheckBox->SetIsChecked(bIsChecked);
	return true;
}

bool UCustomGameModesWidgetComponent::UpdateValueIfDifferent(const UEditableTextBoxOptionWidget* Widget, const FText& NewText)
{
	if (Widget->EditableTextBox->GetText().EqualTo(NewText))
	{
		return false;
	}
	Widget->EditableTextBox->SetText(NewText);
	return true;
}

bool UCustomGameModesWidgetComponent::UpdateWarningTooltips()
{
	bool bAllClean = true;
	for (const TObjectPtr<UMenuOptionWidget> Widget : MenuOptionWidgets)
	{
		Widget->UpdateAllWarningTooltips();
		TArray<FTooltipData>& TooltipData = Widget->GetTooltipWarningData();
		for (FTooltipData& Data : TooltipData)
		{
			if (Data.IsDirty())
			{
				bAllClean = false;
				if (Data.ShouldShowTooltipImage())
				{
					Widget->ConstructTooltipWarningImageIfNeeded(Data);
					SetupTooltip(Data);
				}
				else
				{
					Data.RemoveTooltipImage();
				}
				Data.SetIsClean(true);
			}
		}
	}
	UpdateCustomGameModeCategoryInfo();
	return bAllClean;
}

void UCustomGameModesWidgetComponent::UpdateCustomGameModeCategoryInfo()
{
	int32 NumWarnings = 0;
	int32 NumCautions = 0;
	for (const TObjectPtr<UMenuOptionWidget> Widget : MenuOptionWidgets)
	{
		NumWarnings += Widget->GetNumberOfWarnings();
		NumCautions += Widget->GetNumberOfCautions();
	}
	CustomGameModeCategoryInfo.Update(NumCautions, NumWarnings);
}

float UCustomGameModesWidgetComponent::GetHorizontalSpread() const
{
	return MaxValue_HorizontalSpread + BSConfig->TargetConfig.MaxSpawnedTargetScale * SphereTargetDiameter;
}

float UCustomGameModesWidgetComponent::GetVerticalSpread() const
{
	return MaxValue_VerticalSpread + BSConfig->TargetConfig.MaxSpawnedTargetScale * SphereTargetDiameter;
}

float UCustomGameModesWidgetComponent::GetMinRequiredHorizontalSpread() const
{
	const float TotalTargetWidth = GetMaxTargetDiameter() * BSConfig->GridConfig.NumHorizontalGridTargets;
	return BSConfig->GridConfig.GridSpacing.X * (BSConfig->GridConfig.NumHorizontalGridTargets - 1) + TotalTargetWidth;
}

float UCustomGameModesWidgetComponent::GetMinRequiredVerticalSpread() const
{
	const float TotalTargetHeight = GetMaxTargetDiameter() * BSConfig->GridConfig.NumVerticalGridTargets;
	return BSConfig->GridConfig.GridSpacing.Y * (BSConfig->GridConfig.NumVerticalGridTargets - 1) + TotalTargetHeight;
}

float UCustomGameModesWidgetComponent::GetMaxTargetDiameter() const
{
	return BSConfig->TargetConfig.MaxSpawnedTargetScale * SphereTargetDiameter;
}

int32 UCustomGameModesWidgetComponent::GetMaxAllowedNumHorizontalTargets() const
{
	// HorizontalSpread = MaxTargetSize * NumHorizontalTargets + HorizontalSpacing * (NumHorizontalTargets - 1)
	return (GetHorizontalSpread() + BSConfig->GridConfig.GridSpacing.X) / (GetMaxTargetDiameter() + BSConfig->GridConfig.GridSpacing.X);
}

int32 UCustomGameModesWidgetComponent::GetMaxAllowedNumVerticalTargets() const
{
	return (GetVerticalSpread() + BSConfig->GridConfig.GridSpacing.Y) / (GetMaxTargetDiameter() + BSConfig->GridConfig.GridSpacing.Y);
}

float UCustomGameModesWidgetComponent::GetMaxAllowedHorizontalSpacing() const
{
	const float TotalTargetWidth = GetMaxTargetDiameter() * BSConfig->GridConfig.NumHorizontalGridTargets;
	return (GetHorizontalSpread() - TotalTargetWidth) / (BSConfig->GridConfig.NumHorizontalGridTargets - 1);
}

float UCustomGameModesWidgetComponent::GetMaxAllowedVerticalSpacing() const
{
	const float TotalTargetHeight = GetMaxTargetDiameter() * BSConfig->GridConfig.NumVerticalGridTargets;
	return (GetVerticalSpread() - TotalTargetHeight) / (BSConfig->GridConfig.NumVerticalGridTargets - 1);
}

float UCustomGameModesWidgetComponent::GetMaxAllowedTargetScale() const
{
	const float MaxAllowedHorizontal = (MaxValue_HorizontalSpread + SphereTargetDiameter - BSConfig->GridConfig.GridSpacing.X * BSConfig->GridConfig.NumHorizontalGridTargets + BSConfig->GridConfig.GridSpacing.X) /
		(SphereTargetDiameter * BSConfig->GridConfig.NumHorizontalGridTargets);
	
	const float MaxAllowedVertical = (MaxValue_HorizontalSpread + SphereTargetDiameter - BSConfig->GridConfig.GridSpacing.Y * BSConfig->GridConfig.NumVerticalGridTargets + BSConfig->GridConfig.GridSpacing.Y) /
		(SphereTargetDiameter * BSConfig->GridConfig.NumVerticalGridTargets);
	
	
	if (MaxAllowedVertical < MaxAllowedHorizontal)
	{
		return MaxAllowedVertical;
	}
	return MaxAllowedHorizontal;
}





