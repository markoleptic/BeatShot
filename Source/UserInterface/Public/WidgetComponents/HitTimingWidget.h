// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HitTimingWidget.generated.h"

class UTextBlock;
class UImage;
UCLASS()
class USERINTERFACE_API UHitTimingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateHitTiming(const float TimeOffsetNormalized, const float TimeOffsetRaw);

	void Init(const FText MinTickValue, const FText MaxTickValue);

protected:
	virtual void NativeConstruct() override;

	FText MakeCurrentTickText(const float TimeOffsetRaw) const;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Image_LogoTick;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Image_Background;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* Tick_Min;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* Tick_Mid;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* Tick_Max;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* Tick_Current;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicImageMaterial;
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* EaseInNewOffset;

	float LastTimeOffset = 0.5f;
	
};
