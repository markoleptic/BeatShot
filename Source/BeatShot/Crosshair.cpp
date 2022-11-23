// Fill out your copyright notice in the Description page of Project Settings.


#include "Crosshair.h"


FLinearColor UCrosshair::HexToColor(FString HexString)
{
	return FLinearColor(FColor::FromHex(HexString));
}

FString UCrosshair::ColorToHex(FLinearColor Color)
{
	return Color.ToFColor(false).ToHex();
}
