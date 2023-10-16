// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "EnumTagMap.h"
#include "SubMenuWidgets/GameModesWidgets/CustomGameModesWidgetBase.h"
#include "SubmenuWidgets/SettingsWidgets/BSSettingCategoryWidget.h"
#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/MenuOptionWidget.h"
#include "CustomGameModesWidgetComponent.generated.h"

class UMenuOptionWidget;
class UEditableTextBoxOptionWidget;
class UCheckBoxOptionWidget;
class UComboBoxOptionWidget;
class USliderTextBoxOptionWidget;
class UCustomGameModesWidgetComponent;

USTRUCT(BlueprintType)
struct FMenuOptionTooltipHandle
{
	GENERATED_BODY()

	/** Weak pointer to the widget this data is for */
	UPROPERTY()
	TWeakObjectPtr<UMenuOptionWidget> Widget;

	/** The text to display on the TooltipImage */
	TArray<FTooltipData>* TooltipData;

	FORCEINLINE bool operator==(const FMenuOptionTooltipHandle& Other) const
	{
		if (Widget != Other.Widget)
		{
			return false;
		}
		return true;
	}
};

DECLARE_MULTICAST_DELEGATE(FRequestComponentUpdate);

/** Base class for child widgets of UCustomGameModesWidgetBase */
UCLASS(Abstract)
class USERINTERFACE_API UCustomGameModesWidgetComponent : public UBSSettingCategoryWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	/** Sets BSConfig, sets the pointer to next widget in linked list, and calls UpdateOptionsFromConfig */
	virtual void InitComponent(FBSConfig* InConfigPtr, const int32 InIndex);

	/** Sets all custom game mode option values using the BSConfig pointer. Only changes the values if different. Only called during transitions */
	virtual void UpdateOptionsFromConfig();

	/** Broadcast when a caution/warning tooltip needed to be added, removed, or updated. Helps synchronize caution/warnings across different components */
	FRequestComponentUpdate RequestComponentUpdate;

	/** Broadcast when a widget wants to refresh the preview after a change to the config */
	FRequestGameModePreviewUpdate RequestGameModePreviewUpdate;

	/** Returns whether or not Init has been called */
	bool IsInitialized() const { return bIsInitialized; }

	/** Checks all custom game mode options for validity by calling UpdateWarningTooltips and broadcasts RequestComponentUpdate
	 *  if any are not valid. Should be called anytime an option is changed */
	virtual void UpdateAllOptionsValid();

	/** Returns the struct containing info about the number of caution and warnings current present */
	FCustomGameModeCategoryInfo* GetCustomGameModeCategoryInfo() { return &CustomGameModeCategoryInfo; }

	/** Returns the index set during initialization */
	int32 GetIndex() const { return Index; }

	/** Returns true if the widget should be indexed on the carousel */
	bool ShouldIndexOnCarousel() const { return bIndexOnCarousel; }

protected:

	/** Adds a GameModeCategoryTagWidget for each matching GameplayTag on the Menu Option widget*/
	void AddGameModeCategoryTagWidgets(UMenuOptionWidget* MenuOptionWidget);

	static bool UpdateValueIfDifferent(const USliderTextBoxOptionWidget* Widget, const float Value);
	static bool UpdateValueIfDifferent(const UComboBoxOptionWidget* Widget, const FString& NewOption);
	static bool UpdateValueIfDifferent(const UComboBoxOptionWidget* Widget, const TArray<FString>& NewOptions);
	static bool UpdateValueIfDifferent(const UCheckBoxOptionWidget* Widget, const bool bIsChecked);
	static bool UpdateValueIfDifferent(const UEditableTextBoxOptionWidget* Widget, const FText& NewText);

	/** Iterates through all MenuOptionWidgets, calling UpdateAllWarningTooltips on each. Iterates through each
	 *  widget's TooltipWarningData, checking if any changed from the update. If so, the tooltip is updated.
	 *  Calls UpdateCustomGameModeCategoryInfo when finished. Returns false if any tooltips required an update */
	bool UpdateWarningTooltips();

	/** Iterates through all MenuOptionWidgets to sum the total of Warning and Caution tooltips visible. Updates CustomGameModeCategoryInfo struct */
	void UpdateCustomGameModeCategoryInfo();

	float GetMinRequiredHorizontalSpread() const;
	float GetMinRequiredVerticalSpread() const;
	float GetMaxTargetDiameter() const;
	int32 GetMaxAllowedNumHorizontalTargets() const;
	int32 GetMaxAllowedNumVerticalTargets() const;
	float GetMaxAllowedHorizontalSpacing() const;
	float GetMaxAllowedVerticalSpacing() const;
	float GetMaxAllowedTargetScale() const;

	/** Pointer to the game mode config inside GameModesWidget */
	FBSConfig* BSConfig;

	/** Pointer to next widget in linked list. Used for CreatorView */
	UPROPERTY()
	TObjectPtr<UCustomGameModesWidgetComponent> Next;

	/** Whether or not Init has been called */
	bool bIsInitialized = false;

	/** Index used for parent widgets */
	int32 Index = -1;

	/** Struct containing info about NumWarning & NumCaution tooltips */
	FCustomGameModeCategoryInfo CustomGameModeCategoryInfo;

	UPROPERTY()
	TArray<TObjectPtr<UMenuOptionWidget>> MenuOptionWidgets;

	/** Whether or not to index this widget as part of the carousel */
	UPROPERTY(EditAnywhere, Category="CustomGameModesWidgetComponent")
	bool bIndexOnCarousel = true;

	UPROPERTY(EditDefaultsOnly, Category="CustomGameModesWidgetComponent")
	TObjectPtr<UEnumTagMap> EnumTagMap;

	UPROPERTY(EditDefaultsOnly, Category="CustomGameModesWidgetComponent")
	TMap<FGameplayTag, TSubclassOf<UGameModeCategoryTagWidget>> GameplayTagWidgetMap;


	/** Returns the string display name of the enum, or empty string if not found */
	template <typename T>
	FString GetStringFromEnum_FromTagMap(const T& InEnum);

	template <typename T>
	TArray<FString> GetStringArrayFromEnumArray_FromTagMap(const TArray<T>& InEnumArray);
};

template <typename T>
FString UCustomGameModesWidgetComponent::GetStringFromEnum_FromTagMap(const T& InEnum)
{
	if (EnumTagMap)
	{
		FString Found = EnumTagMap->GetStringFromEnumTagPair<T>(InEnum);
		if (!Found.IsEmpty())
		{
			return Found;
		}
	}
	return GetStringFromEnum(InEnum);
}

template <typename T>
TArray<FString> UCustomGameModesWidgetComponent::GetStringArrayFromEnumArray_FromTagMap(const TArray<T>& InEnumArray)
{
	TArray<FString> OutArray;
	for (const T& InEnum : InEnumArray)
	{
		OutArray.Add(GetStringFromEnum_FromTagMap<T>(InEnum));
	}
	return OutArray;
}
