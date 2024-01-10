// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateWidgetStyle.h"
#include "Styling/SlateWidgetStyleContainerBase.h"
#include "LoadingScreenStyle.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct USERINTERFACE_API FLoadingScreenStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

	FLoadingScreenStyle();
	virtual ~FLoadingScreenStyle();

	// FSlateWidgetStyle
	virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;
	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };
	static const FLoadingScreenStyle& GetDefault();

	/** Background Image style */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	FSlateBrush BackgroundImage;
	FLoadingScreenStyle& SetBackgroundImage( const FSlateBrush& InBackgroundImage ){ BackgroundImage = InBackgroundImage; return *this; }

	/** Logo Image style */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	FSlateBrush LogoImage;
	FLoadingScreenStyle& SetLogoImage( const FSlateBrush& InLogoImage ){ LogoImage = InLogoImage; return *this; }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	FSlateFontInfo LogoFontLeft;
	FLoadingScreenStyle& SetLogoFontLeft( const FSlateFontInfo& InFont ){ LogoFontLeft = InFont; return *this; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	FSlateFontInfo LogoFontRight;
	FLoadingScreenStyle& SetLogoFontRight( const FSlateFontInfo& InFont ){ LogoFontRight = InFont; return *this; }
};

/**
 */
UCLASS(BlueprintType, hidecategories=Object, MinimalAPI)
class ULoadingScreenWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_BODY()

public:
	/** The actual data describing the widget appearance. */
	UPROPERTY(Category="Style", EditAnywhere, BlueprintReadWrite, meta=(ShowOnlyInnerProperties))
	FLoadingScreenStyle WidgetStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast<const struct FSlateWidgetStyle*>(&WidgetStyle);
	}
};
