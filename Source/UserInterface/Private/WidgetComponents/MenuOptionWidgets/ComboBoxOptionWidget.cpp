// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/BSComboBoxEntry.h"
#include "WidgetComponents/BSComboBoxEntry_Tagged.h"
#include "WidgetComponents/GameModeCategoryTagWidget.h"
#include "WidgetComponents/BSComboBoxString.h"

void UComboBoxOptionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ComboBox->OnGenerateWidgetEventDelegate.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent);
	ComboBox->OnSelectionChanged_GenerateWidgetForMultiSelection.BindDynamic(this, &ThisClass::OnSelectionChanged_GenerateMultiSelectionItem);
}

UBSComboBoxEntry* UComboBoxOptionWidget::ConstructComboBoxEntryWidget()
{
	return CreateWidget<UBSComboBoxEntry>(this, ComboBox->GetComboboxEntryWidget());
}

UWidget* UComboBoxOptionWidget::OnGenerateWidgetEvent(const UBSComboBoxString* ComboBoxString, FString Method)
{
	UWidget* Widget = IBSWidgetInterface::OnGenerateWidgetEvent(ComboBoxString, Method);
	
	if (UBSComboBoxEntry_Tagged* Tagged = Cast<UBSComboBoxEntry_Tagged>(Widget))
	{
		const FText EntryText = Tagged->GetEntryText();
		FCategoryEntryTag EntryTag;
		EntryTag.EntryText = Tagged->GetEntryText();
		const int32 Found = EntryTags.Find(EntryTag);
		if (Found != INDEX_NONE)
		{
			for (const FGameplayTag& Tag : EntryTags[Found].GameModeCategoryTags)
			{
				if (GameModeCategoryTagWidgets)
				{
					if (const TSubclassOf<UGameModeCategoryTagWidget>* Subclass = GameModeCategoryTagWidgets->Find(Tag))
					{
						UGameModeCategoryTagWidget* TagWidget = CreateWidget<UGameModeCategoryTagWidget>(this, *Subclass);
						Tagged->AddGameModeCategoryTagWidget(TagWidget);
					}
				}
			}
		}
	}
	return Widget;
}

UWidget* UComboBoxOptionWidget::OnSelectionChanged_GenerateMultiSelectionItem(const UBSComboBoxString* ComboBoxString, const TArray<FString>& SelectedOptions)
{
	UWidget* Widget = IBSWidgetInterface::OnSelectionChanged_GenerateMultiSelectionItem(ComboBoxString, SelectedOptions);
	
	if (UBSComboBoxEntry_Tagged* Tagged = Cast<UBSComboBoxEntry_Tagged>(Widget))
	{
		const FText EntryText = Tagged->GetEntryText();
		FCategoryEntryTag EntryTag;
		EntryTag.EntryText = Tagged->GetEntryText();
		const int32 Found = EntryTags.Find(EntryTag);
		if (Found != INDEX_NONE)
		{
			for (const FGameplayTag& Tag : EntryTags[Found].GameModeCategoryTags)
			{
				if (GameModeCategoryTagWidgets)
				{
					if (const TSubclassOf<UGameModeCategoryTagWidget>* Subclass = GameModeCategoryTagWidgets->Find(Tag))
					{
						UGameModeCategoryTagWidget* TagWidget = CreateWidget<UGameModeCategoryTagWidget>(this, *Subclass);
						Tagged->AddGameModeCategoryTagWidget(TagWidget);
					}
				}
			}
		}
	}
	return Widget;
}

FString UComboBoxOptionWidget::GetStringTableKeyFromComboBox(const UBSComboBoxString* ComboBoxString, const FString& EnumString)
{
	if (GetComboBoxEntryTooltipStringTableKey.IsBound())
	{
		return GetComboBoxEntryTooltipStringTableKey.Execute(EnumString);
	}
	return IBSWidgetInterface::GetStringTableKeyFromComboBox(ComboBoxString, EnumString);
}

void UComboBoxOptionWidget::SortAndAddOptions(TArray<FString>& InOptions) const
{
	InOptions.Sort([] (const FString& FirstOption, const FString& SecondOption)
	{
		return FirstOption < SecondOption;
	});
	for (const FString Option : InOptions)
	{
		ComboBox->AddOption(Option);
	}
}

void UComboBoxOptionWidget::SetGameModeCategoryTagWidgets(TMap<FGameplayTag, TSubclassOf<UGameModeCategoryTagWidget>>& InMap)
{
	GameModeCategoryTagWidgets = &InMap;
}
