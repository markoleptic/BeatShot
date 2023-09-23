// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BSCarouselNavBar.generated.h"

class UButtonNotificationWidget;
class UCommonWidgetCarousel;
class UBSButton;

UCLASS()
class USERINTERFACE_API UBSCarouselNavBar : public UWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CarouselNavBar")
	TSubclassOf<UBSButton> ButtonWidgetType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CarouselNavBar")
	TSubclassOf<UButtonNotificationWidget> NotificationWidgetType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CarouselNavBar")
	FMargin ButtonPadding;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CarouselNavBar")
	FMargin NotificationWidgetContainerPadding;

	/**
	 * Establishes the Widget Carousel instance that this Nav Bar should interact with
	 * @param CommonCarousel The carousel that this nav bar should be associated with and manipulate
	 */
	UFUNCTION(BlueprintCallable, Category = "CarouselNavBar")
	void SetLinkedCarousel(UCommonWidgetCarousel* CommonCarousel);

	void SetNavButtonText(const TArray<FText>& InButtonText);

	void UpdateNotifications(const int32 Index, const int32 NumCautions, const int32 NumWarnings);

	// UWidget interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	// End of UWidget interface

protected:
	UFUNCTION()
	void HandlePageChanged(UCommonWidgetCarousel* CommonCarousel, int32 PageIndex);

	UFUNCTION()
	void HandleButtonClicked(const UBSButton* AssociatedButton);

	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface
	
	void RebuildButtons();
	
	TSharedPtr<SHorizontalBox> MyContainer;
	
	UPROPERTY()
	TObjectPtr<UCommonWidgetCarousel> LinkedCarousel;

	UPROPERTY()
	TArray<TObjectPtr<UBSButton>> Buttons;

	TArray<TObjectPtr<UButtonNotificationWidget>> Notifications;

	TArray<FText> ButtonText;
};
