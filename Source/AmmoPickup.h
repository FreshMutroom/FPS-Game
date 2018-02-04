// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FPS_ProjectCharacter.h"
#include "AmmoPickup.generated.h"

UCLASS()
class FPS_PROJECT_API AAmmoPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAmmoPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/* The type of weapon this ammo pickup is for
	   0 = laser rifle, 1 = auto rifle, 2 = grenade launcher, 3 = sniper rifle */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint8 Type;

	// The amount of ammo this pickup gives
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int Amount;

	// Mesh for ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* AmmoMesh;

	// Collision sphere for the ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* AmmoCollision;

	// A reference to the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AFPS_ProjectCharacter* Player;

	// True if the player is in range of the ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsPlayerInRange;

	// If true this ammo will respawn after a delay
	UPROPERTY(EditAnywhere)
	bool bRespawns;

	// Delay for respawn
	UPROPERTY(EditAnywhere)
	float RespawnDelay;

	// If respawning then this should be false
	bool bCanBePickedUp;

	// Makes ammo respawn
	void Respawn();

	UFUNCTION()
	void TriggerEnter(UPrimitiveComponent * OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void TriggerExit(UPrimitiveComponent * OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


};
