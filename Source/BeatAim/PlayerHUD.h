// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "PlayerHUD.generated.h"
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
class BEATAIM_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	/*UPlayerHUD(const FObjectInitializer& ObjectInitializer);*/

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "CountDown")
		UTextBlock* CountdownText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		UTextBlock* CurrentScoreText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		UTextBlock* HighScoreText;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
		float TotalTargetsSpawned;

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void SetTargetBar(float TargetsHit, float ShotsFired);

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void SetTargetsHit(float TargetsHit);

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void SetAccuracy(float TargetsHit, float ShotsFired);

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void SetShotsFired(float ShotsFired);

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void SetTargetsSpawned(float TargetsSpawned);

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void SetCurrentScore(float Score);

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void SetHighScore(float HighScore);
};

