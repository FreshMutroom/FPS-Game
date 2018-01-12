// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "Pickup.h"


// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PickupRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PickupRoot"));
	RootComponent = PickupRoot;

	// Commented since errors when packaging from AttachToComponent. Don't use this class anyway
	//PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MyMesh"));	// TEXT = name of mesh in editor
	//PickupMesh->AttachToComponent(PickupRoot, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	//
	//PickupBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PickupBox"));
	//PickupBox->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));	// Size of box, can change in editor
	//PickupBox->bGenerateOverlapEvents = true;
	//PickupBox->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnPlayerEnterPickupBox);
	// // attach box to root. Can also attach to mesh
	//PickupBox->AttachToComponent(PickupRoot, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewLocation = GetActorLocation();

	NewLocation.Z += (DeltaTime * 100.0f);

	SetActorLocation(NewLocation);

}

void APickup::OnPlayerEnterPickupBox(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	Destroy();

}

