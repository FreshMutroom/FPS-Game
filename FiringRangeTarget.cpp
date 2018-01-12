// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "FiringRangeTarget.h"
#include "Runtime/Engine/Classes/Components/PrimitiveComponent.h"


// Sets default values
AFiringRangeTarget::AFiringRangeTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	Target = CreateDefaultSubobject <UStaticMeshComponent>(TEXT("Main part"));
	Target->bGenerateOverlapEvents = false;
	Target->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Target->SetHiddenInGame(true);
	RootComponent = Target;

}

// Called when the game starts or when spawned
void AFiringRangeTarget::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFiringRangeTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

