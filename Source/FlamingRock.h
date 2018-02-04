// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FlamingRock.generated.h"

UCLASS()
class FPS_PROJECT_API AFlamingRock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFlamingRock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UShapeComponent* Root;

	// The static mesh for the rock
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMesh;
	
	// Aprticle system for the rock
	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* PS_Rock;

	// The duration in seconds the fire stays lit
	UPROPERTY(EditDefaultsOnly)
	float ParticleDuration;

	// The amount of damage this rock will deal when hitting the player. Should be 101.f
	UPROPERTY(EditAnywhere)
	float Damage;

	// How fast the boulder must be going for it to damage the player
	UPROPERTY(EditAnywhere)
	float DamagingSpeed;

	// DamagingSpeed squared. Calculated on begin play
	float DamagingSpeedSquared;

	// Makes boulder set fire
	void SetFire();

	// Sound of fire
	UPROPERTY(EditAnywhere)
	USoundBase* FireSound;

	// Timer handle to put fall sound on cooldown
	FTimerHandle FallSoundTimerHandle;

	void FallSoundOffCooldown();

	// Sound of rock hitting ground from fall
	UPROPERTY(EditAnywhere)
	USoundBase* FallSound;

	// Called when the rock collides with something
	UFUNCTION()
	void OnHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit);


	FTimerHandle CountdownTimerHandle;

	void PutOutFire();

	bool bFireAlreadyLit = false;
};
