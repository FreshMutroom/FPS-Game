// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Explosion.generated.h"


/* A class that represents an explosion from the grenade launcher
   and damages enemies, the player and destroys DamagableRocks. */
UCLASS()
class FPS_PROJECT_API AExplosion : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosion();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// The mesh used to see if anything is caught in the explosion
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;

	UFUNCTION()
	void TriggerEnter(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void TriggerExit(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex);

	void Deestroy();

};
