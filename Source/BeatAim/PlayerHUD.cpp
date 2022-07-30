// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "Components/ProgressBar.h"

void UPlayerHUD::SetTargetBar(float TargetsHit)
{
	TargetBar->SetPercent(TargetsHit / 100);
}

FText UPlayerHUD::GetTargetsHit() const
{
	return TargetsHit;
}

void UPlayerHUD::SetTargetsHit(const FText& NewTargetsHit)
{
	TargetsHit = NewTargetsHit;
}
