// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SongMetadata.generated.h"

USTRUCT(BlueprintType)
struct FSongMetadata
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Title;

	UPROPERTY(BlueprintReadOnly)
	FString Artist;

	UPROPERTY(BlueprintReadOnly)
	FString Album;

	UPROPERTY(BlueprintReadOnly)
	FString Year;

	UPROPERTY(BlueprintReadOnly)
	FString Genre;

	UPROPERTY(BlueprintReadOnly)
	float Length = 0.f;

	bool operator==(const FSongMetadata& Other) const
	{
		return Title == Other.Title && Artist == Other.Artist && Album == Other.Album && Year == Other.Year && Genre ==
		       Other.Genre && FMath::IsNearlyEqual(Length, Other.Length);
	}

	bool IsEqualExcludingLength(const FSongMetadata& Other) const
	{
		return Title == Other.Title && Artist == Other.Artist && Album == Other.Album && Year == Other.Year && Genre ==
		       Other.Genre;
	}

	bool IsValid() const
	{
		return !Title.IsEmpty();
	}
};
