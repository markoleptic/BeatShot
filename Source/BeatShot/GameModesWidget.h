// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorBase.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/EditableTextBox.h"
#include "GameModesWidget.generated.h"

class UDefaultGameInstance;
class UButton;
class UComboBoxString;
class USlider;

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

public:

	// The array of Game Modes to display
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode State")
		TArray<FGameModeActorStruct> GameModesToDisplay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "GameMode State")
		UComboBoxString* GameModeCategoryComboBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "GameMode State")
		UComboBoxString* GameModeNameComboBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "GameMode State")
		UEditableTextBox* CustomGameModeETB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode State")
		EGameModeActorName SelectedGameMode;

	UFUNCTION(BlueprintCallable, Category = "Default GameModes")
		FGameModeActorStruct GameModeActorStructConstructor(EGameModeActorName GameModeActor,
		EGameModeDifficulty NewGameModeDifficulty = EGameModeDifficulty::Normal,
		ESpreadType NewSpreadType = ESpreadType::StaticNarrow);

	// The array of default Game Modes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default GameModes")
		TArray<FGameModeActorStruct> GameModeActorDefaults;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Default GameModes")
		UBorder* SpreadSelect;
	
	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
		void SaveCustomGameMode();

	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
		void UpdateCustomGameModeOptions(FString SelectedGameModeActorName, ESelectInfo::Type SelectionType);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Custom GameModes")
		void PopulateGameModeOptions(FGameModeActorStruct InputGameModeActorStruct);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Custom GameModes")
		void PopulateGameModeNameComboBox(const FString& OptionToSelect);

	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
		void ChangeSaveButtonStates(const FText& Text);

	// loads and returns CustomGameModesMap from CustomGameModes save slot
	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
		TMap<FString, FGameModeActorStruct> LoadCustomGameModes();
	
	// The object used to save custom game mode properties to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom GameModes")
		FGameModeActorStruct CustomGameMode;

	// The map to add custom game modes to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom GameModes")
		TMap<FString, FGameModeActorStruct> CustomGameModesMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
		USlider* MinTargetScaleSlider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
		USlider* MaxTargetScaleSlider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
		USlider* SpawnWidthSlider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
		USlider* SpawnHeightSlider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Navigation Buttons")
		UButton* SaveCustom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Navigation Buttons")
		UButton* SaveCustomAndStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Navigation Buttons")
		UButton* StartCustom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Navigation Buttons")
		UButton* RemoveSelectedCustom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Navigation Buttons")
		UButton* RemoveAllCustom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Navigation Buttons")
		UButton* CustomizeFromStandard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Navigation Buttons")
		UButton* PlayFromStandard;

	// only called when user tries to save game mode or play game mode
	UFUNCTION(BlueprintCallable, Category = "BeatGrid")
		bool CheckAllBeatGridConstraints();

	UFUNCTION(BlueprintCallable, Category = "BeatGrid")
		void BeatGridTargetSizeConstrained(float value);

	UFUNCTION(BlueprintCallable, Category = "BeatGrid")
		void BeatGridSpawnAreaConstrained(float value);

	UFUNCTION(BlueprintCallable, Category = "BeatGrid")
		void BeatGridNumberOfTargetsConstrained(FString SelectedSong, ESelectInfo::Type SelectionType);

	UFUNCTION(BlueprintCallable, Category = "BeatGrid")
		void BeatGridSpacingConstrained(float value);

	UPROPERTY(BlueprintAssignable, Category = "BeatGrid")
		FUpdateBeatGridConstraints BeatGridConstraintsDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "BeatGrid")
		USlider* BeatGridHorizontalSpacingSlider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "BeatGrid")
		USlider* BeatGridVerticalSpacingSlider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "BeatGrid")
		UComboBoxString* MaxNumBeatGridTargetsComboBox;

	UFUNCTION(BlueprintCallable, Category = "Utility Functions")
		FGameModeActorStruct FindGameModeFromString(const FString& GameModeName);

	UFUNCTION(BlueprintCallable, Category = "Utility Functions")
		FGameModeActorStruct FindGameMode(EGameModeActorName GameModeActorName = EGameModeActorName::Custom, FString CustomGameModeName = "");

	UFUNCTION(BlueprintCallable, Category = "Utility Functions")
		bool IsCustomGameMode(const FString& CustomGameModeName);

	UFUNCTION(BlueprintCallable, Category = "Utility Functions")
		bool IsDefaultGameMode(const FString& GameModeName);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
		UDefaultGameInstance* GI;

	const float SphereDiameter = 100.f;
};
