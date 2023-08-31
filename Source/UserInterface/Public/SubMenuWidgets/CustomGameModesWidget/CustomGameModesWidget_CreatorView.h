// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CustomGameModesWidgetBase.h"
#include "Components/CustomGameModesWidget_Start.h"
#include "CustomGameModesWidget_CreatorView.generated.h"

class UScrollBox;
class UBSCarouselNavBar;
class UCommonWidgetCarouselNavBar;
class UCustomGameModesWidget_Target;
class UCustomGameModesWidget_Spawning;
class UCustomGameModesWidget_General;
class UCustomGameModesWidget_Deactivation;
class UCustomGameModesWidget_Activation;
class UCustomGameModesWidget_SpawnArea;
class UCustomGameModesWidgetComponent;
class UCustomGameModesWidget_Preview;
class UBSButton;
class UCustomGameModesWidget_Start;
class UEditableTextBoxOptionWidget;
class UCheckBoxOptionWidget;
class UComboBoxOptionWidget;
class UCommonWidgetCarousel;

UCLASS()
class USERINTERFACE_API UCustomGameModesWidget_CreatorView : public UCustomGameModesWidgetBase
{
	GENERATED_BODY()

public:
	virtual void Init(FBSConfig* InConfig, const TObjectPtr<UBSGameModeDataAsset> InGameModeDataAsset) override;
	
protected:
	virtual void NativeConstruct() override;

	/** Bound to all child widget's OnValidOptionsStateChanged delegates */
	virtual void OnValidOptionsStateChanged(const TObjectPtr<UCustomGameModesWidgetComponent> Widget, const bool bAllOptionsValid) override;

	UFUNCTION()
	void OnCarouselWidgetIndexChanged(UCommonWidgetCarousel* InCarousel, const int32 NewIndex);

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
