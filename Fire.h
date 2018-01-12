// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FPS_ProjectCharacter.h"
#include "Fire.generated.h"

UCLASS()
class FPS_PROJECT_API AFire : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFire();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UShapeComponent* TB_Fire;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* SM_Fire;
	
	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* PS_Fire;

	AFPS_ProjectCharacter* CurrentPlayerController;

	UPROPERTY(EditAnywhere)
	FString HelpText = FString(TEXT("Press F to activate fire"));

	int32 ResetTime;

	bool bPlayerIsWithinRange = false;
	bool bFireIsLit = false;

	void GetPlayer(AActor* Player);
	void Light();
	void AdvanceTimer();
	void TimerHasFinished();

	FTimerHandle CountdownTimerHandle;

	UFUNCTION()
	void TriggerEnter(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void TriggerExit(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex);







};
