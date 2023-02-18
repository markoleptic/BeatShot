// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovablePlatform.generated.h"

class UBoxComponent;
class UWallMenuComponent;

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
	void OnFloorElevationTimelineTick(const float Alpha);

	UFUNCTION()
	void OnTriggerVolumeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* Floor;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* ControlBase;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* ControlBox;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UWallMenuComponent* WallMenuComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UBoxComponent* TriggerVolume;

	/** The height to interpolate to */
	FVector TargetFloorHeight;

	/** Whether or not the player is in collision volume and pressing an Interact key, or not */
	bool bSafeToChangeElevation;

	/** Whether or not the player is pressing an Interact key or not */
	bool bAllowPlatformMovement;
	
	const FVector MaxFloorHeight = {-4000, 0, 500};

	const FVector MinFloorHeight = {-4000, 0, 0};
};

