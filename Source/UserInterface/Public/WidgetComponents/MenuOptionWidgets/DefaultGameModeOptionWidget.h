// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSGameModeDataAsset.h"
#include "MenuOptionWidget.h"
#include "DefaultGameModeOptionWidget.generated.h"

class UCommonTextBlock;
class UBSButton;
UCLASS()
class USERINTERFACE_API UDefaultGameModeOptionWidget : public UMenuOptionWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	virtual void SetStyling() override;
	
public:
	EBaseGameMode GetBaseGameMode() const { return BaseGameMode; }

	void SetBaseGameMode(const EBaseGameMode InBaseGameMode) { BaseGameMode = InBaseGameMode; }

	void SetAltDescriptionText(const FText& Text);
	
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	UBSButton* Button;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "DefaultGameModeOptionWidget")
	EBaseGameMode BaseGameMode;
	
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	UCommonTextBlock* AltDescriptionText;
};
