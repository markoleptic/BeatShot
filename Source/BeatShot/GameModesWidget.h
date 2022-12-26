// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorBase.h"
#include "GameModeButton.h"
#include "Blueprint/UserWidget.h"
#include "GameModesWidget.generated.h"

class UPopupMessageWidget;
class UDefaultGameInstance;
class UButton;
class UBorder;
class UEditableTextBox;
class UComboBoxString;
class USlider;
class UCheckBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateBeatGridConstraints, float, value);

/**
 *
 */
UCLASS()
class BEATSHOT_API UGameModesWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	virtual void NativeConstruct() override;

#pragma region CustomGeneral

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom Game Modes")
	UComboBoxString* GameModeNameComboBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Default Game Modes")
	UEditableTextBox* CustomGameModeETB;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom Game Modes")
	UComboBoxString* BaseGameModeComboBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom Game Modes")
	UComboBoxString* GameModeDifficultyComboBox;

#pragma endregion

#pragma region CustomSaveStart
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom Game Modes")
	UButton* SaveCustomButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom Game Modes")
	UButton* StartWithoutSavingButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom Game Modes")
	UButton* SaveCustomAndStartButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom Game Modes")
	UButton* StartCustomButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom Game Modes")
	UButton* RemoveSelectedCustomButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom Game Modes")
	UButton* RemoveAllCustomButton;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UPopupMessageWidget* PopUpMessageWidget;

	/** saves CustomGameModesMap to CustomGameModes save slot */
	void SaveCustomGameMode() const;
	/** loads and returns CustomGameModesMap from CustomGameModes save slot */
	TMap<FString, FGameModeActorStruct> LoadCustomGameModes();
	/** Changes the Save Button states depending on what is selected in GameModeNameComboBox and CustomGameModeETB */
	UFUNCTION()
	void ChangeSaveButtonStates(const FText& Text);

#pragma endregion

#pragma region TimeRelated

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
	UComboBoxString* PlayerDelayComboBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
	USlider* LifespanSlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
	UEditableTextBox* LifespanValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
	USlider* TargetSpawnCDSlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
	UEditableTextBox* TargetSpawnCDValue;

#pragma endregion

#pragma region TargetSpread

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UCheckBox* HeadShotOnlyCheckBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UCheckBox* WallCenteredCheckBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	USlider* MinTargetDistanceSlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* MinTargetDistanceValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UComboBoxString* SpreadTypeComboBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom Game Modes")
	USlider* HorizontalSpreadSlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* HorizontalSpreadValue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom Game Modes")
	USlider* VerticalSpreadSlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* VerticalSpreadValue;

#pragma endregion

#pragma region TargetSizing

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UCheckBox* ConstantTargetScaleCheckBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UCheckBox* DynamicTargetScaleCheckBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	USlider* MinTargetScaleSlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* MinTargetScaleValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	USlider* MaxTargetScaleSlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* MaxTargetScaleValue;

#pragma endregion

#pragma region BeatGrid

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "BeatGrid Settings")
	UBorder* BeatGridSpecificSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom Game Modes")
	USlider* BeatGridHorizontalSpacingSlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom Game Modes")
	USlider* BeatGridVerticalSpacingSlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom Game Modes")
	UComboBoxString* NumBeatGridTargetsComboBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "BeatGrid Settings")
	UCheckBox* ConstantBeatGridSpacingCheckBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "BeatGrid Settings")
	UCheckBox* RandomizeNextBeatGridTargetCheckBox;

	// only called when user tries to save game mode or play game mode
	UFUNCTION()
	bool CheckAllBeatGridConstraints();
	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
	void BeatGridTargetSizeConstrained(float value);
	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
	void BeatGridSpawnAreaConstrained(float value);
	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
	void BeatGridNumberOfTargetsConstrained(FString SelectedSong, ESelectInfo::Type SelectionType);
	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
	void UpdateCustomGameModeOptions(FString SelectedGameMode, ESelectInfo::Type SelectionType);
	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
	void BeatGridSpacingConstrained(float value);

	FUpdateBeatGridConstraints BeatGridConstraintsDelegate;

#pragma endregion

#pragma region BeatTrack

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "BeatTrack Settings")
	UBorder* BeatTrackSpecificSettings;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "BeatTrack Settings")
	UCheckBox* ConstantTargetSpeedCheckBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "BeatTrack Settings")
	USlider* BeatTrackMinTargetSpeedSlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "BeatTrack Settings")
	UEditableTextBox*  BeatTrackMinTargetSpeedValue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "BeatTrack Settings")
	USlider* BeatTrackMaxTargetSpeedSlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "BeatTrack Settings")
	UEditableTextBox* BeatTrackMaxTargetSpeedValue;

#pragma endregion

#pragma region DefaultGameModes

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* BeatGridNormalButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* BeatGridHardButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* BeatGridDeathButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* BeatTrackNormalButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* BeatTrackHardButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* BeatTrackDeathButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* MultiBeatNormalButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* MultiBeatHardButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* MultiBeatDeathButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* SingleBeatNormalButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* SingleBeatHardButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* SingleBeatDeathButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Default Game Modes")
	UBorder* SpreadSelect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Default Game Modes")
	UButton* DynamicSpreadButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Default Game Modes")
	UButton* NarrowSpreadButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Default Game Modes")
	UButton* WideSpreadButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Default Game Modes")
	UButton* CustomizeFromStandardButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Default Game Modes")
	UButton* PlayFromStandardButton;

#pragma endregion

private:

#pragma region UpdateOptions
	
	/** Initializes all Custom game mode options based on the GameModeActorStruct */
	void PopulateGameModeOptions(FGameModeActorStruct InputGameModeActorStruct);
	/** Updates GameModeNameComboBox with CustomGameModes */
	void PopulateGameModeNameComboBox(const FString& GameModeOptionToSelect);

#pragma endregion;
	
	FGameModeActorStruct GetGameMode(const EGameModeActorName& GameModeActorName, const FString& GameModeName);
	FGameModeActorStruct GetCustomGameMode(const FString& CustomGameModeName);
	/** Returns whether or not the GameModeName is part of the game's default game modes */
	bool IsDefaultGameMode(const FString& GameModeName);
	/** Creates default game modes based on Difficulty and Spread Type */
	FGameModeActorStruct GameModeActorStructConstructor(EGameModeActorName GameModeActor,
														EGameModeDifficulty NewGameModeDifficulty =
															EGameModeDifficulty::Normal,
														ESpreadType NewSpreadType = ESpreadType::StaticNarrow);

	
	/** The object used to save custom game mode properties to */
	FGameModeActorStruct CustomGameMode;
	/** The map to add custom game modes to */
	TMap<FString, FGameModeActorStruct> CustomGameModesMap;
	/** The array of default Game Modes */
	TArray<FGameModeActorStruct> GameModeActorDefaults;
	/** The array of Game Modes to display */
	TArray<FGameModeActorStruct> GameModesToDisplay;

	const float SphereDiameter = 100.f;
};
