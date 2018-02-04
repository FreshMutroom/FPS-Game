// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "Sniper_CrouchTrigger.h"
#include "Sniper_Bot.h"


// Sets default values
ASniper_CrouchTrigger::ASniper_CrouchTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Colision comp"));
	CollisionComp->bGenerateOverlapEvents = true;
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ASniper_CrouchTrigger::TriggerEnter);
	RootComponent = CollisionComp;

	bShouldCrouchBot = true;

}

// Called when the game starts or when spawned
void ASniper_CrouchTrigger::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASniper_CrouchTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASniper_CrouchTrigger::ResetTimerHandle()
{
}

void ASniper_CrouchTrigger::TriggerEnter(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!OverlapLock.TryLock())
	{
		return;
	}
	
	if (OtherActor->IsA(ASniper_Bot::StaticClass()))
	{
		if (!SniperBot)
		{
			SniperBot = Cast<ASniper_Bot>(OtherActor);
		}

		if (SniperBot)
		{
			UWorld* World = GetWorld();
			if (World)
			{
				if (World->GetTimerManager().IsTimerActive(TriggerCooldownTimerHandle))
				{
					return;
				}
				else
				{
					World->GetTimerManager().SetTimer(TriggerCooldownTimerHandle, this, &ASniper_CrouchTrigger::ResetTimerHandle, 4.f, false);

				}

			}
			
			if (bShouldCrouchBot)
			{
				SniperBot->GoCrouched();
				bShouldCrouchBot = false;
			}
			else
			{
				bShouldCrouchBot = true;
			}

		}

	}

	OverlapLock.Unlock();

}

