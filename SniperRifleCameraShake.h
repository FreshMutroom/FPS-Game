// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraShake.h"
#include "SniperRifleCameraShake.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API USniperRifleCameraShake : public UCameraShake
{
	GENERATED_BODY()
	
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OccilationDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OccilationBlendInTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OccilationBlendOutTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RotationOscillation;

	
};
