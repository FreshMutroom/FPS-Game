// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Sniper_DangerArea.generated.h"

/* 
	This has actually been changed to a safe area. If the player is inside this box then they cannot
	be hit by the sniper bot. This should be placed at the path that leads to the armory
*/

UCLASS()
class FPS_PROJECT_API ASniper_DangerArea : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASniper_DangerArea();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* OverlapComp;
	
	// A reference to the sniper bot
	UPROPERTY()
	ASniper_Bot* SniperBot;

	// A reference to the player
	UPROPERTY()
	class AFPS_ProjectCharacter* Player;

	// Time spent inside the box for the sniper bot to rotate to face it
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SniperBotReactionTime;

	// Time spent inside the box for the sniper bot to take an action against it
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SniperBotActionTime;

	// Timer handle for controlling actions taken when entering trigger box
	FTimerHandle TriggerBoxTimerHandle;

	UFUNCTION()
	void TriggerEnter(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void TriggerExit(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex);

	// True if the player in inside the trigger box
	bool bIsPlayerInLoS;

	// Called to check if an actor that overlapped the trigger box is still inside it
	UFUNCTION()
	void OnSniperBotReact();


};
