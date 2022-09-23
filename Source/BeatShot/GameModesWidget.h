// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveGameCustomGameMode.h"
#include "GameModesWidget.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom GameModes")
	USaveGameCustomGameMode* SaveGameCustomGameMode;

	// The object used to save custom game mode properties to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom GameModes")
	FGameModeActorStruct CustomGameMode;

	// The map to add custom game modes to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom GameModes")
	TMap<FString, FGameModeActorStruct> CustomGameModesMap;

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

	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
		void UpdateBeatGridHeightConstraints(float value);

	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
		void UpdateBeatGridWidthConstraints(float value);

	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
		void BeatGridTargetSizeConstrained(float value);

	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
		void BeatGridSpawnAreaConstrained(float value);

	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
		void BeatGridNumberOfTargetsConstrained(FString SelectedSong, ESelectInfo::Type SelectionType);

	UFUNCTION(BlueprintCallable, Category = "Custom GameModes")
		void BeatGridSpacingConstrained(float value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom GameModes")
		FUpdateBeatGridConstraints BeatGridConstraintsDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
		class UDefaultGameInstance* GI;

	const float SphereDiameter = 100.f;
};
