// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/TargetPoint.h"
#include "BotSpawnPoint.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API ABotSpawnPoint : public ATargetPoint
{
	GENERATED_BODY()
	
public:
	
	virtual void BeginPlay() override;

	// Time between spawns. Make it at least 2 seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeBetweenSpawns;

	// The patrol route ID of this point. Doesn't actually do anything, but it helps to make all bots
	// that spawn from this point to use the same ID as this if you want them to follow a predtermined path
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PatrolPointID;

	// Queue of what bots to spawn. 0 = AIPatrol, 1 = Advanced_Bot, 2 = Ver2_Bot
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> SpawnQueue;

	// Index of where we are in spawn queue
	int32 Index;

	// Makes this spawn point active and starts spawning bots
	void Start();

	// Called by timer handle and calls SpawnBot
	void BeforeSpawnBot();

	// Signals what bot to spawn. Needed by timer handle 0 = AIPatrol, 1 = Advanced_Bot, 2 = Ver2_Bot
	int32 SpawnID;

	// Spawn a bot by calling a helper function
	void SpawnBot();

	void SpawnAIPatrol();

	void SpawnAdvanced_Bot();

	void SpawnVer2_Bot();

	// Blueprint for all AIPatrols spawned from this point
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AAIPatrol> BP_AIPatrol;

	// Blueprint for all advacned bots spawned from this point
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AAdvanced_Bot> BP_Advanced_Bot;

	// Blueprint for all ver2 bots spawned from this point
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AVer2_Bot> BP_Ver2_Bot;


private:

	// Controls when the next bot should spawn
	FTimerHandle SpawnTimerHandle;


};
