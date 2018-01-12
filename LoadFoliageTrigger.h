// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "LoadFoliageTrigger.generated.h"

/*
	Changes visiblity of foliage when player enters trigger
*/
UCLASS()
class FPS_PROJECT_API ALoadFoliageTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALoadFoliageTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* LoadTriggerBox;
	
	UPROPERTY(EditAnywhere)
	class UBoxComponent* UnloadTriggerBox;

	// The name of the level that holds the foliage to toggle visibility on

	// Reference to the foliage to load
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AInstancedFoliageActor* Foliage;

	// Holds references to all foliage.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class AInstancedFoliageActor*> Foliages;

	// Name of foliage instance you want to toggle visibility on. Will not work in packaged game
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FoliageName;

	// Alternative to using GetName(). Use index of array. Banks on the hope that GetAllActorsOfClass
	// always creates the same array every time.
	// Going to have to use trial and error with it
	// 1 = Part 2 foliage, 2 = part 1 foliage
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index;

	// Only false for first tick. Not needed anymore
	bool bHasSearchedAllActors;

	// Populate arrray with all foliage instances after a delay to make sure they have all loaded
	void AssignFoliage();

	// True if foliage is loaded, false if it is not. You will need to set this to the correct value
	// in the editor
	UPROPERTY(EditAnywhere)
	bool bIsFoliageVisible;

	// SetActorHiddenInGame() does not work here so this function is implemented in blueprints
	UFUNCTION(BlueprintImplementableEvent)
	void SetHidden(bool NewValue);

	UFUNCTION()
	void Load(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void Unload(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
