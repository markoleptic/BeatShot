// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "RLAgentWidget.generated.h"

USTRUCT()
struct FAgentPoint
{
	GENERATED_BODY()
	
	UPROPERTY()
	UImage* Image;
	
	UPROPERTY()
	UTextBlock* Text;

	FAgentPoint()
	{
		Image = nullptr;
		Text = nullptr;
	}
	
	FAgentPoint(UImage* InImage, UTextBlock* InText)
	{
		Image = InImage;
		Text = InText;
	}
};

class UUniformGridPanel;
UCLASS()
class USERINTERFACE_API URLAgentWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UImage> DefaultImage;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UTextBlock> DefaultText;

public:

	UFUNCTION()
	void UpdatePanel(const TArray<float>& QTable);

	UFUNCTION()
	void UpdatePanel2(const TArray<float>& QTable);
	
	void InitQTable(const int32 Rows, const int32 Columns, const TArray<float>& QTable);

	void InitQTable2(const int32 Rows, const int32 Columns, const TArray<float>& QTable);

	FLinearColor InterpColor(const float Value) const;

	float MinValue = -0.5f;
	
	float MaxValue = 0.5f;

	float MinValue2 = -0.5f;
	
	float MaxValue2 = 0.5f;

	TArray<FAgentPoint> Points;

	TArray<FAgentPoint> Points2;

private:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UUniformGridPanel* GridPanel;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UUniformGridPanel* GridPanel2;
};


