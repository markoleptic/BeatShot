// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Delegates/DelegateCombinations.h"

#include "VideoSettingButton.generated.h"

UENUM()
enum class EVideoSettingType : uint8
{
	AntiAliasing UMETA(DisplayName="AntiAliasing"),
	GlobalIllumination UMETA(DisplayName="GlobalIllumination"),
	PostProcessing UMETA(DisplayName="PostProcessing"),
	Reflection UMETA(DisplayName="Reflection"),
	Shadow UMETA(DisplayName="Shadow"),
	Shading UMETA(DisplayName="Shading"),
	Texture UMETA(DisplayName="Texture"),
	ViewDistance UMETA(DisplayName="ViewDistance"),
	VisualEffect UMETA(DisplayName="VisualEffect")
};
ENUM_RANGE_BY_FIRST_AND_LAST(EVideoSettingType, EVideoSettingType::AntiAliasing, EVideoSettingType::VisualEffect);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnButtonClicked, const UVideoSettingButton*, VideoSettingButton);

/** Button representing a video setting designed to be stored in a linked list. Contains a pointer to the next one */
UCLASS()
class USERINTERFACE_API UVideoSettingButton : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Broadcasts self when button is clicked */
	FOnButtonClicked OnButtonClicked;

	/** Initializes the widget with setting specifications */
	void InitVideoSettingButton(const EVideoSettingType& InSettingType, const int32 InQuality, const TObjectPtr<UVideoSettingButton>& InNext);

	/** Changes all video settings buttons' background colors within the same video setting category, provided they were initialized correctly.
	 *  The button in which this function was invoked is changed to BeatShotBlue, while the others are changed to white */
	void SetButtonSettingCategoryBackgroundColors() const;

	/** Returns the video setting type for this button widget */
	EVideoSettingType GetVideoSettingType() const { return SettingType; }

	/** Returns the video setting quality for this button widget */
	int32 GetVideoSettingQuality() const { return Quality; }

	UVideoSettingButton* GetVideoSettingButtonFromQuality(const int32 InQuality);

protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UButton* Button;

	/** The video setting quality this button widget represents */
	int32 Quality;

	/** The video setting type this button widget represents */
	EVideoSettingType SettingType;

	/** Pointer to next VideoSettingButton */
	TObjectPtr<UVideoSettingButton> Next;

private:
	UFUNCTION()
	void OnButtonClickedCallback();
};
