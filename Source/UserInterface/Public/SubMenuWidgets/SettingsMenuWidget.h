// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once

#include "CoreMinimal.h"
#include "AASettingsWidget.h"
#include "SaveLoadInterface.h"
#include "Blueprint/UserWidget.h"
#include "SettingsMenuWidget.generated.h"

class UCrossHairSettingsWidget;
class UGameSettingsWidget;
class UVideoAndSoundSettingsWidget;
class USensitivitySettingsWidget;
class USlideRightButton;
class UVerticalBox;
class UWidgetSwitcher;


UCLASS()
class USERINTERFACE_API USettingsMenuWidget : public UUserWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

public:
	/** Whether or not this instance of SettingsMenuWidget belongs to MainMenuWidget or not */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Default", meta = (ExposeOnSpawn="true"))
	bool bIsMainMenuChild;

	UPROPERTY()
	FOnPlayerSettingsChange OnPlayerSettingsChanged;

	UPROPERTY()
	FOnAASettingsChange OnAASettingsChanged;
	
	FOnRestartButtonClicked OnRestartButtonClicked;

protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnPlayerSettingsSaved();

	UFUNCTION()
	void OnAASettingsSaved();

	FGameModeTransitionState RestartState;

	UFUNCTION()
	void OnRestartButtonClicked_AudioAnalyzer() { if (!OnRestartButtonClicked.ExecuteIfBound()) {
		UE_LOG(LogTemp, Display, TEXT("OnRestartButtonClicked not bound."));
	} }
	
	/** A map to store buttons and the widgets they associate with */
	UPROPERTY()
	TMap<USlideRightButton*, UVerticalBox*> MenuWidgets;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;

	/** Containers for each widget in SettingsMenu */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* SM_Game;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* SM_VideoAndSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* SM_AudioAnalyzer;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* SM_Sensitivity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* SM_CrossHair;
	
	/* --------------- */
	/* SubMenu Widgets */
	/* --------------- */
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UGameSettingsWidget* SM_Game_Widget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVideoAndSoundSettingsWidget* SM_VideoAndSound_Widget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USensitivitySettingsWidget* SM_Sensitivity_Widget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCrossHairSettingsWidget* SM_CrossHair_Widget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UAASettingsWidget* SM_AudioAnalyzer_Widget;
	
	/* ------------------ */
	/* Navigation Buttons */
	/* ------------------ */

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SM_Game_Button;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SM_VideoAndSound_Button;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SM_AudioAnalyzer_Button;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SM_Sensitivity_Button;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SM_CrossHair_Button;

	/** Function to Play the Slide Animation for Navigation Buttons */
	UFUNCTION()
	void SlideButtons(const USlideRightButton* ActiveButton);
	UFUNCTION()
	void OnSM_Game_ButtonClicked() { SlideButtons(SM_Game_Button); }
	UFUNCTION()
	void OnSM_VideoAndSound_ButtonClicked() { SlideButtons(SM_VideoAndSound_Button); }
	UFUNCTION()
	void OnSM_AudioAnalyzer_ButtonClicked() { SlideButtons(SM_AudioAnalyzer_Button); }
	UFUNCTION()
	void OnSM_Sensitivity_ButtonClicked() { SlideButtons(SM_Sensitivity_Button); }
	UFUNCTION()
	void OnSM_CrossHair_ButtonClicked() { SlideButtons(SM_CrossHair_Button); }
};
