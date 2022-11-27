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

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

public:

	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
		FGameModeActorStruct GameModeActorStructConstructor(EGameModeActorName GameModeActor,
			EGameModeDifficulty NewGameModeDifficulty = EGameModeDifficulty::Normal,
			ESpreadType NewSpreadType = ESpreadType::StaticNarrow);

	// The object used to save custom game mode properties to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom GameModes")
		FGameModeActorStruct CustomGameMode;

	// The map to add custom game modes to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom GameModes")
		TMap<FString, FGameModeActorStruct> CustomGameModesMap;

	// The array of default Game Modes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default GameModes")
		TArray<FGameModeActorStruct> GameModeActorDefaults;

	// The array of Game Modes to display
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default GameModes")
		TArray<FGameModeActorStruct> GameModesToDisplay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Default GameModes")
		UButton* CustomizeFromStandard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Default GameModes")
		UButton* PlayFromStandard;

	// saves CustomGameModesMap to CustomGameModes save slot
	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
		void SaveCustomGameMode();

	// loads and returns CustomGameModesMap from CustomGameModes save slot
	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
		TMap<FString, FGameModeActorStruct> LoadCustomGameModes();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
		UComboBoxString* GameModeCategoryComboBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
		UComboBoxString* GameModeNameComboBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
		USlider* SpawnWidthSlider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
		USlider* SpawnHeightSlider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
		USlider* BeatGridHorizontalSpacingSlider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
		USlider* BeatGridVerticalSpacingSlider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
		UComboBoxString* MaxNumBeatGridTargetsComboBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
		USlider* MinTargetScaleSlider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
		USlider* MaxTargetScaleSlider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
		UButton* SaveCustom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
		UButton* SaveCustomAndStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
		UButton* StartCustom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
		UButton* RemoveSelectedCustom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
		UButton* RemoveAllCustom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
		UBorder* DynamicSpreadSelect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
		UBorder* SpreadSelect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
		UEditableTextBox* CustomGameModeETB;

	UFUNCTION(Category = "Custom GameModes")
		void ChangeSaveButtonStates(const FText& Text);

	// only called when user tries to save game mode or play game mode
	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
		bool CheckAllBeatGridConstraints();

	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
		void BeatGridTargetSizeConstrained(float value);

	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
		void BeatGridSpawnAreaConstrained(float value);

	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
		void BeatGridNumberOfTargetsConstrained(FString SelectedSong, ESelectInfo::Type SelectionType);

	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
		void UpdateCustomGameModeOptions(FString SelectedGameModeActorName, ESelectInfo::Type SelectionType);

	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
		void BeatGridSpacingConstrained(float value);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Custom GameModes")
		void PopulateGameModeOptions(FGameModeActorStruct InputGameModeActorStruct);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Custom GameModes")
		void PopulateGameModeNameComboBox(const FString& OptionToSelect);

	UPROPERTY(BlueprintAssignable, Category = "Custom GameModes")
		FUpdateBeatGridConstraints BeatGridConstraintsDelegate;

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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameModes")
		EGameModeActorName SelectedGameMode;
	
};
