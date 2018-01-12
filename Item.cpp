// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "Item.h"
#include "MyStatics.h"


// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Skeletal Mesh"));
	WeaponSkeletalMesh->SetVisibility(false);
	//WeaponSkeletalMesh->AttachTo(RootComponent);
	
	RootComponent = WeaponSkeletalMesh;

	TBox = CreateDefaultSubobject<USphereComponent>(TEXT("Box"));
	TBox->bGenerateOverlapEvents = true;
	TBox->OnComponentBeginOverlap.AddDynamic(this, &AItem::TriggerEnter);
	TBox->OnComponentEndOverlap.AddDynamic(this, &AItem::TriggerExit);
	TBox->AttachTo(RootComponent);

	SM_TBox = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Box Mesh"));
	SM_TBox->AttachTo(RootComponent);

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	// Add a delay preventing this weapon from being picked up. This is so when the player drops a weapon 
	// the weapon isn't picked up again straight away. Actually by just changing the players boolean 
	// bIsPickupUp to false when an item is picked up should do the trick
	FTimerHandle NoPickupTimerHandle;

	UWorld* World = GetWorld();

	if (World)
	{
		World->GetTimerManager().SetTimer(NoPickupTimerHandle, this, &AItem::MakePickupOk, 0.75f, false);
	
	}

	if (bShouldSimulatePhysics && WeaponSkeletalMesh)
	{
		WeaponSkeletalMesh->SetSimulatePhysics(true);
		WeaponSkeletalMesh->SetEnableGravity(true);
		WeaponSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}

}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MyPlayerController != NULL)	// can crash game without this
	{
		if (MyPlayerController->bIsPickingUp && bItemIsWithinRange)
		{
			Pickup();
		}
	}
}

void AItem::ToggleGlow(bool IsGlowing)
{
	if (SM_TBox)
	{
		SM_TBox->SetRenderCustomDepth(IsGlowing);
	}
	
	if (WeaponSkeletalMesh)
	{
		WeaponSkeletalMesh->SetRenderCustomDepth(IsGlowing);
	}
}

void AItem::PlayPickupSound(int32 WType)
{
	UMyStatics::PlayASound(MyPlayerController, PickupSound, MyPlayerController->GetActorLocation());
}

