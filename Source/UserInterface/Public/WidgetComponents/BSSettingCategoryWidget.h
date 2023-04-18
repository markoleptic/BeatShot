// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TooltipInterface.h"
#include "BSVerticalBox.h"
#include "Blueprint/UserWidget.h"
#include "BSSettingCategoryWidget.generated.h"

/** Base Setting Category widget, with a BSVerticalBox acting as the main container */
UCLASS()
class USERINTERFACE_API UBSSettingCategoryWidget : public UUserWidget, public ITooltipInterface
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	/** Override this function to set up more specific settings, such as calling AddWidgetBoxPair inside it */
	virtual void InitSettingCategoryWidget();

	/** Overriden from TooltipInterface, */
	virtual UTooltipWidget* ConstructTooltipWidget() override;

	/** Adds a widget-box pair to the WidgetBoxMap. Any widget-box pairs will be called when updating
	 *  the background colors so that other child widgets that contain BSVerticalBoxes will match this one */
	void AddWidgetBoxPair(UWidget* InWidget, UBSVerticalBox* InBox);
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSVerticalBox* MainContainer;
	UPROPERTY(EditDefaultsOnly, Category = "Classes | Tooltip")
	TSubclassOf<UTooltipWidget> TooltipWidgetClass;
	
public:
	/** Calls UpdateBrushColors on the MainContainer which iterates through the widget tree and alternates the Brush Color of each widget */
	void UpdateBrushColors() const;

	/** Returns the BSVerticalBox, the MainContainer for this widget */
	UBSVerticalBox* GetMainContainer() const { return MainContainer; }
};
