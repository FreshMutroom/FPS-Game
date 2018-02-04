// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "TutorialMessage.generated.h"

UCLASS()
class FPS_PROJECT_API ATutorialMessage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATutorialMessage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// The overlap box that generates the message
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* OverlapShape;

	// A reference to the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AFPS_ProjectCharacter* Player;

	// The message to display
	FString Message;

	/* You can have up to 4 sentences for this message. If you want a pause between each sentence 
	   then type each new sentence in a new FString sentence variable. Otherwise just put them 
	   all in Sentence1. Put a space at the start of each of Sentence2, 3 and 4 */

	// First sentence
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Sentence1;

	// Second sentence
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Sentence2;

	// Third sentence
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Sentence3;

	// Forth sentence
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Sentence4;

	// Delay in starting message
	UPROPERTY(EditAnywhere)
	float StartDelay;
	
	// Create message. Called by timer handle
	void CreateMessage();

	// Delay after showing first sentence
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Sentence1Delay;

	// Delay after showing Second sentence
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Sentence2Delay;

	// Delay after showing Third sentence
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Sentence3Delay;


	// When the message should disappear after it has finished
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EraseTime;

	// Whether this message should be able to be repeated if the player re-enters its trigger box
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanRepeat;

	// If this message does repeat, the cooldown before it can display again
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RepeatCooldown;

	// True if message can repeat and is cooling down.
	bool bIsCoolingDown;

	// True if the player is currently overlapping the overlap shape
	bool bIsPlayerInRange;

	// Timer handle for repeating
	FTimerHandle RepeatTimerHandle;

	// For timer handle: makes bIsCoolingDown false
	void MakeIsCoolingDownFalse();

	// If a BP is put here then this message will play after this message finishes
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ATutorialMessage> NextMessage;

	UFUNCTION()
	void TriggerEnter(UPrimitiveComponent * OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void TriggerExit(UPrimitiveComponent * OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);



};
