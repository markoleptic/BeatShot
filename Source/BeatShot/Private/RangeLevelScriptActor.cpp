// Fill out your copyright notice in the Description page of Project Settings.


#include "RangeLevelScriptActor.h"

#include "DefaultGameMode.h"
#include "Components/StaticMeshComponent.h"
#include "Moon.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/LightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/VolumetricCloudComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/RectLight.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ARangeLevelScriptActor::ARangeLevelScriptActor()
{

}

void ARangeLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();
	Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnStreakUpdate.AddDynamic(this, &ARangeLevelScriptActor::OnStreakUpdated);
}

void ARangeLevelScriptActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bStreakActive && !bMoonPositionReached)
	{
		TransitionToNight(DeltaSeconds);
	}
}

void ARangeLevelScriptActor::OnStreakUpdated(const int32 NewStreak, const FVector Position)
{
	if (bStreakActive)
	{
		return;
	}
	if (NewStreak > StreakThreshold)
	{
		BeginNightTransition();
	}
}

void ARangeLevelScriptActor::BeginNightTransition()
{
	UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(SkySphere->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	StarrySkyMatDynamic = Mesh->CreateDynamicMaterialInstance(0, StarrySkyMat, "StarrySky");
	Mesh->SetMaterial(0, StarrySkyMatDynamic);
	MoonMatDynamic = Moon->MoonMesh->CreateDynamicMaterialInstance(0, MoonMaterial, "MoonMatDynamic");
	VolumetricCloud->Destroy();
	bStreakActive = true;
}

void ARangeLevelScriptActor::RotateSun()
{
}

void ARangeLevelScriptActor::RotateMoon()
{
}

void ARangeLevelScriptActor::TransitionToNight(float DeltaTime)
{
	//if (Moon->SphereComponent->GetRelativeRotation().Roll >= 179)
	//{
	//	bMoonPositionReached = true;
	//	return;
	//}

	//Daylight->GetLightComponent()->AddLocalRotation(FRotator(0, DeltaTime * CycleSpeed, 0));
	//Moon->SphereComponent->AddLocalRotation(FRotator(0, - DeltaTime * CycleSpeed, 0));
	//Moon->MoonLight->SetRelativeRotation(UKismetMathLibrary::FindLookAtRotation(Moon->MoonLight->GetComponentLocation(), FVector::Zero()));
	//float Value = 0.f;
	//float DaylightIntensity = 0.f;
	//if (Daylight->GetActorRotation().Yaw > 90)
	//{
		//MoonMatDynamic->SetScalarParameterValue("Opacity", Value);
		//TargetSpawnerLight->GetLightComponent()->SetIntensity(DaylightIntensity);
	//}
	//else if (Daylight->GetActorRotation().Yaw > -130)
	//{
	//}
	
	if (Moon->SphereComponent->GetRelativeRotation() == EndMoonRotation)
	{
		bMoonPositionReached = true;
		return;
	}
	const FRotator NewMoonlightRotation = UKismetMathLibrary::RInterpTo(Moon->SphereComponent->GetRelativeRotation(),
	                                                                   EndMoonRotation, DeltaTime, 0.2);
	Moon->SphereComponent->SetRelativeRotation(NewMoonlightRotation);
	Moon->MoonLight->SetRelativeRotation(UKismetMathLibrary::FindLookAtRotation(Moon->MoonLight->GetComponentLocation(), FVector::Zero()));
	float Value = UKismetMathLibrary::FInterpTo(TargetSpawnerLight->GetLightComponent()->Intensity, 1, DeltaTime, 0.2);
	MoonMatDynamic->SetScalarParameterValue("Opacity", Value);
	TargetSpawnerLight->GetLightComponent()->SetIntensity(Value);
	const FRotator NewSunlightRotation = UKismetMathLibrary::RInterpTo(Daylight->GetLightComponent()->GetRelativeRotation(),
		EndSunRotation, DeltaTime, 0.2);
	Daylight->GetLightComponent()->SetRelativeRotation(NewSunlightRotation);
	RefreshSkySphereMaterial();
}

