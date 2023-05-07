// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingTextActor.generated.h"

/** Provides a way to display floating text on player's screen showing their current target streak */
UCLASS(Abstract)
class BEATSHOT_API AFloatingTextActor : public AActor
{
	GENERATED_BODY()

public:
	/* Sets default values for this actor's properties */
	AFloatingTextActor();

	UFUNCTION(BlueprintImplementableEvent, Category = "FloatingTextActor")
	void Initialize(const FText& Text);

	/* Returns the Anchor location */
	const FVector& GetAnchorLocation() const { return AnchorLocation; }

protected:
	/* Called when the game starts or when spawned */
	virtual void BeginPlay() override;

public:
	/* Called every frame */
	virtual void Tick(float DeltaTime) override;

private:
	/* Location to which the text is anchored */
	FVector AnchorLocation;
};
