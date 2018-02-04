// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "IdleGuard.generated.h"

UCLASS()
class FPS_PROJECT_API AIdleGuard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIdleGuard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// True if the idle guard has been shot
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasBeenShot;

	// Called when shot
	UFUNCTION(BlueprintCallable)
	void HasBeenShot();
	
	// Skeletal mesh for the character
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* CharacterMesh;

	// Static mesh for the gun
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* WeaponMesh;

	// Stop ragdoll
	void StopRagdoll();

};
