// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/GameModesWidgets/Components/CustomGameModesWidgetComponent.h"
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

	WidgetTree->ForEachWidget([this](UWidget* Widget)
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

void UCustomGameModesWidgetComponent::InitComponent(FBSConfig* InConfigPtr, const int32 InIndex)
{
	BSConfig = InConfigPtr;
	UpdateOptionsFromConfig();
	bIsInitialized = true;
	Index = InIndex;
}

void UCustomGameModesWidgetComponent::UpdateOptionsFromConfig()
{
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

bool UCustomGameModesWidgetComponent::UpdateValueIfDifferent(const USliderTextBoxOptionWidget* Widget,
	const float Value)
{
	if (!FMath::IsNearlyEqual(Widget->GetSliderValue(), Value) || !FMath::IsNearlyEqual(
		Widget->GetEditableTextBoxValue(), Value))
	{
		Widget->SetValue(Value);
		return true;
	}
	return false;
}

bool UCustomGameModesWidgetComponent::UpdateValueIfDifferent(const UComboBoxOptionWidget* Widget,
	const FString& NewOption)
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

bool UCustomGameModesWidgetComponent::UpdateValueIfDifferent(const UComboBoxOptionWidget* Widget,
	const TArray<FString>& NewOptions)
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

bool UCustomGameModesWidgetComponent::UpdateValueIfDifferent(const UEditableTextBoxOptionWidget* Widget,
	const FText& NewText)
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

float UCustomGameModesWidgetComponent::GetMinRequiredHorizontalSpread() const
{
	return (BSConfig->GridConfig.GridSpacing.X + GetMaxTargetDiameter()) * (BSConfig->GridConfig.
		NumHorizontalGridTargets - 1);
}

float UCustomGameModesWidgetComponent::GetMinRequiredVerticalSpread() const
{
	return (BSConfig->GridConfig.GridSpacing.Y + GetMaxTargetDiameter()) * (BSConfig->GridConfig.NumVerticalGridTargets
		- 1);
}

float UCustomGameModesWidgetComponent::GetMaxTargetDiameter() const
{
	return FMath::Max(BSConfig->TargetConfig.MinSpawnedTargetScale, BSConfig->TargetConfig.MaxSpawnedTargetScale) *
		SphereTargetDiameter;
}

int32 UCustomGameModesWidgetComponent::GetMaxAllowedNumHorizontalTargets() const
{
	// Total = GridSpacing.X * (NumHorizontalGridTargets - 1) + (NumHorizontalGridTargets - 1) * MaxTargetDiameter;
	// Total = (NumHorizontalGridTargets - 1) * (GridSpacing.X + MaxTargetDiameter);
	// Total / (GridSpacing.X + MaxTargetDiameter) = NumHorizontalGridTargets - 1
	// NumHorizontalGridTargets = Total / (GridSpacing.X + MaxTargetDiameter) + 1
	return MaxValue_HorizontalSpread / (BSConfig->GridConfig.GridSpacing.X + GetMaxTargetDiameter()) + 1;
}

int32 UCustomGameModesWidgetComponent::GetMaxAllowedNumVerticalTargets() const
{
	// Total = GridSpacing.Y * (NumVerticalGridTargets - 1) + (NumVerticalGridTargets - 1) * MaxTargetDiameter;
	// Total = (NumVerticalGridTargets - 1) * (GridSpacing.Y + MaxTargetDiameter);
	// Total / (GridSpacing.Y + MaxTargetDiameter) = NumVerticalGridTargets - 1
	// NumVerticalGridTargets = Total / (GridSpacing.Y * MaxTargetDiameter) + 1
	return MaxValue_VerticalSpread / (BSConfig->GridConfig.GridSpacing.Y + GetMaxTargetDiameter()) + 1;
}

float UCustomGameModesWidgetComponent::GetMaxAllowedHorizontalSpacing() const
{
	// Total = GridSpacing.X * (NumHorizontalGridTargets - 1) + (NumHorizontalGridTargets - 1) * MaxTargetDiameter;
	// Total = (NumHorizontalGridTargets - 1) * (GridSpacing.X + MaxTargetDiameter);
	// Total / (NumHorizontalGridTargets - 1) = GridSpacing.X + MaxTargetDiameter;
	// Total / (NumHorizontalGridTargets - 1) - MaxTargetDiameter = GridSpacing.X;
	return MaxValue_HorizontalSpread / (BSConfig->GridConfig.NumHorizontalGridTargets - 1) - GetMaxTargetDiameter();
}

float UCustomGameModesWidgetComponent::GetMaxAllowedVerticalSpacing() const
{
	// Total = GridSpacing.Y * (NumVerticalGridTargets - 1) + (NumVerticalGridTargets - 1) * MaxTargetDiameter;
	// Total = (NumVerticalGridTargets - 1) * (GridSpacing.Y + MaxTargetDiameter);
	// Total / (NumVerticalGridTargets - 1) = GridSpacing.Y + MaxTargetDiameter;
	// Total / (NumVerticalGridTargets - 1) - MaxTargetDiameter = GridSpacing.Y;
	return MaxValue_VerticalSpread / (BSConfig->GridConfig.NumVerticalGridTargets - 1) - GetMaxTargetDiameter();
}

float UCustomGameModesWidgetComponent::GetMaxAllowedTargetScale() const
{
	// Total = GridSpacing.X * (NumHorizontalGridTargets - 1) + (NumHorizontalGridTargets - 1) * SphereTargetDiameter * Scale;
	// Total - (GridSpacing.X * (NumHorizontalGridTargets - 1)) = (NumHorizontalGridTargets - 1) * SphereTargetDiameter * Scale;
	// Total - (GridSpacing.X * (NumHorizontalGridTargets - 1)) = (NumHorizontalGridTargets - 1) * SphereTargetDiameter * Scale;
	// (Total - (GridSpacing.X * (NumHorizontalGridTargets - 1))) / ((NumHorizontalGridTargets - 1) * SphereTargetDiameter) = Scale;
	// Scale = (Total - (GridSpacing.X * (NumHorizontalGridTargets - 1))) / ((NumHorizontalGridTargets - 1) * SphereTargetDiameter)

	const float Horizontal = (MaxValue_HorizontalSpread - (BSConfig->GridConfig.GridSpacing.X * (BSConfig->GridConfig.
		NumHorizontalGridTargets - 1))) / ((BSConfig->GridConfig.NumHorizontalGridTargets - 1) * SphereTargetDiameter);

	const float Vertical = (MaxValue_VerticalSpread - (BSConfig->GridConfig.GridSpacing.Y * (BSConfig->GridConfig.
		NumVerticalGridTargets - 1))) / ((BSConfig->GridConfig.NumVerticalGridTargets - 1) * SphereTargetDiameter);
	return FMath::Min(Horizontal, Vertical);
}
