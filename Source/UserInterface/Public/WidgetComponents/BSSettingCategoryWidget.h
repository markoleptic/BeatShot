// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BSSettingCategoryWidget.generated.h"

class UBSVerticalBox;

/** Base Setting Category widget, with a BSVerticalBox acting as the main container */
UCLASS()
class USERINTERFACE_API UBSSettingCategoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateBrushColors() const;
	
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSVerticalBox* MainContainer;
};
