// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TargetWidget.generated.h"

class UImage;

UCLASS()
class USERINTERFACE_API UTargetWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	
public:
	/** Change the scale of the target */
	void SetTargetScale(const FVector& NewScale) const;

	/** Sets the color of the Base Target */
	void SetTargetColor(const FLinearColor& Color) const;
	
	/** Sets the color of the Target Outline */
	void SetTargetOutlineColor(const FLinearColor& Color) const;

	void SetUseSeparateTargetOutlineColor(const float bUse) const;

	void SetTargetPosition(const FVector2d& InPosition) const;
	
protected:
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	UImage* TargetImage;

	UPROPERTY()
	UMaterialInstanceDynamic* TargetImageMaterial;
};
