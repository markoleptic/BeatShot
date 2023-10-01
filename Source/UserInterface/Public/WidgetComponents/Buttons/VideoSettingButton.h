// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSButton.h"
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

/** Button representing a video setting designed to be stored in a linked list. Contains a pointer to the next one */
UCLASS()
class USERINTERFACE_API UVideoSettingButton : public UBSButton
{
	GENERATED_BODY()
	
public:
	/** Create functions like this with additional parameters to store info about the button being pressed. Remember to call SetHasSetDefaults in any child implementations */
	void SetDefaults(const EVideoSettingType InVideoSettingType, const uint8 VideoSettingQuality, UBSButton* NextButton = nullptr);

	/** Returns the video setting type for this button widget */
	EVideoSettingType GetVideoSettingType() const { return SettingType; }

	/** Returns the video setting quality for this button widget */
	uint8 GetVideoSettingQuality() const { return Quality; }
	
	virtual UVideoSettingButton* GetNext() const override { return Cast<UVideoSettingButton>(Next); }

private:
	/** The video setting quality this button widget represents */
	uint8 Quality;

	/** The video setting type this button widget represents */
	EVideoSettingType SettingType;
};
