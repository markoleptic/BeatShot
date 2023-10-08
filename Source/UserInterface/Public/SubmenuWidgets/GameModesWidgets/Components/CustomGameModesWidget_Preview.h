// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CustomGameModesWidgetComponent.h"
#include "Components/Overlay.h"
#include "WidgetComponents/GameModePreviewWidgets/BoxBoundsWidget.h"
#include "WidgetComponents/GameModePreviewWidgets/TargetWidget.h"
#include "CustomGameModesWidget_Preview.generated.h"

DECLARE_DELEGATE_RetVal(UTargetWidget*, FCreateTargetWidget);

UCLASS()
class USERINTERFACE_API UCustomGameModesWidget_Preview : public UCustomGameModesWidgetComponent
{
	GENERATED_BODY()

public:
	virtual void InitComponent(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext) override;

	void SetBoxBounds_Current(const FVector2d& InBounds, const float VerticalOffset);

	void SetBoxBounds_Max(const FVector2d& InBounds, const float VerticalOffset);

	void SetBoxBounds_Min(const FVector2d& InBounds, const float VerticalOffset);

	void SetBoxBoundsVisibility_Max(const ESlateVisibility& InVisibility);

	void SetBoxBoundsVisibility_Min(const ESlateVisibility& InVisibility);

	void SetStaticBoundsHeight(const float InHeight);

	void SetFloorDistanceHeight(const float InHeight);

	void SetText_FloorDistance(const FText& InText);

	UFUNCTION()
	UTargetWidget* ConstructTargetWidget();

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UBoxBoundsWidget* BoxBounds_Current;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UBoxBoundsWidget* BoxBounds_Max;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UBoxBoundsWidget* BoxBounds_Min;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	USizeBox* StaticBounds;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	USizeBox* FloorDistance;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* TextBlock_FloorDistance;

	/** Shows or hides the BoxBounds Current, Min, Max and TextBlock_GameModePreviewAvailability */
	void ToggleGameModePreview(const bool bEnable);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_RefreshPreview;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_Create;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UTargetWidget> TargetWidgetClass;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UOverlay* Overlay;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* TextBlock_GameModePreviewAvailability;
};
