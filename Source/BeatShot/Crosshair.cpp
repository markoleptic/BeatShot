// Fill out your copyright notice in the Description page of Project Settings.


#include "Crosshair.h"


FLinearColor UCrosshair::HexToColor(const FString HexString)
{
	return FLinearColor(FColor::FromHex(HexString));
}

FString UCrosshair::ColorToHex(const FLinearColor Color)
{
	return Color.ToFColor(false).ToHex();
}
