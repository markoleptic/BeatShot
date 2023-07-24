// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedActionKeyMapping.h"
#include "InputMappingWidget.generated.h"

class UTextBlock;
class UInputKeySelector;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnKeySelected, const FName KeyName, const FInputChord SelectedKey);

UCLASS()
class USERINTERFACE_API UInputMappingWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	void Init(const TArray<FEnhancedActionKeyMapping>& Mappings);

	FOnKeySelected OnKeySelected;

protected:

	UFUNCTION()
	void OnIsSelectingKeyChanged_Slot1();

	UFUNCTION()
	void OnIsSelectingKeyChanged_Slot2();

	UFUNCTION()
	void OnKeySelected_Slot1(FInputChord SelectedKey);

	UFUNCTION()
	void OnKeySelected_Slot2(FInputChord SelectedKey);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_KeyDescription;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UInputKeySelector* InputKeySelectorSlot1;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UInputKeySelector* InputKeySelectorSlot2;

	TArray<FEnhancedActionKeyMapping> ActionKeyMappings;
};
