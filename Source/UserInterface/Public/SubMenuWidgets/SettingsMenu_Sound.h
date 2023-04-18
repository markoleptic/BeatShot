// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalStructs.h"
#include "WidgetComponents/BSSettingCategoryWidget.h"
#include "SettingsMenu_Sound.generated.h"

class UBSHorizontalBox;
class UEditableTextBox;
class USlider;

UCLASS()
class USERINTERFACE_API USettingsMenu_Sound : public UBSSettingCategoryWidget
{
	GENERATED_BODY()
	
	friend class USettingsMenuWidget;
	virtual void NativeConstruct() override;
	virtual void InitSettingCategoryWidget() override;

public:
	void InitializeAudioSettings(const FPlayerSettings_VideoAndSound& InVideoAndSoundSettings);
	FPlayerSettings_VideoAndSound GetAudioSettings() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes | Sound")
	USoundClass* GlobalSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes | Sound")
	USoundClass* MenuSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes | Sound")
	USoundMix* GlobalSoundMix;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	UBSHorizontalBox* BSBox_GlobalSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	UBSHorizontalBox* BSBox_MenuSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	UBSHorizontalBox* BSBox_MusicSound;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	UEditableTextBox* Value_GlobalSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	UEditableTextBox* Value_MenuSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	UEditableTextBox* Value_MusicSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound ")
	USlider* Slider_GlobalSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	USlider* Slider_MenuSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	USlider* Slider_MusicSound;

	UFUNCTION()
	void OnSliderChanged_GlobalSound(const float NewGlobalSound);
	UFUNCTION()
	void OnSliderChanged_MenuSound(const float NewMenuSound);
	UFUNCTION()
	void OnSliderChanged_MusicSound(const float NewMusicSound);
	UFUNCTION()
	void OnValueChanged_GlobalSound(const FText& NewGlobalSound, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnValueChanged_MenuSound(const FText& NewMenuSound, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnValueChanged_MusicSound(const FText& NewMusicSound, ETextCommit::Type CommitType);
};
