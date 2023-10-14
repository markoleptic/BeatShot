// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CustomGameModesWidgetBase.h"
#include "CustomGameModesWidget_CreatorView.generated.h"

class UScrollBox;
class UBSCarouselNavBar;
class UCustomGameModesWidgetComponent;
class UCustomGameModesWidget_Preview;
class UCommonWidgetCarousel;

UCLASS()
class USERINTERFACE_API UCustomGameModesWidget_CreatorView : public UCustomGameModesWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnCarouselWidgetIndexChanged(UCommonWidgetCarousel* InCarousel, const int32 NewIndex);

	/** Calls UpdateAllOptionsValid for each child widget. */
	virtual void UpdateAllChildWidgetOptionsValid() override;

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCustomGameModesWidget_Preview* Widget_Preview;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USavedTextWidget* SavedTextWidget_CreatorView;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UScrollBox* ScrollBox;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonWidgetCarousel* Carousel;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSCarouselNavBar* CarouselNavBar;
	UPROPERTY(EditDefaultsOnly, Category="Carousel")
	TArray<FText> NavBarButtonText;
};
