// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FPS_ProjectCharacter.h"
#include "WeaponPickup.generated.h"


UCLASS()
class FPS_PROJECT_API AWeaponPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/* The type of the weapon
	*  0 = Unarmed, 1 = Auto Rifle
	*/
	UPROPERTY(EditAnywhere)
	uint8 WeaponType;

	UPROPERTY(EditAnywhere)
	UShapeComponent* WeaponBox;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* WeaponMesh;

	UFUNCTION()
	void PickupWeapon();

	void GetPlayer(AActor* Player);

	AFPS_ProjectCharacter* MyPlayerController;

	bool bWeaponIsWithinRange;

	UFUNCTION()
	void TriggerEnter(UPrimitiveComponent * OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void TriggerExit(UPrimitiveComponent * OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
