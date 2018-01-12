// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SniperCutsceneProjectile.generated.h"

UCLASS()
class FPS_PROJECT_API ASniperCutsceneProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASniperCutsceneProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UParticleSystemComponent* SniperTrail;
	
	// Start location for trail
	UPROPERTY(EditAnywhere)
	FVector StartTrace;

	// End location for trail
	UPROPERTY(EditAnywhere)
	FVector EndTrace;
};
