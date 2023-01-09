// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "SavedTextWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class BEATSHOT_API USavedTextWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* SavedText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInFadeOut;

public:
	
	UFUNCTION()
	void PlayFadeInFadeOut() { PlayAnimationForward(FadeInFadeOut); }

	void SetSavedText(const FText& Text) {SavedText->SetText(Text); }
};
