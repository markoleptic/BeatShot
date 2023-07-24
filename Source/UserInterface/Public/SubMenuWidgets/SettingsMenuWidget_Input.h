// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlayerMappableInputConfig.h"
#include "SaveLoadInterface.h"
#include "WidgetComponents/BSSettingCategoryWidget.h"
#include "SettingsMenuWidget_Input.generated.h"


/*USTRUCT()
struct FActionMappingInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<float> Accuracy;
	TArray<int32> TotalSpawns;
	TArray<int32> TotalHits;

	FActionMappingInfo()
	{
		Accuracy = TArray<float>();
		TotalSpawns = TArray<int32>();
		TotalHits = TArray<int32>();
	}

	FActionMappingInfo(const int32 Size)
	{
		Accuracy = TArray<float>();
		Accuracy.Init(-1.f, Size);
		TotalSpawns = TArray<int32>();
		TotalSpawns.Init(-1, Size);
		TotalHits = TArray<int32>();
		TotalHits.Init(0, Size);
	}
	FORCEINLINE bool operator==(const FActionMappingInfo& Other) const
	{
		return false;
	}
	friend FORCEINLINE uint32 GetTypeHash(const FActionMappingInfo& ActionMappingInfo)
	{
		return HashCombine(GetTypeHash(ActionMappingInfo.GameModeType), HashCombine(GetTypeHash(ActionMappingInfo.BaseGameMode), HashCombine(GetTypeHash(ActionMappingInfo.CustomGameModeName), GetTypeHash(ActionMappingInfo.Difficulty))));
	}
};*/

class USavedTextWidget;
class UBSButton;
class UTextBlock;
class UInputMappingWidget;

UCLASS()
class USERINTERFACE_API USettingsMenuWidget_Input : public UBSSettingCategoryWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

	friend class USettingsMenuWidget;

public:
	/** Returns OnPlayerSettingsChangedDelegate_User, the delegate that is broadcast when this class saves User settings */
	FOnPlayerSettingsChanged_User& GetPublicUserSettingsChangedDelegate() { return OnPlayerSettingsChangedDelegate_User; }
	
	virtual void NativeConstruct() override;

	/** Populates the settings menu */
	void InitializeInputSettings(const FPlayerSettings_User& PlayerSettings_User);

protected:
	/** Function bound to all InputMappingWidgets' OnKeySelected delegates */
	void OnKeySelected(const FName MappingName, const FInputChord SelectedKey);

	TArray<UInputMappingWidget*> FindInputMappingWidgetsByKey(const FKey InKey) const;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInputMappingWidget> InputMappingWidgetClass;
	UPROPERTY(EditDefaultsOnly)
	UPlayerMappableInputConfig* PlayerMappableInputConfig;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSVerticalBox* BSBox_Combat;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSVerticalBox* BSBox_Movement;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSVerticalBox* BSBox_Sensitivity;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sensitivity")
	UTextBlock* Value_CurrentSensitivity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sensitivity")
	UEditableTextBox* Value_NewSensitivity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sensitivity")
	UEditableTextBox* Value_NewSensitivityCsgo;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sensitivity")
	USlider* Slider_Sensitivity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Saving")
	UBSButton* Button_Save;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Saving")
	UBSButton* Button_Reset;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Saving")
	UBSButton* Button_Revert;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USavedTextWidget* SavedTextWidget;

	float Sensitivity;
	TMap<FName, FKey> TempKeybindings;
	FPlayerSettings_User InitialPlayerSettings;

	UFUNCTION()
	void OnValueChanged_NewSensitivity(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnValueChanged_NewSensitivityCsgo(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnSliderChanged_Sensitivity(const float NewValue);
	UFUNCTION()
	void OnButtonClicked_BSButton(const UBSButton* Button);
	void OnButtonClicked_Save();
	void OnButtonClicked_Reset();
	void OnButtonClicked_Revert();
	
	TArray<UInputMappingWidget*> InputMappingWidgets;
};
