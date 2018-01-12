// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Ver2_Projectile.generated.h"

UCLASS()
class FPS_PROJECT_API AVer2_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVer2_Projectile();

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

	// Reference to player
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AFPS_ProjectCharacter* Player;

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

	// Min value this will be from centre
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float XVarianceMin;

	// Max value this will be from centre
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float XVarianceMax;

	// Min value this will be from centre
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float YVarianceMin;

	// Max value this will be from centre
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float YVarianceMax;

};
