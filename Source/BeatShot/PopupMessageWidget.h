// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PopupMessageWidget.generated.h"

class UButton;
class UTextBlock;

/**
 * 
 */
UCLASS()
class BEATSHOT_API UPopupMessageWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

public:
	UFUNCTION(BlueprintCallable)
	void InitPopup(FString TitleInput, FString MessageInput, FString Button1TextInput, FString Button2TextInput = "") const;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Component Parameters")
	UButton* Button1;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Component Parameters")
	UButton* Button2;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Component Parameters")
	UTextBlock* Button1Text;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Component Parameters")
	UTextBlock* Button2Text;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Component Parameters")
	UTextBlock* TitleText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Component Parameters")
	UTextBlock* MessageText;
};
