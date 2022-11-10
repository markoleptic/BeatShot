// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorBase.h"
#include "Blueprint/UserWidget.h"
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

	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
		FGameModeActorStruct GameModeActorStructConstructor(EGameModeActorName GameModeActor,
			EGameModeDifficulty NewGameModeDifficulty = EGameModeDifficulty::Normal,
			ESpreadType NewSpreadType = ESpreadType::StaticNarrow);

	// saves CustomGameModesMap to CustomGameModes save slot
	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
	void SaveCustomGameMode();

	// resets CustomGameMode to default
	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
	void ResetCustomGameMode();

	// loads and returns CustomGameModesMap from CustomGameModes save slot
	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
	TMap<FString, FGameModeActorStruct> LoadCustomGameModes();

	// Called after LoadCustomGameModes to update UI with custom game modes
	UFUNCTION(BlueprintImplementableEvent, Category = "Custom GameModes")
	void PopulateGameModeSettings();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
		UComboBoxString* GameModeCategoryComboBox;

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
		UButton* SaveCustomGameModeButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Custom GameModes")
		UButton* SaveAllAndStartGameModeButton;

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
		void BeatGridSpacingConstrained(float value);

	UPROPERTY(BlueprintAssignable, Category = "Custom GameModes")
		FUpdateBeatGridConstraints BeatGridConstraintsDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
		UDefaultGameInstance* GI;

	const float SphereDiameter = 100.f;
};
