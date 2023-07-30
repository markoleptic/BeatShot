// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "GameFramework/Actor.h"
#include "BSKnife.generated.h"

UCLASS()
class BEATSHOT_API ABSKnife : public AActor, public ISaveLoadInterface
{
	GENERATED_BODY()
	
public:
	ABSKnife();
	
protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/** ~ISaveLoadInterface begin */
	virtual void OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings) override;
	/** ~ISaveLoadInterface end */
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Knife;

	/** Sets whether or not the weapon mesh is visible, updating its GameplayTags */
	UFUNCTION(BlueprintCallable)
	void SetShowKnifeMesh(const bool bShow);
};
