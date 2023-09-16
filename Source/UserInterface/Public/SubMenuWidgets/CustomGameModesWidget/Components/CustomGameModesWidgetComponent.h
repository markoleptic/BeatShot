// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "SubMenuWidgets/CustomGameModesWidget/CustomGameModesWidgetBase.h"
#include "WidgetComponents/BSSettingCategoryWidget.h"
#include "WidgetComponents/MenuOptionWidgets/MenuOptionWidget.h"
#include "CustomGameModesWidgetComponent.generated.h"

class UMenuOptionWidget;
class UEditableTextBoxOptionWidget;
class UCheckBoxOptionWidget;
class UComboBoxOptionWidget;
class USliderTextBoxWidget;
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
	virtual void InitComponent(FBSConfig* InConfigPtr, const TObjectPtr<UCustomGameModesWidgetComponent> InNext);

	/** Sets all custom game mode option values using the BSConfig pointer. Only changes the values if different. Only called during transitions */
	virtual void UpdateOptionsFromConfig();

	/** Returns the next CustomGameModesWidgetComponent to transition to */
	TObjectPtr<UCustomGameModesWidgetComponent> GetNext() const;

	/** Broadcast when a caution/warning tooltip needed to be added, removed, or updated. Helps synchronize caution/warnings across different components */
	FRequestComponentUpdate RequestComponentUpdate;

	/** Returns whether or not Init has been called */
	bool IsInitialized() const { return bIsInitialized; }

	/** Checks all custom game mode options for validity by calling UpdateWarningTooltips and broadcasts RequestComponentUpdate
	 *  if any are not valid. Should be called anytime an option is changed */
	virtual void UpdateAllOptionsValid();

	/** Returns the struct containing info about the number of caution and warnings current present */
	FCustomGameModeCategoryInfo* GetCustomGameModeCategoryInfo() { return &CustomGameModeCategoryInfo; }

protected:
	/** Sets value of Next */
	void SetNext(const TObjectPtr<UCustomGameModesWidgetComponent> InNext);

	static bool UpdateValueIfDifferent(const USliderTextBoxWidget* Widget, const float Value);
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

	/** Width of spawn area, which is StaticHorizontalSpread - MaxTargetSize since targets are allowed to spawn with their center on the edge */
	float GetHorizontalSpread() const;

	/** Height of spawn area, which is StaticVerticalSpread - MaxTargetSize since targets are allowed to spawn with their center on the edge */
	float GetVerticalSpread() const;
	
	float GetMinRequiredHorizontalSpread() const;
	float GetMinRequiredVerticalSpread() const;
	
	/** MaxSpawnedTargetScale * SphereTargetDiameter */
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

	/** Struct containing info about NumWarning & NumCaution tooltips */
	FCustomGameModeCategoryInfo CustomGameModeCategoryInfo;
	
	UPROPERTY()
	TArray<TObjectPtr<UMenuOptionWidget>> MenuOptionWidgets;
};

