// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/TargetPoint.h"
#include "Sniper_VantagePoint.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API ASniper_VantagePoint : public ATargetPoint
{
	GENERATED_BODY()
	
public:

	// If true then the bot should go prone at this vantage point. Otherwise they wil crouch
	UPROPERTY(EditAnywhere)
	bool bShouldGoProne;
	
	// True if the sniper bot could move here. Only here for debugging
	UPROPERTY(EditAnywhere)
	bool bIsActive = true;

};
