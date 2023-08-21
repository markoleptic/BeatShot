// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalStructs.h"
#include "SaveLoadInterface.h"
#include "Blueprint/UserWidget.h"
#include "CustomGameModesWidgetBase.generated.h"

class UCustomGameModesWidgetComponent;
class UBSGameModeDataAsset;
class UCustomGameModesWidget_Target;
class UCustomGameModesWidget_Spawning;
class UCustomGameModesWidget_General;
class UCustomGameModesWidget_Deactivation;
class UCustomGameModesWidget_Activation;
class UCustomGameModesWidget_SpawnArea;
class UCustomGameModesWidget_Start;

DECLARE_MULTICAST_DELEGATE_TwoParams(FRequestGameModeTemplateUpdate, const FString& GameMode, const EGameModeDifficulty& Difficulty);

UCLASS()
class USERINTERFACE_API UCustomGameModesWidgetBase : public UUserWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

public:
	virtual void Init(FBSConfig* InConfig, const TObjectPtr<UBSGameModeDataAsset> InGameModeDataAsset);
	virtual void Update();

	FString GetNewCustomGameModeName() const;
	void SetNewCustomGameModeName(const FString& InCustomGameModeName) const;

	FRequestGameModeTemplateUpdate RequestGameModeTemplateUpdate;
	
	UFUNCTION()
	void UpdateAfterConfigChange();

protected:
	virtual void NativeConstruct() override;
	virtual UBSGameModeDataAsset* GetGameModeDataAsset() const override;
	virtual void OnRequestGameModeTemplateUpdate(const FString& InGameMode, const EGameModeDifficulty& Difficulty);
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCustomGameModesWidget_Start> Widget_Start;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCustomGameModesWidget_SpawnArea> Widget_SpawnArea;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCustomGameModesWidget_Activation> Widget_Activation;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCustomGameModesWidget_Deactivation> Widget_Deactivation;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCustomGameModesWidget_General> Widget_General;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCustomGameModesWidget_Spawning> Widget_Spawning;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCustomGameModesWidget_Target> Widget_Target;

	UPROPERTY()
	TObjectPtr<UBSGameModeDataAsset> GameModeDataAsset;

	/** Adds a CustomGameModesWidgetComponent to Component Update Array and binds to its RequestUpdateAfterConfigChange */
	void AddComponent(const TObjectPtr<UCustomGameModesWidgetComponent> Component);

	/** Pointer to Game Mode Config held in controlling GameModesWidget*/
	FBSConfig* CurrentConfigPtr;

	/** Pointer to all Widget Components */
	TArray<TObjectPtr<UCustomGameModesWidgetComponent>> Components;
};
