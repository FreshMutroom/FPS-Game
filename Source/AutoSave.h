// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "AutoSave.generated.h"

/* 
	Automatically saves the game when the player enters the trigger box. OPtionally certain conditions
	must be true for the save to happen such as a certain weapon needs to be in inventory.
*/


UCLASS()
class FPS_PROJECT_API AAutoSave : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAutoSave();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	UBoxComponent* TriggerBox;
	
	// True if this save point has been triggered. If it has it cannot be activated again
	bool bHasBeenTriggered;

	// If true this auto save will check if player has auto rifle before saving
	UPROPERTY(EditAnywhere)
	bool bChecksForAutoRifle;

	// Check if player has keycard in inventory
	UPROPERTY(EditAnywhere)
	bool bChecksForKeyCard;

	// Checks if auto rifle is in players inventory
	bool CheckIfAutoRifleInInventory(AFPS_ProjectCharacter* Player);

	// Checks to see if keycard is in inventory
	bool CheckIfKeyCardInInventory(AFPS_ProjectCharacter* Player);

	// Run all the checks above. Will skips tests that aren't required. Run this on overlap. Returns 
	// true if the game can be saved
	bool MakeChecks(AFPS_ProjectCharacter* Player);

	UFUNCTION()
	void TriggerEnter(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


};
