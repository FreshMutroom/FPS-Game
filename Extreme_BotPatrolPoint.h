// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/TargetPoint.h"
#include "Extreme_BotPatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API AExtreme_BotPatrolPoint : public ATargetPoint
{
	GENERATED_BODY()
	
	
public:

	// Extreme bots with the same ID as this point will use it.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ID;

	
};
