// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "TriggerEvent.h"
#include "FPS_ProjectCharacter.h"
#include "TutorialMessage.h"
#include "BotSpawnPoint.h"


// Sets default values
ATriggerEvent::ATriggerEvent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	TriggerSphere->bGenerateOverlapEvents = true;
	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &ATriggerEvent::TriggerEnter);
	TriggerSphere->OnComponentEndOverlap.AddDynamic(this, &ATriggerEvent::TriggerExit);
	RootComponent = TriggerSphere;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetAbsolute(true, true, true);
	Mesh->SetupAttachment(RootComponent);

	Delay = 1.f;

}

// Called when the game starts or when spawned
void ATriggerEvent::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();
	if (World)
	{
		ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0);
		if (Character)
		{
			Player = Cast<AFPS_ProjectCharacter>(Character);
		}
	}

}

// Called every frame
void ATriggerEvent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Player && bIsPlayerInRange && !bHasBeenActivated)
	{
		Player->CreateHUDMessage(HUDMessage, false, false);

		if (Player->bIsInteracting)
		{
			bHasBeenActivated = true;
			
			Player->bCanHUDTextBeOverwritten = true;
			Player->ClearHUDText();

			Player->bHasAccessedComputer = true;

			if (TypewriterMessage)
			{
				
				UWorld* World = GetWorld();
				if (World)
				{
					ATutorialMessage* AsTutorialMessage = World->SpawnActor<ATutorialMessage>(TypewriterMessage, FVector(0.f), FRotator(0.f));
					if (AsTutorialMessage)
					{
						// Play typewriter message
						AsTutorialMessage->CreateMessage();

					}
				}
				
			}

			UWorld* World = GetWorld();
			if (World)
			{
				FTimerHandle SpawnBotsTimerHandle;
				World->GetTimerManager().SetTimer(SpawnBotsTimerHandle, this, &ATriggerEvent::MakeSpawnPointsActive, Delay, false);

			}
			

		}
	}

}

void ATriggerEvent::MakeSpawnPointsActive()
{
	UWorld* World = GetWorld();
	if (World)
	{
		TArray<AActor*> AllSpawnPoints;
		
		UGameplayStatics::GetAllActorsOfClass(World, ABotSpawnPoint::StaticClass(), AllSpawnPoints);

		for (AActor* Actor : AllSpawnPoints)
		{
			ABotSpawnPoint* SpawnPoint = Cast<ABotSpawnPoint>(Actor);
			if (SpawnPoint)
			{
				SpawnPoint->Start();
			}
		}

	}

}

void ATriggerEvent::TriggerEnter(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(AFPS_ProjectCharacter::StaticClass()))
	{
		bIsPlayerInRange = true;
		
		if (!Player)
		{
			Player = Cast<AFPS_ProjectCharacter>(OtherActor);
		}

	}

}

void ATriggerEvent::TriggerExit(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(AFPS_ProjectCharacter::StaticClass()))
	{
		bIsPlayerInRange = false;
		
		if (Player)
		{
			if (Player->HUDText == HUDMessage)
			{
				Player->bCanHUDTextBeOverwritten = true;
				Player->ClearHUDText();
			}
		}
	}

}

