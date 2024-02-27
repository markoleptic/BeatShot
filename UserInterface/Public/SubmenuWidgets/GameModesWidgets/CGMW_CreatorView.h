// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CGMW_Base.h"
#include "CGMW_CreatorView.generated.h"

class UScrollBox;
class UBSCarouselNavBar;
class UCGMWC_Base;
class UCGMWC_Preview;
class UCommonWidgetCarousel;

UCLASS()
class USERINTERFACE_API UCGMW_CreatorView : public UCGMW_Base
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
	UCGMWC_Preview* Widget_Preview;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USavedTextWidget* SavedTextWidget_CreatorView;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UScrollBox* ScrollBox;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonWidgetCarousel* Carousel;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSCarouselNavBar* CarouselNavBar;
};
