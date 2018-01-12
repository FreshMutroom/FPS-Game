// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Damagable.h"
#include "FPS_ProjectCharacter.h"
#include "DamagingRock.generated.h"

UCLASS()
class FPS_PROJECT_API ADamagingRock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADamagingRock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// The static mesh
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* SM_DamagingRock;

	// The destructible mesh
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ADestructibleActor> DM_DamagingRock;

	// The number of ticks to keep the static mesh around after it has spawned the 
	// destructible mesh. This is to cause the destructible mesh to explode.
	int8 TicksSinceDMSpawn = -1;

	// True if the rock has been fractured
	UPROPERTY(EditAnywhere)
	bool bHasBeenDestroyed = false;

	// The amount of health this rock has. When it reaches 0 it will fracture
	UPROPERTY(EditAnywhere)
	float Health;

	// The amount this rock damages the player when it hits them
	UPROPERTY(EditAnywhere)
	float Damage;

	// Cause the destructible mesh to spawn and fracture instantly
	UFUNCTION(BlueprintCallable)
	void Explode(FVector NormalImpulse);

	bool bIsCollidingWithPlayer;

	// If the rock is traveling at at least this speed it can damage the player if it collides with them. 
	// It needs to be quite large e.g. > 1 000 000
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamagingSpeed;

	//AFPS_ProjectCharacter* CurrentPlayer;

	// Called when the rock collides with something
	UFUNCTION()
	void OnHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit);

};
