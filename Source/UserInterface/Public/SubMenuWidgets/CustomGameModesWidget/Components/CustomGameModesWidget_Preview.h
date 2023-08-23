// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CustomGameModesWidgetComponent.h"
#include "CustomGameModesWidget_Preview.generated.h"

UCLASS()
class USERINTERFACE_API UCustomGameModesWidget_Preview : public UCustomGameModesWidgetComponent
{
	GENERATED_BODY()

public:
	virtual void InitComponent(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext) override;

protected:
	virtual void NativeConstruct() override;
	virtual bool UpdateAllOptionsValid() override;
	virtual void UpdateOptionsFromConfig() override;

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateImages(const FBSConfig& InConfig);
};
