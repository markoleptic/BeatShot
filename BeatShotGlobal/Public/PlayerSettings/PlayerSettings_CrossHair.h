// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSConstants.h"
#include "PlayerSettings_CrossHair.generated.h"

/** CrossHair settings. */
USTRUCT(BlueprintType)
struct BEATSHOTGLOBAL_API FPlayerSettings_CrossHair
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 LineWidth;

	UPROPERTY(BlueprintReadOnly)
	int32 LineLength;

	UPROPERTY(BlueprintReadOnly)
	int32 InnerOffset;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor CrossHairColor;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor OutlineColor;

	UPROPERTY(BlueprintReadOnly)
	int32 OutlineSize;

	UPROPERTY(BlueprintReadOnly)
	bool bShowCrossHairDot;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor CrossHairDotColor;

	UPROPERTY(BlueprintReadOnly)
	int32 CrossHairDotSize;

	FPlayerSettings_CrossHair()
	{
		LineWidth = Constants::DefaultLineWidth;
		LineLength = Constants::DefaultLineLength;
		InnerOffset = Constants::DefaultInnerOffset;
		CrossHairColor = Constants::DefaultCrossHairColor;

		OutlineColor = Constants::DefaultCrossHairOutlineColor;
		OutlineSize = Constants::DefaultOutlineSize;

		bShowCrossHairDot = false;
		CrossHairDotColor = Constants::DefaultCrossHairColor;
		CrossHairDotSize = Constants::DefaultCrossHairDotSize;
	}
};
