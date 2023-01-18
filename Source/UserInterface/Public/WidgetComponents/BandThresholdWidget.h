// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BandThresholdWidget.generated.h"


DECLARE_DELEGATE_ThreeParams(FOnThresholdValueCommitted, const UBandThresholdWidget* BandThreshold, const int32 Index,
                             const float NewValue);
class UBorder;
class USlider;
class UEditableTextBox;
class UTextBlock;

UCLASS()
class USERINTERFACE_API UBandThresholdWidget : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

public:
	UPROPERTY()
	UBandThresholdWidget* Next = nullptr;

	FOnThresholdValueCommitted OnThresholdValueCommitted;

	void SetDefaultValue(const float Value, const int32 ChannelIndex);

	int32 Index;
	
protected:
	UFUNCTION()
	void OnValueCommitted(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnSliderChanged(const float NewValue);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	USlider* ThresholdSlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* ThresholdValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ChannelText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UBorder* LeftBorder;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UBorder* RightBorder;
};
