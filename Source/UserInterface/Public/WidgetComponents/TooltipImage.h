// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TooltipImage.generated.h"

class UButton;
class UTooltipImage;

/** The type of tooltip image */
UENUM()
enum class ETooltipImageType : uint8
{
	Default UMETA(DisplayName="Default"),
	Caution UMETA(DisplayName="Caution"),
	Warning UMETA(DisplayName="Warning"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(ETooltipImageType, ETooltipImageType::Default, ETooltipImageType::Warning);

/** Contains data for the tooltip of a widget */
USTRUCT(BlueprintType)
struct FTooltipData
{
	GENERATED_BODY()

	/** Weak pointer to the tooltip this data is for */
	UPROPERTY()
	TWeakObjectPtr<UTooltipImage> TooltipImage;
	
	/** The text to display on the TooltipImage */
	FText TooltipText;

	/** Whether or not to allow Auto Wrap text */
	bool bAllowTextWrap;

	/** The type of tooltip */
	ETooltipImageType TooltipType;

	/** The String Table Key to fetch the TooltipText from */
	FString TooltipStringTableKey;

	/** Additional text to append to TooltipText */
	FText AdditionalTooltipText;
	
	FTooltipData()
	{
		TooltipImage = TWeakObjectPtr<UTooltipImage>(nullptr);
		TooltipText = FText();
		bAllowTextWrap = false;
		TooltipType = ETooltipImageType::Default;
		TooltipStringTableKey = FString();
		AdditionalTooltipText = FText();
	}

	FTooltipData(const FText& InTooltipText, const bool InbAllowTextWrap)
	{
		TooltipImage = TWeakObjectPtr<UTooltipImage>(nullptr);
		TooltipText = InTooltipText;
		bAllowTextWrap = InbAllowTextWrap;
		TooltipType = ETooltipImageType::Default;
		TooltipStringTableKey = FString();
		AdditionalTooltipText = FText();
	}

	FTooltipData(const FString& InStringTableKey, const ETooltipImageType& InType, const FText& InAdditionalTooltipText = FText())
	{
		TooltipImage = TWeakObjectPtr<UTooltipImage>(nullptr);
		TooltipText = FText();
		bAllowTextWrap = false;
		TooltipType = InType;
		TooltipStringTableKey = InStringTableKey;
		AdditionalTooltipText = InAdditionalTooltipText;
	}

	FORCEINLINE bool operator==(const FTooltipData& Other) const
	{
		if (!TooltipStringTableKey.Equals(Other.TooltipStringTableKey))
		{
			return false;
		}
		if (TooltipType != Other.TooltipType)
		{
			return false;
		}
		return true;
	}
	
	friend FORCEINLINE uint32 GetTypeHash(const FTooltipData& Value)
	{
		return HashCombine(HashCombine(GetTypeHash(Value.TooltipStringTableKey), GetTypeHash(Value.TooltipText.ToString())), GetTypeHash(Value.TooltipType));
	}
};

/** Data structure for dynamic tooltip text */
USTRUCT()
struct FDynamicTooltipData
{
	GENERATED_BODY()

	float MinAllowed;
	FString TooltipTextKey;
	FText FallbackText;
	FString TryChangeString;
	ETooltipImageType TooltipType;

	FDynamicTooltipData()
	{
		MinAllowed = 0.f;
		FallbackText = FText();
		TooltipTextKey = FString();
		TryChangeString = "Try lowering this value to <= ";
		TooltipType = ETooltipImageType::Default;
	}

	FDynamicTooltipData(const float InMin, const FString& InKey, const FText& InFallback, const ETooltipImageType& InTooltipImageType)
	{
		MinAllowed = InMin;
		TooltipTextKey = InKey;
		FallbackText = InFallback;
		TryChangeString = "Try lowering this value to <= ";
		TooltipType = InTooltipImageType;
	}

	// Adds an element to InTooltipData and returns true if Actual > MaxAllowed, otherwise no change to array and returns false
	bool UpdateArray(TArray<FTooltipData>& InTooltipData, const float InActual, const float InMaxAllowed) const
	{
		if (InActual > InMaxAllowed)
		{
			if (InMaxAllowed < MinAllowed)
			{
				InTooltipData.Emplace(TooltipTextKey, TooltipType, FallbackText);
			}
			else
			{
				InTooltipData.Emplace(TooltipTextKey, TooltipType, FText::FromString(TryChangeString + FString::FromInt(InMaxAllowed) + "."));
			}
			return true;
		}
		return false;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTooltipHovered, UTooltipImage*, TooltipImage, const FTooltipData&, TooltipData);

/** The image to draw on a TooltipWidget */
UCLASS()
class USERINTERFACE_API UTooltipImage : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void OnTooltipImageHoveredCallback();

	/** Sets the TooltipData that is accessed when the tooltip is hovered over */
	void SetupTooltipImage(const FText& InText, const bool bAllowTextWrap = false);

	/** Sets the TooltipData that is accessed when the tooltip is hovered over */
	void SetTooltipData(const FTooltipData& InTooltipData);

	/** Returns a pointer to the TooltipData */
	FTooltipData* GetTooltipData() { return &TooltipData; }

	/** Returns the OnTooltipHovered delegate */
	FOnTooltipHovered& GetTooltipHoveredDelegate() { return OnTooltipHovered; }

protected:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UButton* Button;
	
	/** Info about what this Tooltip image should display */
	UPROPERTY()
	FTooltipData TooltipData;

	/** Called when Button is hovered over, provides the tooltip data that should be displayed */
	FOnTooltipHovered OnTooltipHovered;
};
