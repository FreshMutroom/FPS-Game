// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Ver2_LauncherProjectile.generated.h"

/*
   Buglist: 
   - When the projectile calls Explode() it still causes collision until it is destroyed 5s later
*/


UCLASS()
class FPS_PROJECT_API AVer2_LauncherProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVer2_LauncherProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sphere for collision
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* CollisionComp;
	
	// Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UProjectileMovementComponent* ProjectileMovement;

	// Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	// Particles for the trail behind 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystemComponent* TrailParticles;

	// Particles for explosion
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystemComponent* ExplosionParticles;

	// Radial force for detonation
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	URadialForceComponent* ExplosionForce;

	// reference to player
	UPROPERTY(EditAnywhere)
	AFPS_ProjectCharacter* Player;

	// Min amount of damage this deals
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinDamage;

	// Max amount of damage this deals
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxDamage;

	// Radius of blast
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BlastRadius;

	// Array of sounds for explosion. Should be populated in editor
	UPROPERTY(EditDefaultsOnly, Category = "Sound Effects")
	TArray<USoundBase*> ExplosionSounds;

	// Array of sounds for the rocket travelling past the player
	UPROPERTY(EditDefaultsOnly, Category = "Sound Effects")
	TArray<USoundBase*> RokcetInFlightSounds;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Doesn't seem to be working
	UFUNCTION()
	void OnProjectileHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit);
	
	// For timer handle
	void Deestroy();

	// Make projectile blow up
	void Explode();

	// True if projectile has collided with something
	bool bIsPendingDestroy;

};
