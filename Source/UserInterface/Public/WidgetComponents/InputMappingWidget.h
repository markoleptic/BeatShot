// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedActionKeyMapping.h"
#include "InputMappingWidget.generated.h"

class UTextBlock;
class UInputKeySelector;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnKeySelected, const FName MappingName, const FInputChord SelectedKey);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsSelectingKey, UInputKeySelector* KeySelector);

UCLASS()
class USERINTERFACE_API UInputMappingWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	void Init(const TArray<FEnhancedActionKeyMapping>& Mappings);

	/** Broadcast when a new Key has been selected from either of the InputKeySelectors */
	FOnKeySelected OnKeySelected;

	/** Broadcast whenever the key selection mode starts or stops for either InputKeySelectors */
	FOnIsSelectingKey OnIsSelectingKey;

	/** Returns the array of Enhanced Action Key Mappings that this widget represents */
	TArray<FEnhancedActionKeyMapping> GetActionKeyMappings() const { return ActionKeyMappings; }

	/** Returns the FName of the Key that one of the InputKeySelectors represents, or NAME_None */
	FName GetMappingNameForKey(const FKey InKey);

	/** Calls SetSelectedKey with NewKey on the InputKeySelector with the matching OldKey */
	void SetKey(const FName MappingName, const FKey NewKey);

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
