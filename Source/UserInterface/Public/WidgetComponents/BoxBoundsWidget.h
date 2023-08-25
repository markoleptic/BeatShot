// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BoxBoundsWidget.generated.h"

class USizeBox;
class UImage;

/** Represents the BoxBounds of a game mode, to be used with CustomGameModesWidget_Preview */
UCLASS()
class USERINTERFACE_API UBoxBoundsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Sets the Width and Height overrides of the BoxBounds, which adjust the BoxBoundsImage size */
	void SetBoxBounds(const FVector2d& InBounds) const;
	
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	USizeBox* BoxBounds;
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	UImage* BoxBoundsImage;
};
