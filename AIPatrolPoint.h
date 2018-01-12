// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/TargetPoint.h"
#include "AIPatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API AAIPatrolPoint : public ATargetPoint
{
	GENERATED_BODY()

public:
	// The ID of the point. If this points ID and a AIPatrol ID match
	// then it will patrol on these points.
	UPROPERTY(EditAnywhere)
	uint32 ID;
	
	
};
