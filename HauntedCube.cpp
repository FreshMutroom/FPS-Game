// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "HauntedCube.h"


// Sets default values
AHauntedCube::AHauntedCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Commented all this since it causes errors when packaging. Don't even use this class anyway
	//CubeRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CubeRoot"));
	//RootComponent = CubeRoot;
	//
	//CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
	//CubeMesh->AttachToComponent(CubeRoot, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	//
	//CubeTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("CubeTrigger"));
	//CubeTrigger->bGenerateOverlapEvents = true;
	//CubeTrigger->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	//CubeTrigger->OnComponentBeginOverlap.AddDynamic(this, &AHauntedCube::OnPlayerTriggerRust);
	//CubeTrigger->OnComponentEndOverlap.AddDynamic(this, &AHauntedCube::OnPlayerExitRust);
	//CubeTrigger->AttachToComponent(CubeRoot, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	RustAmount = 1.0f;

}

// Called when the game starts or when spawned
void AHauntedCube::BeginPlay()
{
	Super::BeginPlay();
	
	RustMaterialInstance = CubeMesh->CreateDynamicMaterialInstance(0);

}

// Called every frame
void AHauntedCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bRustEffectTriggered)
	{
		if (RustAmount > 0.0f)
		{
			RustAmount -= DeltaTime;
		}
	}
	else
	{
		if (RustAmount < 1.0f)
		{
			RustAmount += DeltaTime;
		}
	}

	if (RustMaterialInstance != nullptr)
	{
		RustMaterialInstance->SetScalarParameterValue(FName("RustAmount"), RustAmount);
	}

}

void AHauntedCube::OnPlayerTriggerRust(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	bRustEffectTriggered = true;
}

void AHauntedCube::OnPlayerExitRust(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	bRustEffectTriggered = false;
}