void AItem::Pickup()
{
	if (!bCanBePickedUp)
	{
		return;
	}
	
	if (!bIsWeaponPickup)
	{
		MyPlayerController->Inventory.Add(*ItemName);
		
		// Adding to inventory doesn't seem to work so doing this instead. Assumes the item is a keycard
		MyPlayerController->bHasKeyCard = true;

	}
	else
	{
		/* If picking up a weapon you get its clip ammo added to your non clip ammo if you already 
		   have picked it up, or you pick it up and start with its clip ammo
		*/
		// Player has this weapon as their equipped or other weapon. Increase their non clip ammo.
		// This section can re removed since on overlap it is decided whether to pickup ammo automatically
		if (MyPlayerController->EquippedWeapon == WeaponType || MyPlayerController->OtherWeapon == WeaponType)
		{
			MyPlayerController->AdjustCurrentNonClipAmmo(WeaponType, ClipAmmo);

			if (MyPlayerController->GetMaxNonClipAmmo(WeaponType) < MyPlayerController->GetCurrentNonClipAmmo(WeaponType))
			{
				ClipAmmo = MyPlayerController->GetMaxNonClipAmmo(WeaponType) - MyPlayerController->GetCurrentNonClipAmmo(WeaponType);
				MyPlayerController->AdjustCurrentNonClipAmmo(WeaponType, ClipAmmo);
			}

			// Generate HUD info message
			MyPlayerController->CreateFinalHUDInfoMessage(WeaponType, ClipAmmo, 2.5f);

			Destroy();

			return;

		}
		// Player does not have this weapon as one of their 2 weapons. It's clip ammo will not be picked up
		// unless the player picks up the weapon
		else 
		{
			/* If here the weapon will be picked up and equipped */

			// Make reloads not do anything
			if (MyPlayerController->bIsReloading)
			{
				MyPlayerController->bIsReloading = false;
				MyPlayerController->bHasRecentlySwitchedWeapons = true;
			}

			// Change aim animation info
			MyPlayerController->AssignAimAnimsOnWeaponChange(WeaponType);
			
			// Cancel reload animation if it is playing
			if (MyPlayerController->ReloadAnimInstance)
			{
				MyPlayerController->ReloadAnimInstance->Montage_Stop(0.15f);
			}

			/* Check if player already has 2 weapons. If they do then drop the equipped weapon */
			
			// Player does not have another weapon
			if (MyPlayerController->OtherWeapon == 255)
			{
				
				// Move the current equipped weapon to the other weapon and make 
				// this the equipped weapon
				MyPlayerController->OtherWeapon = MyPlayerController->EquippedWeapon;
				MyPlayerController->EquippedWeapon = WeaponType;
				
				// Make current clip ammo equal to the clip ammo in this pickup
				MyPlayerController->AdjustCurrentClipAmmo(WeaponType, -MyPlayerController->GetCurrentClipAmmo(WeaponType));
				MyPlayerController->AdjustCurrentClipAmmo(WeaponType, ClipAmmo);
				
				UpdatePlayerWeaponInfo(WeaponType);

				MyPlayerController->PlayWeaponSwitchAnimation(1.5f);

				PlayPickupSound(WeaponType);

			}

			// Player already holds 2 weapons. Swap out the equipped weapon for this weapon
			// If this is their first time picking it up they will be given full ammo values
			// and start with a full clip. Otherwise its a weapon they dropped before and 
			// will be given zero clip ammo and will have to reload immediately.
			else
			{
				
				// Drop the current equipped weapon back into the world
				DropEquippedWeapon(MyPlayerController->EquippedWeapon);

				// Make current clip ammo equal to the clip ammo in this pickup
				MyPlayerController->AdjustCurrentClipAmmo(WeaponType, -MyPlayerController->GetCurrentClipAmmo(WeaponType));
				MyPlayerController->AdjustCurrentClipAmmo(WeaponType, ClipAmmo);

				MyPlayerController->EquippedWeapon = WeaponType;
				UpdatePlayerWeaponInfo(WeaponType);

				MyPlayerController->PlayWeaponSwitchAnimation(1.5f);

				PlayPickupSound(WeaponType);

			}

		}

		/* Let the player fire straight away with the picked up weapon */
		MyPlayerController->ResetFire();
	}

	// This is so if a weapon is dropped it isn't picked up straight away, and so only one it within range 
	// can be picked up at a time
	MyPlayerController->bIsPickingUp = false;

	Destroy();

}

void AItem::GetPlayer(AActor* Player)
{
	MyPlayerController = Cast<AFPS_ProjectCharacter>(Player);
}

void AItem::MakePickupOk()
{
	bCanBePickedUp = true;

}

void AItem::UpdatePlayerWeaponInfo(uint8 Type)
{
	MyPlayerController->WeaponRateOfFire = MyPlayerController->RatesOfFire[Type];
	MyPlayerController->bEquippedWeaponIsAutomatic = MyPlayerController->AutomaticStatus[Type];
	MyPlayerController->ChangeWeaponMesh(Type);
	
}

