// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CustomGameModesWidgetComponent.h"
#include "Components/Overlay.h"
#include "WidgetComponents/BoxBoundsWidget.h"
#include "WidgetComponents/TargetWidget.h"
#include "CustomGameModesWidget_Preview.generated.h"

DECLARE_DELEGATE_RetVal(UTargetWidget*, FCreateTargetWidget);

UCLASS()
class USERINTERFACE_API UCustomGameModesWidget_Preview : public UCustomGameModesWidgetComponent
{
	GENERATED_BODY()

public:
	virtual void InitComponent(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext) override;

	UFUNCTION()
	UTargetWidget* ConstructTargetWidget();

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UBoxBoundsWidget* BoxBounds;

protected:
	virtual void NativeConstruct() override;
	virtual bool UpdateAllOptionsValid() override;
	virtual void UpdateOptionsFromConfig() override;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UTargetWidget> TargetWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UOverlay* Overlay;
};
