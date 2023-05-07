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

/** Widget representing a Band Channel designed to be used with a linked list. Contains pointer to the next Band Channel widget */
UCLASS()
class USERINTERFACE_API UBandChannelWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	/** Executed when a new min or max channel value has been committed */
	FOnChannelValueCommitted OnChannelValueCommitted;

	/** Pointer to the next BandChannelWidget inside a collection of them */
	UPROPERTY()
	UBandChannelWidget* Next = nullptr;

	/** Sets the values and index for this BandChannelWidget */
	void SetDefaultValues(const FVector2d Values, const int32 ChannelIndex);

	/** The index of this BandChannelWidget inside a collection of them */
	int32 Index;

protected:
	UFUNCTION()
	void OnValueCommitted_Min(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnValueCommitted_Max(const FText& NewValue, ETextCommit::Type CommitType);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* Value_BandChannelMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* Value_BandChannelMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextBlock_Channel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UBorder* Border_ChannelText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UBorder* Border_Lower;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UBorder* Border_Upper;
};
