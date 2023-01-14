// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Delegates/DelegateCombinations.h"
#include "AudioSelectWidget.generated.h"

class UVerticalBox;
class UButton;
class UTextBlock;
class UEditableTextBox;
class UWidgetAnimation;
DECLARE_DELEGATE_ThreeParams(FOnStartButtonClicked, const bool bShowOpenFileDialog, const FString SongTitle, const int32 SongLength)

/**
 * 
 */
UCLASS()
class USERINTERFACE_API UAudioSelectWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UFUNCTION()
	void FadeIn();
	UFUNCTION()
	void FadeOut();

	FOnStartButtonClicked OnStartButtonClickedDelegate;

protected:
	FWidgetAnimationDynamicEvent FadeOutDelegate;

	UFUNCTION()
	void OnFadeOutFinish();
	UFUNCTION()
	void OnAudioFromFileButtonClicked();
	UFUNCTION()
	void OnLoopbackAudioButtonClicked();
	UFUNCTION()
	void OnStartButtonClicked();
	UFUNCTION()
	void OnSongTitleValueCommitted(const FText& NewSongTitle, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnMinutesValueCommitted(const FText& NewMinutes, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnSecondsValueCommitted(const FText& NewSeconds, ETextCommit::Type CommitType);
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* AudioFromFileButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* LoopbackAudioButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* StartButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* LoopbackAudioBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* SongTitleText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Minutes;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Seconds;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutAnim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInAnim;

	FNumberFormattingOptions NumberFormattingOptions;
	/** The color used to change the GameModeButton color to when selected */
	const FLinearColor BeatShotBlue = FLinearColor(0.049707, 0.571125, 0.83077, 1.0);
	/** The color used to change the GameModeButton color to when not selected */
	const FLinearColor White = FLinearColor::White;
	bool bShowOpenFileDialog = false;
	int32 SongLength = 0;
	FString SongTitle = "";
};
