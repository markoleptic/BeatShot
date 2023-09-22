// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSWidgetInterface.h"
#include "WidgetComponents/MenuOptionWidgets/MenuOptionWidget.h"
#include "ComboBoxOptionWidget.generated.h"

class UGameModeCategoryTagWidget;
DECLARE_DELEGATE_RetVal_OneParam(FString, FGetComboBoxEntryTooltipStringTableKey, const FString& EnumString);

class UBSComboBoxString;

USTRUCT(BlueprintType, meta=(ShowOnlyInnerProperties))
struct FCategoryEntryTag
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText EntryText;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer GameModeCategoryTags;

	FORCEINLINE bool operator==(const FCategoryEntryTag& Other) const
	{
		return EntryText.EqualTo(Other.EntryText);
	}
	
	FCategoryEntryTag()
	{
		EntryText = FText();
		GameModeCategoryTags = FGameplayTagContainer();
	}
	
	FCategoryEntryTag(const FText& InCompareText)
	{
		EntryText = InCompareText;
		GameModeCategoryTags = FGameplayTagContainer();
	}
};

USTRUCT(BlueprintType, meta=(ShowOnlyInnerProperties))
struct FCategoryEntryTagClass
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag GameModeCategoryTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameModeCategoryTagWidget> GameModeCategoryTagClass;

	FORCEINLINE bool operator==(const FCategoryEntryTagClass& Other) const
	{
		return GameModeCategoryTag.MatchesTagExact(Other.GameModeCategoryTag);
	}
	
	FCategoryEntryTagClass()
	{
		GameModeCategoryTag = FGameplayTag();
		GameModeCategoryTagClass = TSubclassOf<UGameModeCategoryTagWidget>(nullptr);
	}
	FCategoryEntryTagClass(const FGameplayTag& InTag)
	{
		GameModeCategoryTag = InTag;
		GameModeCategoryTagClass = TSubclassOf<UGameModeCategoryTagWidget>(nullptr);
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
	void SetGameModeCategoryTagWidgets(TArray<FCategoryEntryTagClass>& InArray);

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
	TArray<FCategoryEntryTagClass>* GameModeCategoryTagClasses;
};
