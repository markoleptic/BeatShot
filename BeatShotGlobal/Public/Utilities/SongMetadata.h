// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/TypeHash.h"
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

	/** Length is hashed in whole seconds so values that `IsNearlyEqual` treats as equal also hash equal. */
	friend FORCEINLINE uint32 GetTypeHash(const FSongMetadata& InSongMetadata)
	{
		uint32 KeyHash = GetTypeHash(InSongMetadata.Title);
		KeyHash = HashCombine(KeyHash, GetTypeHash(InSongMetadata.Artist));
		KeyHash = HashCombine(KeyHash, GetTypeHash(InSongMetadata.Album));
		KeyHash = HashCombine(KeyHash, GetTypeHash(InSongMetadata.Year));
		KeyHash = HashCombine(KeyHash, GetTypeHash(InSongMetadata.Genre));
		KeyHash = HashCombine(KeyHash, GetTypeHash(FMath::RoundToInt(InSongMetadata.Length)));
		return KeyHash;
	}
};
