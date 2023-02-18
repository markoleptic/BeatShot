// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "MovablePlatform.h"
#include "DefaultCharacter.h"
#include "WallMenu.h"
#include "DefaultPlayerController.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AMovablePlatform::AMovablePlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	Floor = CreateDefaultSubobject<UStaticMeshComponent>("Floor");
	Floor->SetupAttachment(TriggerVolume);
	RootComponent = Floor;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>("TriggerVolume");
	TriggerVolume->SetupAttachment(Floor);
	
	ControlBase = CreateDefaultSubobject<UStaticMeshComponent>("ControlBase");
	ControlBase->SetupAttachment(Floor);

	ControlBox = CreateDefaultSubobject<UStaticMeshComponent>("ControlBox");
	ControlBox->SetupAttachment(ControlBase);

	WallMenuComponent = CreateDefaultSubobject<UChildActorComponent>("WallMenuComponent");
	WallMenuComponent->SetupAttachment(Floor);
}

void AMovablePlatform::BeginPlay()
{
	Super::BeginPlay();
	if (ADefaultCharacter* Character = Cast<ADefaultCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		Character->OnInteractDelegate.BindUFunction(this, "MovePlatformUp");
		Character->OnShiftInteractDelegate.BindUFunction(this, "MovePlatformDown");
	}
	
	WallMenu = Cast<AWallMenu>(WallMenuComponent->GetChildActor());
	
	TriggerVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &AMovablePlatform::OnTriggerVolumeBeginOverlap);
	TriggerVolume->OnComponentEndOverlap.AddUniqueDynamic(this, &AMovablePlatform::OnTriggerVolumeEndOverlap);
}

void AMovablePlatform::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!bAllowPlatformMovement || !bSafeToChangeElevation)
	{
		return;
	}
	OnFloorElevationTimelineTick(DeltaSeconds);
}

void AMovablePlatform::MovePlatformUp(const int32 Stop)
{
	if (!bAllowPlatformMovement)
	{
		return;
	}
	TargetFloorHeight = MaxFloorHeight;
	switch (Stop)
	{
	case 0:
		bSafeToChangeElevation = true;
		break;
	default:
		bSafeToChangeElevation = false;
	}
}

void AMovablePlatform::MovePlatformDown(const int32 Stop)
{
	if (!bAllowPlatformMovement)
	{
		return;
	}
	TargetFloorHeight = MinFloorHeight;
	switch (Stop)
	{
	case 0:
		bSafeToChangeElevation = true;
		break;
	default:
		bSafeToChangeElevation = false;
	}
}

void AMovablePlatform::OnFloorElevationTimelineTick(const float Alpha)
{
	const FVector Location = UKismetMathLibrary::VInterpTo_Constant(Floor->GetComponentLocation(), TargetFloorHeight, Alpha, 75);
	Floor->SetWorldLocation(Location);
}

void AMovablePlatform::OnTriggerVolumeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ADefaultCharacter>(OtherActor))
	{
		Cast<ADefaultPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->ShowInteractInfo();
		bAllowPlatformMovement = true;
	}
}

void AMovablePlatform::OnTriggerVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<ADefaultCharacter>(OtherActor))
	{
		Cast<ADefaultPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->HideInteractInfo();
		bAllowPlatformMovement = false;
		bSafeToChangeElevation = false;
	}
}
