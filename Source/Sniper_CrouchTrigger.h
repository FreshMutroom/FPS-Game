// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Sniper_CrouchTrigger.generated.h"


/* 
	Placed for the sniper bot. Makes him go crouched when he enters it so that he approaches
	a vantage point crouch walking to make him less easy to spot
*/

UCLASS()
class FPS_PROJECT_API ASniper_CrouchTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASniper_CrouchTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Reference to sniper bot
	UPROPERTY()
	class ASniper_Bot* SniperBot;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionComp;

	// If true then when the sniper bot enters this box it will crouch. This needs to be toggled every time
	// the bot enters the collision box since it should only crouch when moving through it one way, but not 
	// the other way. This should be true for all crouch triggers except one which is the one that is close 
	// to the sniper bots spawn point, but only if the sniper bot starts crouched. If the sniper bot starts
	// prone then no box needs its default value of this edited.
	UPROPERTY(EditAnywhere)
	bool bShouldCrouchBot;

	// Timer handle so overlaps cannot happen in quick succession. This is only in case movement 
	// somehow makes the bot go back and forwards for some sections
	FTimerHandle TriggerCooldownTimerHandle;

	void ResetTimerHandle();

	UFUNCTION()
	void TriggerEnter(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Lock for overlap trigger
	FCriticalSection OverlapLock;

};
