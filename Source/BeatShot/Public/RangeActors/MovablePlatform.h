// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BeatShot/BeatShot.h"
#include "MovablePlatform.generated.h"

class AWallMenu;
class UBoxComponent;
class UWallMenuComponent;
class UChildActorComponent;

UCLASS()
class BEATSHOT_API AMovablePlatform : public AActor
{
	GENERATED_BODY()

	AMovablePlatform();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void MovePlatformUp(const int32 Stop);

	UFUNCTION()
	void MovePlatformDown(const int32 Stop);

	UFUNCTION()
	void InterpFloorElevation(const float DeltaSeconds);

	UFUNCTION()
	void OnTriggerVolumeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnCharacterStepOnFloor(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCharacterStepOffFloor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* Floor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* ControlBase;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* ControlBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AWallMenu> WallMenuClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* WallMenuComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UBoxComponent* ControlTriggerVolume;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UBoxComponent* FloorTriggerVolume;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AWallMenu* WallMenu;

	/** The movement state for the platform */
	EPlatformTransitionType PlatformTransitionType;

	/** Whether or not the player is is overlapping the ControlTriggerVolume */
	bool bPlayerIsOverlappingControl;

	/** Whether or not the player is overlapping the FloorTriggerVolume */
	bool bPlayerIsOverlappingFloor;
};
