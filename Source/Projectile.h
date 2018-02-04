// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FPS_ProjectCharacter.h"
#include "Projectile.generated.h"


// Enum for the equipped weapon of the player
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	LaserRifle,
	AutoRifle,
	GrenadeLauncher,
	SniperRifle,

};


UCLASS()
class FPS_PROJECT_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	// Added by me for grenade launcher ammo only
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	/* The player who fired this bullet*/
	UPROPERTY(EditDefaultsOnly)
	AFPS_ProjectCharacter* MyPlayer;

	UPROPERTY(EditDefaultsOnly)
	USphereComponent* ProjectileCollision;
	
	UPROPERTY(EditDefaultsOnly)
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystemComponent* ProjectileParticles;

	// This is only used by bullet weapons
	UPROPERTY(EditAnywhere)
	class URadialForceComponent* BulletMeshDamage;

	/* The velocity of our bullet */
	UPROPERTY(EditAnywhere)
	float ProjectileSpeed;

	/* This is the amount of seconds to wait before the bullet begins dropping*/
	UPROPERTY(EditDefaultsOnly)
	float GravityDelay;

	// True if the projectile is an instant hit projectile with no gravity
	UPROPERTY(EditAnywhere)
	bool bIsInstantHit;

	/* This is the scale for the gravity acting on our bullet*/
	UPROPERTY(EditDefaultsOnly)
	float GravityScale;

	UFUNCTION()
	void OnProjectileHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit);

	/* Once we call this function gravity will start effecting the bullet, making it drop*/
	void ApplyGravity();

	void Deestroy();

	/* What ammo to use
       0 = laser rifle ammo
	   1 = automatic rifle ammo
	   2 = grenade launcher ammo
	   3 = sniper rifle ammo
	*/
	UPROPERTY(EditAnywhere, Category = "Spawning")
	uint8 Type;
	
	// The blueprint for the grenade launcher ammo. Only edit this in
	// BP_TestProjectile. Leave it blank for any other blueprints.
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class AProjectile> BP_GrenadeLauncher;

	// This should be true for only BP_TestProjectile and false for any other blueprints
	UPROPERTY(EditAnywhere, Category = "Spawning")
	bool bIsOriginalAmmo;

	// Radial force for the grenade launcher only
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	URadialForceComponent* ExplosionForce;

	// A mesh that checks which actors are caught in the blast radius.
	// Only used by grenade launcher ammo
	UPROPERTY(EditAnywhere)
	USphereComponent* BlastSphere;

	// Function used by the blast to determine what it hit
	UFUNCTION(BlueprintCallable)
	void InsideBlast(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit);

	// Blueprint for sniper rifle ammo
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class AProjectile> BP_SniperRifle;

	// Blueprint for auto rifle ammo
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class AProjectile> BP_AutoRifle;

	// A skeletal mesh used only by the sniper rifle ammo for contrail particle system
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* SkeletalMesh;

	// The base amount of damage this projectile does if it hits the torso. Headshots
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ProjectileDamage;

	// Deal damage to everything caught in the blast. Only for grenade launcher
	void DealExplosionDamage();

	// Calculate how much damage to take from explosion based on distance
	float CalculateExplosionDamage(float Distance, float Radius);

	// An array that holds hit decals for when this projectile hits different surfaces
	// Should be populated in editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UMaterialInterface*> ImpactDecals;

	// An array that holds particle effects for when this projectile hits different surfaces
	// Should be populated in editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UParticleSystem*> ImpactParticles;

	// Spawn a hit decal
	void SpawnHitDecal(int32 PhysMaterialType, float Scale, float Duration, const FHitResult & Hit);

	// Max range of projectile
	UPROPERTY(EditAnywhere)
	float MaxRange;

};
