// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FPS_ProjectCharacter.h"
#include "Item.generated.h"

UCLASS()
class FPS_PROJECT_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/* Toggles item glow*/
	void ToggleGlow(bool IsGlowing);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* TBox;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* SM_TBox;
	
	// Sound for when item/weapon is picked up
	UPROPERTY(EditAnywhere)
	USoundBase* PickupSound;

	void PlayPickupSound(int32 WType);

	AFPS_ProjectCharacter* MyPlayerController;

	UPROPERTY(EditAnywhere)
	FString ItemName;

	void Pickup();

	void GetPlayer(AActor* Player);

	bool bItemIsWithinRange = false;

	// True if item should simulate physics
	UPROPERTY(EditAnywhere)
	bool bShouldSimulatePhysics;

	/* True if the item can be picked up */
	bool bCanBePickedUp;

	/* For timer handle: makes the weapon able to be picked up */
	void MakePickupOk();

	/* True if the item should glow when the player is close and looking at it */
	UPROPERTY(EditAnywhere)
	bool bCanGlow;

	// The message to show on the player HUD when within range of this pickup
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Message;

	/* True if this is a weapon pickup, false otherwise */
	UPROPERTY(EditAnywhere)
	bool bIsWeaponPickup;

	/* The weapon type if this is a weapon pickup
	 * 0 = Unarmed, 1 = Auto Rifle
	 */
	UPROPERTY(EditAnywhere)
	uint8 WeaponType;

	/* This only needs a mesh if this is a weapon pickup */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* WeaponSkeletalMesh;

	/* Only called if this is a weapon pickup. This function chnages the players damage, rate of fire etc*/
	UFUNCTION()
	void UpdatePlayerWeaponInfo(uint8 Type);

	// True if this item has been picked up before. False otherwise.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasBeenPickedUpBefore;

	UFUNCTION()
	void TriggerEnter(UPrimitiveComponent * OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void TriggerExit(UPrimitiveComponent * OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Only for weapon pickups: the ammo this weapon holds in its clip.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ClipAmmo;

	// Drops the item back into the world
	UFUNCTION(BlueprintCallable)
	void DropEquippedWeapon(uint8 WType);

	// The blueprint for the laser rifle item so that it can be spawned 
	// into the world when it is dropped.
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class AItem> BP_LaserRifle;

	// The blueprint for the auto rifle item so that it can be spawned 
	// into the world when it is dropped.
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class AItem> BP_AutoRifle;

	// The blueprint for the grenade launcher item so that it can be spawned 
	// into the world when it is dropped.
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class AItem> BP_GrenadeLauncher;

	// The blueprint for the sniper rifle item so that it can be spawned 
	// into the world when it is dropped.
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class AItem> BP_SniperRifle;

};
