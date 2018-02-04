// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "InvisibleMesh.h"


// Sets default values
AInvisibleMesh::AInvisibleMesh()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Invisible Mesh"));
	Mesh->SetVisibility(false);
	Mesh->SetHiddenInGame(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetEnableGravity(false);
	RootComponent = Mesh;

}

// Called when the game starts or when spawned
void AInvisibleMesh::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();

	if (World)
	{
		FTimerHandle DestroyTimerHandle;
		World->GetTimerManager().SetTimer(DestroyTimerHandle, this, &AInvisibleMesh::Deestroy, 5.f, false);

	}

}

// Called every frame
void AInvisibleMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInvisibleMesh::Deestroy()
{
	Destroy();

}

