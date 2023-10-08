// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSWidgetInterface.h"
#include "EnumTagMap.h"
#include "WidgetComponents/MenuOptionWidgets/MenuOptionWidget.h"
#include "ComboBoxOptionWidget.generated.h"

class UBSComboBoxEntry_Tagged;
class UEnumTagMap;
class UGameModeCategoryTagWidget;
class UBSComboBoxString;

DECLARE_DELEGATE_RetVal_OneParam(FString, FGetComboBoxEntryTooltipStringTableKey, const FString& EnumString);

UCLASS()
class USERINTERFACE_API UComboBoxOptionWidget : public UMenuOptionWidget, public IBSWidgetInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UBSComboBoxString* ComboBox;

	/** Executed when a ComboBoxEntry requests a tooltip description. If an empty string is returned, no tooltip image is shown */
	FGetComboBoxEntryTooltipStringTableKey GetComboBoxEntryTooltipStringTableKey;

	/** Sorts the array alphabetically and adds each option to the ComboBox */
	void SortAndAddOptions(TArray<FString>& InOptions);

	/** Sets the correct EnumTagMapping from the EnumTagMap depending on the enum class */
	template <typename T>
	void SetEnumType();

	/** Combines SetEnumType and SortAndAddOptions */
	template <typename T>
	void SortAddOptionsAndSetEnumType(TArray<FString>& InOptions);

	/** Sets the GameplayTagWidgetMap */
	void SetGameplayTagWidgetMap(const TMap<FGameplayTag, TSubclassOf<UGameModeCategoryTagWidget>>& InMap);

	/** Sets the EnumTagMap */
	void SetEnumTagMap(const TObjectPtr<UEnumTagMap> InEnumTagMap);

protected:
	virtual void NativeConstruct() override;
	virtual UTooltipWidget* IBSWidgetInterface::ConstructTooltipWidget() override { return nullptr; }
	virtual UBSComboBoxEntry* ConstructComboBoxEntryWidget() override;
	virtual UWidget* OnGenerateWidgetEvent(const UBSComboBoxString* ComboBoxString, FString Method) override;
	virtual UWidget* OnSelectionChanged_GenerateMultiSelectionItem(const UBSComboBoxString* ComboBoxString,
		const TArray<FString>& SelectedOptions) override;
	virtual FString
	GetStringTableKeyFromComboBox(const UBSComboBoxString* ComboBoxString, const FString& EnumString) override;

	/** Adds GameModeCategoryTagWidgets to the ComboBox entry if matching tags are found */
	UWidget* AddGameModeCategoryTagWidgets(UBSComboBoxEntry_Tagged* ComboBoxEntry);

	/** Pointer to CustomGameModesWidgetComponent's GameplayTagWidgetMap */
	TMap<FGameplayTag, TSubclassOf<UGameModeCategoryTagWidget>> GameplayTagWidgetMap;;

	/** Pointer to CustomGameModesWidgetComponent's EnumTagMap */
	UPROPERTY()
	TObjectPtr<UEnumTagMap> EnumTagMap;

	/** The enum to gameplay tag mapping for this combo box option widget */
	FEnumTagMapping EnumTagMapping;
};

template <typename T>
void UComboBoxOptionWidget::SetEnumType()
{
	if (const FEnumTagMapping* FoundEnumTagMapping = EnumTagMap->GetEnumTagMapping<T>())
	{
		EnumTagMapping = *FoundEnumTagMapping;
	}
}

template <typename T>
void UComboBoxOptionWidget::SortAddOptionsAndSetEnumType(TArray<FString>& InOptions)
{
	if (const FEnumTagMapping* FoundEnumTagMapping = EnumTagMap->GetEnumTagMapping<T>())
	{
		EnumTagMapping = *FoundEnumTagMapping;
	}
	SortAndAddOptions(InOptions);
}
