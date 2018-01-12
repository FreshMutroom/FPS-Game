// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "TriggerEvent.generated.h"

/*
	The intended use for this class is to create the bot spawn points. 
*/

UCLASS()
class FPS_PROJECT_API ATriggerEvent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATriggerEvent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Trigger box used for player interacting with this
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USphereComponent* TriggerSphere;

	// Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* Mesh;
	
	// HUD message to appear when entering trigger box
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString HUDMessage;

	// True if player is inside trigger box
	bool bIsPlayerInRange;

	// True if the mesh has been activated
	bool bHasBeenActivated;

	// A reference tutorial message that will play a typewriter message. It will be spawned and 
	// then destroyed after the message has finished
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ATutorialMessage> TypewriterMessage;

	// Delay in spawning bot spawn points
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Delay;

	// Makes spawn points linked to this trigger event active
	void MakeSpawnPointsActive();

	// Reference to player
	class AFPS_ProjectCharacter* Player;

	UFUNCTION()
	void TriggerEnter(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void TriggerExit(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex);



};
