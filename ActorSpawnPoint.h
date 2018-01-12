// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ActorSpawnPoint.generated.h"

UCLASS()
class FPS_PROJECT_API AActorSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActorSpawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Blueprint of actor to spawn
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AActor> BP_ActorToSpawn;
	
	// Mesh to get direction spawn point is facing
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent * Mesh;

	// Actor to spawn
	UPROPERTY()
	AActor * ActorToSpawn;
	
	// Spawn Actor
	void Spawn();

	// Destroy actor that was spawned from this point
	void DestroyLinkedActor();

};
