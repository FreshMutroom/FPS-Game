// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Ver2_FiringRangeProjectile.generated.h"

UCLASS()
class FPS_PROJECT_API AVer2_FiringRangeProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVer2_FiringRangeProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ProjectileMesh;

	// Trail particles
	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* RifleParticles;

	// Ammo particles
	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* RifleAmmoParticles;

	UPROPERTY(EditAnywhere)
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere)
	USphereComponent* ProjectileCollision;

	// Reference to bot who firing this projectile
	class AVer2_Bot* OwningBot;

	// Reference to player
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AFPS_ProjectCharacter* Player;

	// Having problems with these collising and destroying the moment they spawn. Timer handle 
	// to prevent this from happening
	FTimerHandle DurationTimerHandle;

	// Makes projectile ok to collide with things
	void OkToCollide();

	// True if projectile can collide with things
	bool bCanCollide;

	/* The velocity of our bullet */
	UPROPERTY(EditAnywhere)
	float ProjectileSpeed;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Min damage this projectile will deal
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MinDamage;

	// Max damage this projectile will deal
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxDamage;

	// Min value this will be from centre in degrees
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float XVarianceMin;

	// Max value this will be from centre in degrees
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float XVarianceMax;

	// Min value this will be from centre in degrees
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float YVarianceMin;

	// Max value this will be from centre in degrees
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float YVarianceMax;
	
};
