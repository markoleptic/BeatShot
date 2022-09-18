// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "GameModeActorStruct.h"
#include "PlayerHUD.generated.h"

struct FPlayerScore;
class ABeatAimGameModeBase;
class UDefaultGameInstance;
class ADefaultCharacter;
class UTargetSubsystem;
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

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void UpdateAllElements(FPlayerScore NewPlayerScoreStruct);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
		class UDefaultGameInstance* GI;
};