void AItem::TriggerEnter(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AFPS_ProjectCharacter::StaticClass()))
	{
		// Pickup ammo but leave empty weapon there is this is one of the players 2 weapons
		if (MyPlayerController && bIsWeaponPickup)
		{
			if (MyPlayerController->EquippedWeapon == WeaponType || MyPlayerController->OtherWeapon == WeaponType)
			{

				if (MyPlayerController->GetMaxNonClipAmmo(WeaponType) < MyPlayerController->GetCurrentNonClipAmmo(WeaponType) && ClipAmmo > 0)
				{
					ClipAmmo = MyPlayerController->GetMaxNonClipAmmo(WeaponType) - MyPlayerController->GetCurrentNonClipAmmo(WeaponType);
					MyPlayerController->AdjustCurrentNonClipAmmo(WeaponType, ClipAmmo);
				}
				else
				{
					MyPlayerController->AdjustCurrentNonClipAmmo(WeaponType, ClipAmmo);
				}

				// Generate HUD info message
				if (ClipAmmo > 0)
				{
					MyPlayerController->CreateFinalHUDInfoMessage(WeaponType, ClipAmmo, 2.5f);
				}

				ClipAmmo = 0;

				return;
				
			}
		}
		
		bItemIsWithinRange = true;
		
		if (bCanGlow)
		{
			ToggleGlow(true);
		}
		
		if (MyPlayerController)
		{
			// Create HUD message
			MyPlayerController->HUDText = Message;

		}

	}
	
	if (!MyPlayerController)
	{
		GetPlayer(OtherActor);
	}
	
}

