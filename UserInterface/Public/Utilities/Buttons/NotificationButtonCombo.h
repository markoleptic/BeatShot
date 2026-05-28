// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSButton.h"
#include "NotificationButtonCombo.generated.h"

class USpacer;
class UOverlay;
class UTextBlock;

UCLASS()
class USERINTERFACE_API UNotificationButtonCombo : public UBSButton
{
	GENERATED_BODY()

public:
	void SetNumWarnings(const uint32 InNumWarnings);
	void SetNumCautions(const uint32 InNumCautions);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_NumWarnings;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_NumCautions;

	UPROPERTY(meta = (BindWidget))
	UOverlay* Overlay_Warnings;

	UPROPERTY(meta = (BindWidget))
	UOverlay* Overlay_Cautions;

	UPROPERTY(meta = (BindWidget))
	USpacer* Spacer_Mid;

	UPROPERTY(EditAnywhere, Category = "BSButton|Notification")
	float NotificationImageSpacing = 5.f;
};
