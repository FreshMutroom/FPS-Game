// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ChangeWorldSettingsTrigger.generated.h"


/*
	Changes world settings when the player walks into it. Used to change the minimum Z
	depth actors can go to.
*/


UCLASS()
class FPS_PROJECT_API AChangeWorldSettingsTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChangeWorldSettingsTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* OverlapComp;
	
	// New Z value actors that fall below are automatically destroyed
	UPROPERTY(EditAnywhere)
	float NewZValue;

	UFUNCTION()
	void TriggerEnter(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Destroy all non sniper bot AI
	void DestroyAI();

};
