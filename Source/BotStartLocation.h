// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/TargetPoint.h"
#include "BotStartLocation.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API ABotStartLocation : public ATargetPoint
{
	GENERATED_BODY()
	
	
public:

	ABotStartLocation();
	
	/* A mesh so the rotation of the spawn point can be found easily*/
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent * Mesh;

	/* The type of bot to spawn at this location
		0 = AI patrol
		1 = Advanced bot
		2 = Ver2 bot
		3 = Ver 2 firing range bot
	*/
	UPROPERTY(EditAnywhere)
	int32 ID;

	/* Spawns bot based on ID */
	UFUNCTION()
	void SpawnBot();

	// Blueprint for all AIPatrols spawned from this point
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AAIPatrol> BP_AIPatrol;

	// Blueprint for all advacned bots spawned from this point
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AAdvanced_Bot> BP_Advanced_Bot;

	// Blueprint for all ver2 bots spawned from this point
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AVer2_Bot> BP_Ver2_Bot;

	// Blueprint for all ver2 firing range bots spawned from this point
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AVer2_Bot> BP_Ver2_RangeBot;

};
