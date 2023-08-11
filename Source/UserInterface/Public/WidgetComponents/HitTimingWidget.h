// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HitTimingWidget.generated.h"

class UImage;
UCLASS()
class USERINTERFACE_API UHitTimingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateHitTiming(const float TimeOffsetNormalized);

protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Image_LogoTick;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Image_Background;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicImageMaterial;
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* EaseInNewOffset;

	float LastTimeOffset = 0.5f;
	
};
