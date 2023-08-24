// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BoxBoundsWidget.generated.h"

class USizeBox;
class UImage;

UCLASS()
class USERINTERFACE_API UBoxBoundsWidget : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	
public:
	void SetBoxBounds(const FVector2d& InBounds) const;
	
protected:
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	USizeBox* BoxBounds;
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	UImage* BoxBoundsImage;
};
