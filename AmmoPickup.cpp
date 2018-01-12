// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "AmmoPickup.h"


// Sets default values
AAmmoPickup::AAmmoPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ammo mesh"));
	RootComponent = AmmoMesh;

	AmmoCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Pickup range sphere"));
	AmmoCollision->bGenerateOverlapEvents = true;
	AmmoCollision->OnComponentBeginOverlap.AddDynamic(this, &AAmmoPickup::TriggerEnter);
	AmmoCollision->OnComponentEndOverlap.AddDynamic(this, &AAmmoPickup::TriggerExit);
	AmmoCollision->AttachTo(RootComponent);

	bCanBePickedUp = true;

}

// Called when the game starts or when spawned
void AAmmoPickup::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AAmmoPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// This first statement could be moved to a "Right after begin play" function
	if (!Player)
	{
		Player = Cast<AFPS_ProjectCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}

	else if (bIsPlayerInRange && bCanBePickedUp)
	{
		if (Player->GetCurrentNonClipAmmo(Type) < Player->GetMaxNonClipAmmo(Type))
		{
			// Player has room for ammo. Pickup ammo

			int Difference = Player->GetMaxNonClipAmmo(Type) - Player->GetCurrentNonClipAmmo(Type);

			if (Difference < Amount)
			{
				Amount = Difference;
			}

			Player->AdjustCurrentNonClipAmmo(Type, Amount);
			
			Player->CreateFinalHUDInfoMessage(Type, Amount, 2.5f);

			if (!bRespawns)
			{
				Destroy();
			}
			else
			{
				bCanBePickedUp = false;

				if (AmmoMesh)
				{
					AmmoMesh->SetVisibility(false);

					UWorld* World = GetWorld();
					if (World)
					{
						FTimerHandle DummyTimerHandle;
						World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AAmmoPickup::Respawn, RespawnDelay, false);

					}

				}

			}

		}
		
	}

}

void AAmmoPickup::Respawn()
{
	if (AmmoMesh)
	{
		AmmoMesh->SetVisibility(true);
		bCanBePickedUp = true;
	}

}

void AAmmoPickup::TriggerEnter(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(AFPS_ProjectCharacter::StaticClass()))
	{
		bIsPlayerInRange = true;

		if (Player && bCanBePickedUp)
		{
			if (Player->GetCurrentNonClipAmmo(Type) >= Player->GetMaxNonClipAmmo(Type))
			{
				// Ammo is at max. Change the players HUD warning message
				Player->CreateFinalHUDInfoMessage(Type + 4, 0, 2.5f);	

			}

		}

	}

}

void AAmmoPickup::TriggerExit(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(AFPS_ProjectCharacter::StaticClass()))
	{
		bIsPlayerInRange = false;
	}

}

