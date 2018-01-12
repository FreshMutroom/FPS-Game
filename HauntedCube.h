// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "HauntedCube.generated.h"

UCLASS()
class FPS_PROJECT_API AHauntedCube : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHauntedCube();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Dummy root component for the cube
	UPROPERTY(EditAnywhere)
	USceneComponent* CubeRoot;

	// The mesh for the cube
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* CubeMesh;

	// The trigger for the cube
	UPROPERTY(EditAnywhere)
	UBoxComponent* CubeTrigger;

	UMaterialInstanceDynamic* RustMaterialInstance;

	// Function called when player triggers the rust effect
	UFUNCTION()
	void OnPlayerTriggerRust(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Function called when the player untriggers the rust effect
	UFUNCTION()
	void OnPlayerExitRust(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:

	bool bRustEffectTriggered;

	float RustAmount;

};
