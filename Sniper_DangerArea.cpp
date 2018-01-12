// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "Sniper_DangerArea.h"
#include "Sniper_Bot.h"
#include "FPS_ProjectCharacter.h"


// Sets default values
ASniper_DangerArea::ASniper_DangerArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OverlapComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Overlap comp"));
	OverlapComp->bGenerateOverlapEvents = true;
	OverlapComp->OnComponentBeginOverlap.AddDynamic(this, &ASniper_DangerArea::TriggerEnter);
	OverlapComp->OnComponentEndOverlap.AddDynamic(this, &ASniper_DangerArea::TriggerExit);
	RootComponent = OverlapComp;


}

// Called when the game starts or when spawned
void ASniper_DangerArea::BeginPlay()
{
	// These should be spawned after the sniper bot has spawned
	
	Super::BeginPlay();
	
	UWorld* World = GetWorld();
	if (World)
	{
		// This should only hold one sniper bot
		TArray<AActor*> AllSniperBots;

		UGameplayStatics::GetAllActorsOfClass(World, ASniper_Bot::StaticClass(), AllSniperBots);

		for (AActor* Bot : AllSniperBots)
		{
			ASniper_Bot* Sniper = Cast<ASniper_Bot>(Bot);
			if (Sniper)
			{
				SniperBot = Sniper;
				break;
			}

		}

		ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0);
		if (Character)
		{
			AFPS_ProjectCharacter* Player = Cast<AFPS_ProjectCharacter>(Character);

		}

	}
	
}

// Called every frame
void ASniper_DangerArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SniperBot)
	{

	}

}

void ASniper_DangerArea::TriggerEnter(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(AFPS_ProjectCharacter::StaticClass()))
	{
		bIsPlayerInLoS = true;
		
		UWorld* World = GetWorld();
		{
			if (World)
			{
				if (!World->GetTimerManager().IsTimerActive(TriggerBoxTimerHandle))
				{
					World->GetTimerManager().SetTimer(TriggerBoxTimerHandle, this, &ASniper_DangerArea::OnSniperBotReact, SniperBotReactionTime, false);

				}
				
			}
		}

	}

}

void ASniper_DangerArea::TriggerExit(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(AFPS_ProjectCharacter::StaticClass()))
	{
		bIsPlayerInLoS = false;

		UWorld* World = GetWorld();
		if (World)
		{
			World->GetTimerManager().ClearTimer(TriggerBoxTimerHandle);
		}

		if (Player && SniperBot && SniperBot->BotController && SniperBot->BotController->GetFocusActor() == Player)
		{
			SniperBot->BotController->ClearFocus(EAIFocusPriority::Gameplay);
		}

	}
	

}

void ASniper_DangerArea::OnSniperBotReact()
{
	if (bIsPlayerInLoS)
	{
		
		if (SniperBot && Player)
		{
			
		}
		
	}

}

