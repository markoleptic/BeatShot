// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveLoadInterface.h"
#include "CrossHairWidget.generated.h"

class USizeBox;
class UImage;
class UScaleBox;

/** Widget used to show the CrossHair */
UCLASS()
class USERINTERFACE_API UCrossHairWidget : public UUserWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

public:
	void InitializeCrossHair(const FPlayerSettings_CrossHair& CrossHairSettings);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScaleBox* Container_Left;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScaleBox* Container_Top;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScaleBox* Container_Right;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScaleBox* Container_Bottom;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScaleBox* ScaleBox_Left;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScaleBox* ScaleBox_Top;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScaleBox* ScaleBox_Right;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScaleBox* ScaleBox_Bottom;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UImage* Image_LeftOutline;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UImage* Image_TopOutline;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UImage* Image_RightOutline;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UImage* Image_BottomOutline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UImage* Image_Left;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UImage* Image_Top;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UImage* Image_Right;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UImage* Image_Bottom;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USizeBox* SizeBox_CrossHairDot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UImage* Image_CrossHairDot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USizeBox* SizeBox_CrossHairDotOutline;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UImage* Image_CrossHairDotOutline;

public:
	/** Changes the width of the containers */
	void SetLineWidth(const int32 NewWidthValue);
	
	/** Changes the length of the containers */
	void SetLineLength(const float NewLengthValue);
	
	/** Changes the size of the colored portion of the CrossHair (ScaleBox_Left, etc.) while leaving the outline unchanged */
	void SetOutlineWidth(const int32 NewWidthValue);
	
	/** Changes the opacity of the Outline Images */
	void SetOutlineOpacity(const float NewOpacityValue);
	
	/** Changes the color of the images (Image_Left, etc.) while leaving the OutlineImages unchanged */
	void SetImageColor(const FLinearColor NewColor);
	
	/** Changes the offset between the middle of the screen and the start of a container */
	void SetInnerOffset(const int32 NewOffsetValue);

	/** Changes the visibility of the CrossHair Dot */
	void SetShowCrossHairDot(const bool bShow);

	/** Changes the color of the CrossHair Dot */
	void SetCrossHairDotColor(const FLinearColor NewColor);

	/** Changes the size of the CrossHair Dot */
	void SetCrossHairDotSize(const int32 NewSize);
	
	/** Changes the size of the CrossHair Dot Outline */
	void SetCrossHairDotOutlineSize(const int32 CrossHairDotSize, const int32 OutlineWidth);

	/** Changes the opacity of the CrossHair Dot Outline */
	void SetCrossHairDotOutlineOpacity(const float NewOpacityValue);
	
	/** Reinitialize CrossHair if Player Settings change */
	virtual void OnPlayerSettingsChanged_CrossHair(const FPlayerSettings_CrossHair& CrossHairSettings) override;
};
