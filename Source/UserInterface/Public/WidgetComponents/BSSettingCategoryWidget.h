// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSWidgetInterface.h"
#include "BSVerticalBox.h"
#include "Blueprint/UserWidget.h"
#include "BSSettingCategoryWidget.generated.h"

/** Base Setting Category widget, with a BSVerticalBox acting as the main container. The background colors of BSHorizontalBoxes inside the MainContainer are synced.
 *  Additional containers can be added, but will not be synced with MainContainer or each other */
UCLASS()
class USERINTERFACE_API UBSSettingCategoryWidget : public UUserWidget, public IBSWidgetInterface
{
	GENERATED_BODY()

public:
	/** Calls UpdateBrushColors on all BSVerticalBoxes in Containers array. For each box, it iterates through the widget tree and alternates the Brush Color of each widget */
	void UpdateBrushColors() const;

	/** Returns the BSVerticalBox, the MainContainer for this widget */
	UBSVerticalBox* GetMainContainer() const { return MainContainer; }
	
protected:
	virtual void NativeConstruct() override;

	/** Override this function to set up more specific settings, such as calling AddWidgetBoxPair inside it */
	virtual void InitSettingCategoryWidget();

	/** Overriden from TooltipInterface, creates the tooltip widget */
	virtual UTooltipWidget* ConstructTooltipWidget() override;

	/** Overriden from TooltipInterface, returns the tooltip widget */
	virtual UTooltipWidget* GetTooltipWidget() const override;

	/** Add another container separate from MainContainer that will also be called to update background colors,
	 *  but will sync with MainContainers, or with each other */
	void AddAdditionalContainers(const TArray<TObjectPtr<UBSVerticalBox>>& InContainers);

	/** Adds a widget-box pair to the WidgetBoxMap. Any widget-box pairs will be called when updating
	 *  the background colors so that other child widgets that contain BSVerticalBoxes will match this one */
	void AddWidgetBoxPair(UWidget* InWidget, UBSVerticalBox* InBox) const;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSVerticalBox* MainContainer;
	
	UPROPERTY(EditDefaultsOnly, Category = "Setting Category Widget | Tooltip")
	TSubclassOf<UTooltipWidget> TooltipWidgetClass;
	
	UPROPERTY()
	UTooltipWidget* ActiveTooltipWidget;

private:
	TArray<TObjectPtr<UBSVerticalBox>> Containers;
};
