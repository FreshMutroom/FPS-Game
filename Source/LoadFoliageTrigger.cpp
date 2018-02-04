// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "LoadFoliageTrigger.h"
#include "Runtime/Foliage/Public/InstancedFoliageActor.h"


// Sets default values
ALoadFoliageTrigger::ALoadFoliageTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LoadTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LoadTriggerBox"));
	LoadTriggerBox->bGenerateOverlapEvents = true;
	LoadTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ALoadFoliageTrigger::Load);
	RootComponent = LoadTriggerBox;

	UnloadTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("UnloadTriggerBox"));
	UnloadTriggerBox->bGenerateOverlapEvents = true;
	UnloadTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ALoadFoliageTrigger::Unload);
	UnloadTriggerBox->AttachTo(RootComponent);

}

// Called when the game starts or when spawned
void ALoadFoliageTrigger::BeginPlay()
{
	Super::BeginPlay();

	/* Only way I can get foliage get assigned the right value - add delay in checking for foliage in world. 
	   Since foliage sometimes hasn't loaded yet this has to be done but may not be enough time depending 
	   on how fast load times are. */
	UWorld * World = GetWorld();
	if (World)
	{
		FTimerHandle DummyTimerHandle;
		World->GetTimerManager().SetTimer(DummyTimerHandle, this, &ALoadFoliageTrigger::AssignFoliage, 20.f, false);

	}
}

// Called every frame
void ALoadFoliageTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

}

void ALoadFoliageTrigger::AssignFoliage()
{
	if (bHasSearchedAllActors)
	{
		return;
	}

	bHasSearchedAllActors = true;

	UWorld* World = GetWorld();
	if (World)
	{
		TArray<AActor*> AllActors;

		UGameplayStatics::GetAllActorsOfClass(World, AInstancedFoliageActor::StaticClass(), AllActors);
		
		for (AActor* Actor : AllActors)
		{
			if (Index == 1 && Actor->bHidden)
			{
				AInstancedFoliageActor* FoliageInstance = Cast<AInstancedFoliageActor>(Actor);
				Foliage = FoliageInstance;
				break;
			}
			// Only way to know foliage is for part 1: CanBeInCluster() is true
			else if (Index == 2 && Actor->CanBeInCluster())	
			{
				
				AInstancedFoliageActor* FoliageInstance = Cast<AInstancedFoliageActor>(Actor);
				Foliage = FoliageInstance;
				break;
				
				
			}
			
		}
		//if (GEngine)
		//{
		//	GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Purple, FString::Printf(TEXT("i = %d, from LoadFoliageTrigger.cpp"), i));
		//}
		//
		///* The first part of the or statement only works in editor but leaving it in anyway */
		//if (FoliageInstance && FoliageInstance->GetName() == FoliageName || i == Index)
		//{
		//	Foliage = FoliageInstance;
		//	break;
		//}
		//
		//i++;

		//}

	}

}

void ALoadFoliageTrigger::Load(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(AFPS_ProjectCharacter::StaticClass()))
	{
		if (!bIsFoliageVisible)
		{
			bIsFoliageVisible = true;

			SetHidden(false);
			
		}
	}

}

void ALoadFoliageTrigger::Unload(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(AFPS_ProjectCharacter::StaticClass()))
	{
		
		if (bIsFoliageVisible)
		{
			bIsFoliageVisible = false;
			
			SetHidden(true);
			
		}
	}

}

