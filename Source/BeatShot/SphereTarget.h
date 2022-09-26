// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "SphereTarget.generated.h"

class UHealthComponent;
class UCapsuleComponent;
class UNiagaraSystem;
class UCurveFloat;
class UCurveLinearColor;

UCLASS()
class BEATSHOT_API ASphereTarget : public AActor
{
	GENERATED_BODY()

public:	
	ASphereTarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "Target Properties", BlueprintReadOnly)
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(VisibleAnywhere, Category = "Target Properties", BlueprintReadOnly)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, Category = "Target Properties", BlueprintReadOnly)
	FTimerHandle TimeSinceSpawn;

	UPROPERTY(EditAnywhere, Category = "Effects", BlueprintReadWrite)
	UNiagaraSystem* NS_Standard_Explosion;

	UFUNCTION(BlueprintCallable, Category = "Target Handling")
	void HandleDestruction();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowTargetExplosion();

	UFUNCTION(BlueprintImplementableEvent)
		void RemoveAndReappear();

	UPROPERTY(VisibleAnywhere, Category = "References", BlueprintReadOnly)
	class UDefaultGameInstance* GI;

	UPROPERTY(EditAnywhere, Category = "Materials", BlueprintReadWrite)
	UMaterialInterface* Material;

	UPROPERTY(EditAnywhere, Category = "Materials", BlueprintReadWrite)
	UMaterialInstanceDynamic* MID_TargetColorChanger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UHealthComponent* HealthComp;

	UFUNCTION(BlueprintCallable, Category = "Target Properties")
	void SetMaxHealth(float NewMaxHealth);

	UFUNCTION(BlueprintImplementableEvent, Category = "Target Properties")
	void PlayColorGradient();
};
