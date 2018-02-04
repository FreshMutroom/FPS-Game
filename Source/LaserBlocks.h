// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "LaserBlocks.generated.h"

UCLASS()
class FPS_PROJECT_API ALaserBlocks : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaserBlocks();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* Root;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystemComponent* LaserParticles;
	
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* BlockA;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* BlockB;

	/*
	// True if the laser blocks move back and forth on a patrol
	UPROPERTY(EditAnywhere)
	bool bIsPatrolling;
	
	// The first coordinate for the patrol route
	UPROPERTY(EditAnywhere)
	FVector PatrolCoord1;

	// The second coordinate for the patrol route
	UPROPERTY(EditAnywhere)
	FVector PatrolCoord2;
	
	// The speed the blocks move at if patrolling
	UPROPERTY(EditAnywhere)
	float PatrolSpeed;
	*/
	// For mesh1: Offset to draw the laser at different heights
	UPROPERTY(EditAnywhere)
	float LaserZOffset1;

	// For mesh1: Offset to draw the laser at different heights
	UPROPERTY(EditAnywhere)
	float LaserZOffset2;

};