void AItem::TriggerExit(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{

	if (OtherActor->IsA(AFPS_ProjectCharacter::StaticClass()))
	{
		bItemIsWithinRange = false;
		
		if (bCanGlow)
		{
			ToggleGlow(false);
		}
		
		if (MyPlayerController)
		{
			if (MyPlayerController->HUDText == Message)
			{
				MyPlayerController->ClearHUDText();
			}
			
		}

	}
	
}

/* This method really should be in FPS_ProjectCharacter.cpp */
void AItem::DropEquippedWeapon(uint8 WType)
{
	UWorld* World = GetWorld();

	switch (WType)
	{
	// Drop laser rifle
	case 0:
		if (World)
		{
			if (MyPlayerController && BP_LaserRifle)
			{
				// Hide skeletal mesh on player
				MyPlayerController->FP_Gun->SetHiddenInGame(true);
				MyPlayerController->FP_Gun->SetVisibility(false);

				// Spawn laser rifle
				AItem* DroppedWeapon = World->SpawnActor<AItem>(BP_LaserRifle, 
					MyPlayerController->GetActorLocation() + MyPlayerController->GetActorForwardVector() * 100.f,
					MyPlayerController->GetActorRotation() + FRotator(45.f, 70.f, 0.f));

				if (DroppedWeapon)
				{
					// Mark the dropped weapon as a weapon that was dropped by the player
					DroppedWeapon->bHasBeenPickedUpBefore = true;

					// Make the dropped weapons clip ammo what was in the players clip before they dropped it
					DroppedWeapon->ClipAmmo = MyPlayerController->GetCurrentClipAmmo(WType);

					// Put the weapon in front of the player a little
					USkeletalMeshComponent* WeaponMesh = DroppedWeapon->WeaponSkeletalMesh;
					if (WeaponMesh)
					{
						WeaponMesh->SetSimulatePhysics(true);
						WeaponMesh->SetEnableGravity(true);
						WeaponMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
						WeaponMesh->AddForce(FVector(MyPlayerController->GetActorForwardVector()));
					}
				}

			}
		}
		break;

	// Drop auto rifle
	case 1:
		if (World)
		{
			if (MyPlayerController && BP_AutoRifle)
			{
				// Hide skeletal mesh on player
				MyPlayerController->FP_GunAutoRifle->SetHiddenInGame(true);
				MyPlayerController->FP_GunAutoRifle->SetVisibility(false);

				// Spawn auto rifle
				AItem* DroppedWeapon = World->SpawnActor<AItem>(BP_AutoRifle, 
					MyPlayerController->GetActorLocation() + MyPlayerController->GetActorForwardVector() * 100.f, 
					MyPlayerController->GetActorRotation() + FRotator(45.f, 70.f, 0.f));

				// Mark the dropped weapon as a weapon that was dropped by the player
				if (DroppedWeapon)
				{
					// Mark the dropped weapon as a weapon that was dropped by the player
					DroppedWeapon->bHasBeenPickedUpBefore = true;

					// Make the dropped weapons clip ammo what was in the players clip before they dropped it
					DroppedWeapon->ClipAmmo = MyPlayerController->GetCurrentClipAmmo(WType);

					// Apply some force to the dropped weapon so it falls in front of the player
					USkeletalMeshComponent* WeaponMesh = DroppedWeapon->WeaponSkeletalMesh;
					if (WeaponMesh)
					{
						WeaponMesh->SetSimulatePhysics(true);
						WeaponMesh->SetEnableGravity(true);
						WeaponMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
						WeaponMesh->AddForce(FVector(MyPlayerController->GetActorForwardVector()));
					}
				}

			}
		}
		break;

	// Drop grenade launcher
	case 2:
		if (World)
		{
			if (MyPlayerController && BP_GrenadeLauncher)
			{
				// Hide skeletal mesh on player
				MyPlayerController->FP_GunGrenadeLauncher->SetHiddenInGame(true);
				MyPlayerController->FP_GunGrenadeLauncher->SetVisibility(false);

				// Spawn grenade launcher
				AItem* DroppedWeapon = World->SpawnActor<AItem>(BP_GrenadeLauncher, 
					MyPlayerController->GetActorLocation() + MyPlayerController->GetActorForwardVector() * 100.f,
					MyPlayerController->GetActorRotation() + FRotator(45.f, 70.f, 0.f));

				if (DroppedWeapon)
				{
					// Mark the dropped weapon as a weapon that was dropped by the player
					DroppedWeapon->bHasBeenPickedUpBefore = true;

					// Make the dropped weapons clip ammo what was in the players clip before they dropped it
					DroppedWeapon->ClipAmmo = MyPlayerController->GetCurrentClipAmmo(WType);

					// Apply some force to the dropped weapon so it falls in front of the player
					USkeletalMeshComponent* WeaponMesh = DroppedWeapon->WeaponSkeletalMesh;
					if (WeaponMesh)
					{
						WeaponMesh->SetSimulatePhysics(true);
						WeaponMesh->SetEnableGravity(true);
						WeaponMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
						WeaponMesh->AddForce(FVector(MyPlayerController->GetActorForwardVector()));
					}
				}
			}
		}
		break;

	// Drop sniper rifle
	case 3:
		if (World)
		{
			if (MyPlayerController && BP_SniperRifle)
			{
				// Hide skeletal mesh on player
				MyPlayerController->FP_GunSniperRifle->SetHiddenInGame(true);
				MyPlayerController->FP_GunSniperRifle->SetVisibility(false);

				// Spawn sniper rifle
				AItem* DroppedWeapon = World->SpawnActor<AItem>(BP_SniperRifle, 
					MyPlayerController->GetActorLocation() + MyPlayerController->GetActorForwardVector() * 100.f,
					MyPlayerController->GetActorRotation() + FRotator(45.f, 70.f, 0.f));

				if (DroppedWeapon)
				{
					// Mark the dropped weapon as a weapon that was dropped by the player
					DroppedWeapon->bHasBeenPickedUpBefore = true;

					// Make the dropped weapons clip ammo what was in the players clip before they dropped it
					DroppedWeapon->ClipAmmo = MyPlayerController->GetCurrentClipAmmo(WType);

					// Apply some force to the dropped weapon so it falls in front of the player
					USkeletalMeshComponent* WeaponMesh = DroppedWeapon->WeaponSkeletalMesh;
					if (WeaponMesh)
					{
						WeaponMesh->SetSimulatePhysics(true);
						WeaponMesh->SetEnableGravity(true);
						WeaponMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
						WeaponMesh->AddForce(FVector(MyPlayerController->GetActorForwardVector()));
					}
				}
			}
		}
		break;

	default:
		break;
	}

}

