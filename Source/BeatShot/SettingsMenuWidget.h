// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameModeActorStruct.h"
#include "SettingsMenuWidget.generated.h"

class USlider;
class UEditableTextBox;
class UComboBoxString;
class UTextBlock;

/**
 * 
 */
UCLASS()
class BEATSHOT_API USettingsMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

public:

	// Load AASettings from Save slot
	UFUNCTION(BlueprintCallable, Category = "Player Settings")
		void LoadPlayerSettings();

	// Save AASettings to Save slot
	UFUNCTION(BlueprintCallable, Category = "Player Settings")
		void SavePlayerSettings();

	// Reset AASettings to default value and repopulate in Settings Menu. Doesn't automatically save
	UFUNCTION(BlueprintCallable, Category = "Player Settings")
		void ResetPlayerSettings();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
		class UDefaultGameInstance* GI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Settings")
		FPlayerSettings PlayerSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Settings")
		UTextBlock* CurrentSensitivityValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Settings")
		UEditableTextBox* NewSensitivityValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Settings")
		UEditableTextBox* MasterSoundInputValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Settings")
		UEditableTextBox* MenuSoundInputValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Settings")
		UEditableTextBox* MusicSoundInputValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Settings")
		USlider* MasterSoundSlider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Settings")
		USlider* MenuSoundSlider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Settings")
		USlider* MusicSoundSlider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Settings")
		USlider* SensSlider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Settings")
		UComboBoxString* ResolutionComboBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Settings")
		UComboBoxString* WindowModeComboBox;
};
