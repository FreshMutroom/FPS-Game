// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "Explosion.h"
#include "FPS_ProjectCharacter.h"

// Sets default values
AExplosion::AExplosion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Explosion mesh"));
	Mesh->bGenerateOverlapEvents = true;
	Mesh->AttachTo(RootComponent);
	Mesh->SetVisibility(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetEnableGravity(false);
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &AExplosion::TriggerEnter);
	Mesh->OnComponentEndOverlap.AddDynamic(this, &AExplosion::TriggerExit);

	// Get static mesh from content browser
	//static ConstructorHelpers::FObjectFinder<UStaticMeshComponent> TempMesh
	//	(TEXT("StaticMesh'/Game/StarterContent/Props/MaterialSphere.MaterialSphere'"));
	//if (TempMesh.Object)
	//{
	//	Mesh = TempMesh.Object;
	//}

}

// Called when the game starts or when spawned
void AExplosion::BeginPlay()
{
	Super::BeginPlay();

	// Destroy the mesh after 5 seconds for testing purposes
	FTimerHandle DestroyTimerHandle;
	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AExplosion::Deestroy, 5.f, false);

}

// Called every frame
void AExplosion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosion::TriggerEnter(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(AFPS_ProjectCharacter::StaticClass()))
	{

	}

}

void AExplosion::TriggerExit(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
}

void AExplosion::Deestroy()
{
	
	Destroy();

}

