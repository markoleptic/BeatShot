// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorBase.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "PlayerHUD.generated.h"

class UHorizontalBox;
class ABeatAimGameModeBase;
class UDefaultGameInstance;
class ADefaultCharacter;
class UProgressBar;
class UTextBlock;
/**
 * 
 */
UCLASS()
class BEATSHOT_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
	UProgressBar* TargetBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
	UTextBlock* Accuracy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
	UTextBlock* TargetsHitText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
	UTextBlock* ShotsFiredText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
	UTextBlock* TargetsSpawnedText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
	UTextBlock* CurrentScoreText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
	UTextBlock* HighScoreText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
	UTextBlock* GameModeNameText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
	UTextBlock* SongTitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
	UTextBlock* TotalSongLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
	UTextBlock* CurrentStreakBestText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
	UHorizontalBox* TargetsSpawnedBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
	UHorizontalBox* StreakBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
	UHorizontalBox* TargetsHitBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
	UHorizontalBox* ShotsFiredBox;

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
	void UpdateAllElements(FPlayerScore NewPlayerScoreStruct);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	UDefaultGameInstance* GI;
};
