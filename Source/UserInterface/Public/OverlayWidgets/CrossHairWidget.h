// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveGamePlayerSettings.h"
#include "SaveLoadInterface.h"
#include "CrossHairWidget.generated.h"

class UImage;
class UScaleBox;

UCLASS()
class USERINTERFACE_API UCrossHairWidget : public UUserWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

public:

	void InitializeCrossHair(const FPlayerSettings_CrossHair& CrossHairSettings);

protected:

	virtual void NativeConstruct() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScaleBox* LeftContainer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScaleBox* TopContainer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScaleBox* RightContainer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScaleBox* BottomContainer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScaleBox* LeftScaleBox;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScaleBox* TopScaleBox;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScaleBox* RightScaleBox;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScaleBox* BottomScaleBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UImage* LeftOutlineImage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UImage* TopOutlineImage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UImage* RightOutlineImage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UImage* BottomOutlineImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UImage* LeftImage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UImage* TopImage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UImage* RightImage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UImage* BottomImage;

public:
	
	/** Changes the width of the containers */
	void SetLineWidth(const int32 NewWidthValue);
	/** Changes the length of the containers */
	void SetLineLength(const float NewLengthValue);
	/** Changes the size of the colored portion of the CrossHair (LeftScaleBox, etc.) while leaving the outline unchanged */
	void SetOutlineWidth(const int32 NewWidthValue);
	/** Changes the opacity of the Outline Images */
	void SetOutlineOpacity(float NewOpacityValue);
	/** Changes the color of the images (LeftImage, etc.) while leaving the OutlineImages unchanged */
	void SetImageColor(const FLinearColor NewColor);
	/** Changes the offset between the middle of the screen and the start of a container */
	void SetInnerOffset(const int32 NewOffsetValue);
	/** Reinitialize CrossHair if Player Settings change */
	UFUNCTION()
	void OnPlayerSettingsChange(const FPlayerSettings& PlayerSettings);
};


