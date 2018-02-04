// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FPS_ProjectCharacter.h"
#include "Moveable.generated.h"

UCLASS()
class FPS_PROJECT_API AMoveable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMoveable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// If this moveable moves from the time it is spawned and never stops
	UPROPERTY(EditAnywhere)
	bool bMovesWhenSpawned;

	// If true then this object will move without the need for an activation item
	// such as a key card
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAlwaysTriggers;

	// If true then this will only trigger once
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTriggersOnce;

	// True if the moveable cannot trigger again
	bool bCannotMoveAgain;

	// If true then the moveable will show a HUD message whenever the player enters its trigger box
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bShowsHUDMessage;

	// Text to be displayed on screen when near this object
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString OnScreenMessage;

	// Text to be displayed when the player enters the trigger box and does not have a keycard
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString OnScreenErrorMessage;

	// The length in seconds the on screen message is displayed. 0.0 implies while the player is in the trigger range.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MessageDisplayTime;

	// Removes on screen text of the character
	UFUNCTION(BlueprintCallable)
	void RemoveOnScreenText();

	// If true this moveable will not trigger until the player has accessed the computer
	UPROPERTY(EditAnywhere)
	bool bChecksForComputerEvent;

	// If true this moveable will check for completing the bunker event
	UPROPERTY(EditAnywhere)
	bool bChecksForBunkerEvent;

	// Typerwiter message to play when triggering moveable. Leave blank to trigger no message
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ATutorialMessage> BP_TypewriterMessage;

	// Creates typewriter message
	void CreateTypewriterMessage();

	// True if the player is within range
	bool bPlayerIsWithinRange;
	
	// True if the object has been activated and is already moving
	bool bIsAlreadyMoving;

	UPROPERTY()
	AFPS_ProjectCharacter* CurrentPlayerController;
	
	/* The axis the object moves along. Should be one of X, Y or Z */
	UPROPERTY(EditAnywhere)
	FString MovementAxis;

	UPROPERTY(EditAnywhere)
	UShapeComponent* CollisionBox;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* SM_Moveable;

	// Curve float reference
	UPROPERTY(EditAnywhere)
	UCurveFloat* AnimCurve;

	// The max that the moveable will move
	UPROPERTY(EditAnywhere)
	float MaxMovement;

	UFUNCTION()
	void TriggerEnter(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void TriggerExit(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION(BlueprintCallable)
	void Open();
	
	// Initial rotation
	FVector ActorInitialLocation;

	// Target location
	FVector TargetLocation;

	// Timeline to make moveable move
	UPROPERTY()
	FTimeline MyTimeline;

	UFUNCTION(BlueprintCallable)
	void HandleProgress(float Value);

};
