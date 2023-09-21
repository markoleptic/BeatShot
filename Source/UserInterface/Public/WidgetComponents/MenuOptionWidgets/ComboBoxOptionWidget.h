// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSWidgetInterface.h"
#include "WidgetComponents/MenuOptionWidgets/MenuOptionWidget.h"
#include "ComboBoxOptionWidget.generated.h"

class UGameModeCategoryTagWidget;
DECLARE_DELEGATE_RetVal_OneParam(FString, FGetComboBoxEntryTooltipStringTableKey, const FString& EnumString);

class UBSComboBoxString;

USTRUCT()
struct FCategoryEntryTag
{
	GENERATED_BODY()
	
	UPROPERTY()
	FText EntryText;
	
	UPROPERTY()
	FGameplayTagContainer GameModeCategoryTags;

	FORCEINLINE bool operator==(const FCategoryEntryTag& Other) const
	{
		return EntryText.EqualTo(Other.EntryText);
	}
};

UCLASS()
class USERINTERFACE_API UComboBoxOptionWidget : public UMenuOptionWidget, public IBSWidgetInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UBSComboBoxString* ComboBox;

	/** Executed when a ComboBoxEntry requests a tooltip description. If an empty string is returned, no tooltip image is shown */
	FGetComboBoxEntryTooltipStringTableKey GetComboBoxEntryTooltipStringTableKey;

	void SortAndAddOptions(TArray<FString>& InOptions) const;

	/** Sets the GameModeCategoryTagWidgets */
	void SetGameModeCategoryTagWidgets(TMap<FGameplayTag, TSubclassOf<UGameModeCategoryTagWidget>>& InMap);

	/** An array of entries, where each entry has tags that correspond to the entry text */
	UPROPERTY(EditInstanceOnly, Category="ComboBoxOptionWidget")
	TArray<FCategoryEntryTag> EntryTags;
	
protected:
	virtual void NativeConstruct() override;
	virtual UTooltipWidget* IBSWidgetInterface::ConstructTooltipWidget() override { return nullptr; }
	virtual UBSComboBoxEntry* ConstructComboBoxEntryWidget() override;
	virtual UWidget* OnGenerateWidgetEvent(const UBSComboBoxString* ComboBoxString, FString Method) override;
	virtual UWidget* OnSelectionChanged_GenerateMultiSelectionItem(const UBSComboBoxString* ComboBoxString, const TArray<FString>& SelectedOptions) override;
	virtual FString GetStringTableKeyFromComboBox(const UBSComboBoxString* ComboBoxString, const FString& EnumString) override;

	/** Pointer to CustomGameModesWidgetComponent's map */
	TMap<FGameplayTag, TSubclassOf<UGameModeCategoryTagWidget>>* GameModeCategoryTagWidgets;
};
