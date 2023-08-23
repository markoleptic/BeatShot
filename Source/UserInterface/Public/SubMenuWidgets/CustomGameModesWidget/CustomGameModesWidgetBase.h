// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalStructs.h"
#include "SaveLoadInterface.h"
#include "Blueprint/UserWidget.h"
#include "SubMenuWidgets/GameModesWidget.h"
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
	/** Sets the value of BSConfig and GameModeDataAsset. Calls InitComponent on all widgets in ChildWidgets array */
	virtual void Init(FBSConfig* InConfig, const TObjectPtr<UBSGameModeDataAsset> InGameModeDataAsset);

	/** Calls UpdateOptionsFromConfig on all widgets in ChildWidgets array */
	UFUNCTION()
	void Update();

	/** Returns the NewCustomGameModeName from Widget_Start */
	FString GetNewCustomGameModeName() const;
	
	/** Sets the value of NewCustomGameModeName in Widget_Start */
	void SetNewCustomGameModeName(const FString& InCustomGameModeName) const;

	/** Returns the options for a start widget since they're not all shared with BSConfig pointer */
	FStartWidgetProperties GetStartWidgetProperties() const;

	/** Sets the options for a start widget since they're not all shared with BSConfig pointer */
	void SetStartWidgetProperties(const FStartWidgetProperties& InProperties);

	/** Returns whether or not all child widget custom game mode options are valid. Iterates through ChildWidgetValidityMap */
	bool GetAllChildWidgetOptionsValid() const;

	/** Clears all GameModeTemplate options and repopulates */
	void RefreshGameModeTemplateOptions() const;
	
	/** Broadcast any time Widget_Start broadcasts their RequestGameModeTemplateUpdate delegate */
	FRequestGameModeTemplateUpdate RequestGameModeTemplateUpdate;

	/** Broadcast when Widget_Start's OnValidOptionsStateChanged is changed */
	FRequestButtonStateUpdate RequestButtonStateUpdate;

protected:
	/** Adds child widgets to ChildWidgets array, binds to Widget_Start's RequestGameModeTemplateUpdate */
	virtual void NativeConstruct() override;

	/** Override to provide widget with access to FindPresetGameMode functions from ISaveLoadInterface */
	virtual UBSGameModeDataAsset* GetGameModeDataAsset() const override;

	/** Called any time a widget in ChildWidgets broadcasts their RequestGameModeTemplateUpdate delegate */
	virtual void OnRequestGameModeTemplateUpdate(const FString& InGameMode, const EGameModeDifficulty& Difficulty);
	
	/** Bound to all child widget's OnValidOptionsStateChanged delegates */
	virtual void OnValidOptionsStateChanged(const TObjectPtr<UCustomGameModesWidgetComponent> Widget, const bool bAllOptionsValid);

	/** Bound to all child widget's RequestComponentUpdate delegates */
	void OnRequestComponentUpdate();
	
	/** Adds a widget to ChildWidgets array. Binds to its RequestComponentUpdate and OnValidOptionsStateChanged delegates */
	void AddChildWidget(const TObjectPtr<UCustomGameModesWidgetComponent> Component);
	
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

	/** Pointer to Game Mode Config held in controlling GameModesWidget*/
	FBSConfig* CurrentConfigPtr;

	/** Maps each child widget to a boolean value representing if all its custom game mode options are valid */
	TMap<TObjectPtr<UCustomGameModesWidgetComponent>, bool> ChildWidgetValidityMap;

	bool bIsUpdatingFromComponentRequest = false;
	bool bShouldUpdateFromComponentRequest = false;
};
