// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Delegates/DelegateCombinations.h"
#include "BandChannelWidget.generated.h"

DECLARE_DELEGATE_FourParams(FOnChannelValueCommitted, const UBandChannelWidget* BandChannel, const int32 Index, const float NewValue, const bool bIsMinValue);
class UBorder;
class UEditableTextBox;
class UTextBlock;

UCLASS()
class USERINTERFACE_API UBandChannelWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	FOnChannelValueCommitted OnChannelValueCommitted;
	UPROPERTY()
	UBandChannelWidget* Next = nullptr;

	void SetDefaultValues(const FVector2d Values, const int32 ChannelIndex);

	int32 Index;

protected:
	UFUNCTION()
	void OnMinValueCommitted(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnMaxValueCommitted(const FText& NewValue, ETextCommit::Type CommitType);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* BandChannelMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* BandChannelMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ChannelText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UBorder* ChannelTextBorder;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UBorder* LowerBorder;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UBorder* UpperBorder;

	const float AbsoluteMin = 0;
	const float AbsoluteMax = 22720;
};
