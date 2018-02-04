// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Sound/AmbientSound.h"
#include "MyAmbientSound.generated.h"

/**
 *	 
 */
UCLASS()
class FPS_PROJECT_API AMyAmbientSound : public AAmbientSound
{
	GENERATED_BODY()


	virtual void BeginPlay() override;


public:
	
	// Update volume of audio component
	void Update();

	// Min volume of audio component
	UPROPERTY(EditDefaultsOnly)
	float MinVolume = 0.001f;

	// Max volume of audio component
	UPROPERTY(EditDefaultsOnly)
	float MaxVolume = 1.f;

};
