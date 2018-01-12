// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FiringRangeTarget.generated.h"

UCLASS()
class FPS_PROJECT_API AFiringRangeTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFiringRangeTarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Main part that has the image on it
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Target;

	
};
