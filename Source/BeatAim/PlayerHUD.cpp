// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include <string>

#include "SphereTarget.h"
#include "TargetSubsystem.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "kismet/GameplayStatics.h"

UPlayerHUD::UPlayerHUD(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
	GI = UGameplayStatics::GetGameInstance(this);
	TargetSubsystem = GI ? GI->GetSubsystem<UTargetSubsystem>() : nullptr;
}

void UPlayerHUD::SetTargetBar(float TargetsHit, float ShotsFired)
{
	TargetBar->SetPercent(TargetsHit / ShotsFired);
}

void UPlayerHUD::SetTargetsHit(float TargetsHit)
{
	TargetsHitText->SetText(FText::AsNumber(TargetsHit));
}

void UPlayerHUD::SetAccuracy(float TargetsHit, float ShotsFired)
{
	Accuracy->SetText(FText::AsPercent(TargetsHit / ShotsFired));
}

void UPlayerHUD::SetShotsFired(float ShotsFired)
{
	ShotsFiredText->SetText(FText::AsNumber(ShotsFired));
}

void UPlayerHUD::SetTargetsSpawned(float TargetsSpawned)
{
	//TargetSubsystem->SetTotalTargetsSpawned(TargetsSpawned);
	TargetsSpawnedText->SetText(FText::AsNumber(TargetsSpawned));
}

void UPlayerHUD::ActOnTargetsSpawned()
{
	
}

