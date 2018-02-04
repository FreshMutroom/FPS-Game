// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/TargetPoint.h"
#include "Ver2_PatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API AVer2_PatrolPoint : public ATargetPoint
{
	GENERATED_BODY()
	
public:
	
	// Bots ID and this ID must match for the bot to use it
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ID;
	
};
