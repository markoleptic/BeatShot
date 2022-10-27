// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DefaultHealthComponent.generated.h"


class UDefaultGameInstance;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEATSHOT_API UDefaultHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDefaultHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SetMaxHealth(float NewMaxHealth);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TotalPossibleDamage;

	UPROPERTY(VisibleAnywhere, Category = "Target Properties", BlueprintReadOnly)
	bool ShouldUpdateTotalPossibleDamage;

private:
	UPROPERTY(EditAnywhere)
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere)
	float Health = 0.f;

	UFUNCTION()
	void DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* Instigator,
		AActor* DamageCauser);

	UPROPERTY(VisibleAnywhere)
	UDefaultGameInstance* GI;
};
