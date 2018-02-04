// Fill out your copyright notice in the Description page of Project Settings.

/* Something is wrong in a LaserBlocks file I think */

#include "FPS_Project.h"
#include "LaserBlocks.h"


// Sets default values
ALaserBlocks::ALaserBlocks()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(FName("Root"));

	BlockA = CreateDefaultSubobject<UStaticMeshComponent>(FName("BlockA"));
	BlockA->SetRelativeLocation(FVector(500.f, 0.f, 0.f));

	BlockB = CreateDefaultSubobject<UStaticMeshComponent>(FName("BlockB"));
	BlockA->SetRelativeLocation(FVector(-500.f, 0.f, 0.f));

	LaserParticles = CreateDefaultSubobject<UParticleSystemComponent>(FName("LaserParticles"));

}

// Called when the game starts or when spawned
void ALaserBlocks::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ALaserBlocks::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*
	// Move the blocks if they are a patrolling type
	if (bIsPatrolling)
	{
		const FVector Temp = FVector((BlockA->GetComponentLocation() - (8 * DeltaTime)));
		SetActorLocation(Temp);
	}*/

	const FVector Offset1 = FVector(0.f, 0.f, LaserZOffset1);
	const FVector Offset2 = FVector(0.f, 0.f, LaserZOffset2);

	LaserParticles->SetBeamSourcePoint(0, BlockA->GetComponentLocation() + Offset1, 0);
	LaserParticles->SetBeamTargetPoint(0, BlockB->GetComponentLocation() + Offset2, 0);



}


