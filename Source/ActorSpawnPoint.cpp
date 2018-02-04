// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "ActorSpawnPoint.h"


// Sets default values
AActorSpawnPoint::AActorSpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->bGenerateOverlapEvents = false;
	Mesh->SetHiddenInGame(true);
	RootComponent = Mesh;

}

// Called when the game starts or when spawned
void AActorSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AActorSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AActorSpawnPoint::Spawn()
{
	UWorld * World = GetWorld();
	if (World && BP_ActorToSpawn)
	{
		ActorToSpawn = World->SpawnActor<AActor>(BP_ActorToSpawn, GetActorLocation(), GetActorRotation());
	}

}

void AActorSpawnPoint::DestroyLinkedActor()
{
	if (ActorToSpawn)
	{
		ActorToSpawn->Destroy();
	}

}

