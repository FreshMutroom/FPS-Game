// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Sniper_Projectile.generated.h"


/*
	Projectile class for Sniper_Bot
*/

UCLASS()
class FPS_PROJECT_API ASniper_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASniper_Projectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Trail PS
	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* ProjectileTrail;
	
	// Reference to player
	UPROPERTY()
	class AFPS_ProjectCharacter* Player;

	// How far away from player centre this shot can be on X axis
	UPROPERTY(EditAnywhere)
	float XVariance;

	// How far away from player centre this shot can be on Y axis
	UPROPERTY(EditAnywhere)
	float YVariance;

};
