// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Advanced_BotProjectile.generated.h"

UCLASS()
class FPS_PROJECT_API AAdvanced_BotProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAdvanced_BotProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Particle system for projectile
	UPROPERTY(EditDefaultsOnly)
	UParticleSystemComponent* ProjectileParticles;

};
