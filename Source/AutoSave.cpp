// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "AutoSave.h"
#include "FPS_ProjectCharacter.h"


// Sets default values
AAutoSave::AAutoSave()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->bGenerateOverlapEvents = true;
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AAutoSave::TriggerEnter);
	RootComponent = TriggerBox;

}

// Called when the game starts or when spawned
void AAutoSave::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAutoSave::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AAutoSave::CheckIfAutoRifleInInventory(AFPS_ProjectCharacter* Player)
{
	if (bChecksForAutoRifle)
	{
		if (Player->EquippedWeapon == 1 || Player->OtherWeapon == 1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
	
}

bool AAutoSave::CheckIfKeyCardInInventory(AFPS_ProjectCharacter * Player)
{
	if (bChecksForKeyCard)
	{
		if (Player->bHasKeyCard)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
	
	
}

bool AAutoSave::MakeChecks(AFPS_ProjectCharacter * Player)
{
	if (CheckIfKeyCardInInventory(Player) && CheckIfAutoRifleInInventory(Player))
	{
		return true;
	}
	else
	{
		return false;
	}
	
}

void AAutoSave::TriggerEnter(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!bHasBeenTriggered)
	{
		if (OtherActor->IsA(AFPS_ProjectCharacter::StaticClass()))
		{
			
			AFPS_ProjectCharacter* Player = Cast<AFPS_ProjectCharacter>(OtherActor);
			if (Player)
			{
				if (MakeChecks(Player))
				{
					bHasBeenTriggered = true;
					Player->SaveGame();
				}
				
			}

		}
	}

}

