// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Styles/LoadingScreenStyle.h"


FLoadingScreenStyle::FLoadingScreenStyle()
{
}

FLoadingScreenStyle::~FLoadingScreenStyle()
{
}

const FName FLoadingScreenStyle::TypeName(TEXT("LoadingScreen"));

const FLoadingScreenStyle& FLoadingScreenStyle::GetDefault()
{
	static FLoadingScreenStyle Default;
	return Default;
}

void FLoadingScreenStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
	OutBrushes.Add(&BackgroundImage);
	OutBrushes.Add(&LogoImageTexture);
	OutBrushes.Add(&LogoImage);
}
