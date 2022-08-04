// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "PlayerHUD.generated.h"
class ADefaultCharacter;
class UTargetSubsystem;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTargetSpawned);
//class UEditableText;
/**
 * 
 */
class UProgressBar;
class UTextBlock;
UCLASS()
class BEATAIM_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	UPlayerHUD(const FObjectInitializer& ObjectInitializer);

	void SetTargetBar(float TargetsHit, float ShotsFired);

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

	UPROPERTY(EditAnywhere, Category = "Player Stats")
		float TotalTargetsSpawned;

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void SetTargetsHit(float TargetsHit);

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void SetAccuracy(float TargetsHit, float ShotsFired);

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void SetShotsFired(float ShotsFired);

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void SetTargetsSpawned(float TargetsSpawned);

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void ActOnTargetsSpawned();

	UPROPERTY(BlueprintCallable)
		FTargetSpawned OnTargetSpawned;

	ADefaultCharacter* DefaultCharacter;
	UTargetSubsystem* TargetSubsystem;
	UGameInstance* GI;
};

