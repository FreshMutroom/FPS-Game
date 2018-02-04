// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "ChangeWorldSettingsTrigger.h"
#include "DamagableBot.h"


// Sets default values
AChangeWorldSettingsTrigger::AChangeWorldSettingsTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OverlapComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Overlap comp"));
	OverlapComp->bGenerateOverlapEvents = true;
	OverlapComp->OnComponentBeginOverlap.AddDynamic(this, &AChangeWorldSettingsTrigger::TriggerEnter);
	RootComponent = OverlapComp;

}

// Called when the game starts or when spawned
void AChangeWorldSettingsTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChangeWorldSettingsTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChangeWorldSettingsTrigger::TriggerEnter(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(AFPS_ProjectCharacter::StaticClass()))
	{
		UWorld* World = GetWorld();
		if (World)
		{
			AWorldSettings* WorldSettings = World->GetWorldSettings();
			if (WorldSettings)
			{
				WorldSettings->KillZ = NewZValue;

				// Also despawn all non sniper bot AI
				DestroyAI();

			}
		}
		
	}

}

void AChangeWorldSettingsTrigger::DestroyAI()
{
	TArray<AActor*> AllAI;

	UGameplayStatics::GetAllActorsWithInterface(this, UDamagableBot::StaticClass(), AllAI);

	for (AActor * Actor : AllAI)
	{
		Actor->Destroy();
	}

}

