// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "Sniper_Bot.h"
#include "Sniper_VantagePoint.h"
#include "Sniper_Controller.h"
#include "Sniper_Projectile.h"
#include "MyGameInstance.h"
#include "FPS_ProjectCharacter.h"
#include "MyStatics.h"
#include "TutorialMessage.h"


// Sets default values
ASniper_Bot::ASniper_Bot()
{
	SniperRifleWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon mesh"));

	SniperRifleMuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Rifle muzzle location"));

	MuzzleFlash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Muzzle flash"));

	SmokeBomb_PS = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Smoke bomb PS"));

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Without this line setting focus to the default focus was not working. 
	AIControllerClass = ASniper_Controller::StaticClass();

	//bUseControllerRotationYaw = true;

	// Added by me
	//bUseControllerRotationPitch = true;

	// With aimspace bUseControllerRotationYaw  and pitch are false, and this line makes pawn face where
	// they are moving
	UCharacterMovementComponent* CharMoveComp = GetCharacterMovement();
	if (CharMoveComp)
	{
		CharMoveComp->bOrientRotationToMovement = true;
	}

	ProneToStandMontageDuration = 1.433f;

	StandToProneMontageDuration = 1.633f;

	StandToCrouchMontageDuration = 0.800f;

	CrouchToStandMontageDuration = 0.600f;

	FireProneMontageDuration = 0.233f;

	ReloadProneMontageDuration = 4.3f * (1.f / 0.8f);

	ReloadCrouchedMontageDuration = 4.3f * (1.f / 0.8f);

	ZeroHealthMontageDuration = 1.900f;

	LoSCheckRate = 0.1f;

	Health = 3;

	MaxClipAmmo = 5;
	ClipAmmo = MaxClipAmmo - 1;

}

// Called when the game starts or when spawned
void ASniper_Bot::BeginPlay()
{
	Super::BeginPlay();

	AController* Controller = GetController();
	if (Controller)
	{
		BotController = Cast<ASniper_Controller>(Controller);
	}

	if (GetMesh() && SniperRifleWeaponMesh)
	{
		SniperRifleWeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("hand_rSocket"));
		SniperRifleWeaponMesh->SetRelativeRotation(RifleRotationOffset);
		SniperRifleWeaponMesh->SetRelativeLocation(RifleLocationOffset);

	}

	if (SniperRifleWeaponMesh)
	{
		SniperRifleMuzzleLocation->AttachTo(SniperRifleWeaponMesh);
		SniperRifleMuzzleLocation->SetRelativeRotation(MuzzleRotationOffset);
		SniperRifleMuzzleLocation->SetRelativeLocation(MuzzleLocationOffset);
	}
	
}

// Called every frame
void ASniper_Bot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!Player)
	{
		ACharacter * Character = UGameplayStatics::GetPlayerCharacter(this, 0);
		if (Character)
		{
			Player = Cast<AFPS_ProjectCharacter>(Character);
		}
	}

	//bool bIsProne;
	//bool bCanSeePlayer;
	//bool bIsReloading;
	//bool bReadyToMoveLocation;
	//bool bIsChangingLocation;
	//bool bKnowsPlayerHasSniperRifle;
	//bool bHasBeenRecentlyHit

	if (bHasZeroHealth)
	{
		return;
	}
	
	if (bReadyToMoveLocation && !bCanSeePlayer && !bIsReloading)
	{
		// Move to new vantage point
		SelectNewVantagePoint();

		LoSCheckTotal = LoSTotal = 0.f;

		return;

	}

	LoSCheckTotal += DeltaTime;

	if (LoSCheckTotal >= LoSCheckRate)
	{
		
		// Check for LoS. If only doing CheckLoSToPlayer there are spots where the playre can hit the bot but the 
		// bot cannot see the player. These are spots such as near rocks and through some tree leaves
		if (CheckLoSToPlayer() || CheckLoSFromPlayer())
		{
			// Player is in LoS
			
			bCanSeePlayer = true;

			LoSTotal += LoSCheckTotal;
			
			if (LoSTotal >= LookAtTime)
			{
				// Turn to face player
				if (BotController && Player && !bIsReloading && !bIsChangingLocation)
				{
					BotController->SetFocus(Player);
				}

			}

			if (bIsReloading)
			{
				// This line ensures the bot does not fire the instant they stop reloading
				LoSTotal = LookAtTime;
				return;
			}

			// Fire if player has been out in the open long enough or if they are aiming down sight with sniper rifle
			if (LoSTotal >= OpenFireTime || IsPlayerAimingWithSniper())
			{
				
				if (!bIsReloading && !bIsChangingLocation && !bHasBeenRecentlyHit)
				{
					if (Player && Player->EquippedWeapon == 3)
					{
						OnHasSeenRifle();

					}
					
					// Fire at player
					FireAtPlayer();
					LoSTotal = OpenFireTime * 0.0f;	// change this to another value to make second shot come sooner/later

					PlayRifleFireSoundAfterDelay();

				}

			}

		}
		else
		{
			bCanSeePlayer = false;
			
			LoSTotal -= (LoSCheckTotal * 1.0f);
			if (LoSTotal < 0.f)
			{
				LoSTotal = 0.f;
			}
		}

		LoSCheckTotal = 0.f;

	}

}

// Called to bind functionality to input
void ASniper_Bot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASniper_Bot::FaceRotation(FRotator NewRotation, float DeltaTime)
{
	FRotator CurrentRotation = FMath::RInterpTo(GetActorRotation(), NewRotation, DeltaTime, RotationSpeed);	// last param changed from 8.0f

	Super::FaceRotation(CurrentRotation, DeltaTime);

}

void ASniper_Bot::SelectNewVantagePoint()
{
	if (!bReadyToMoveLocation || bIsChangingLocation)
	{
		// Do not allow another move command to go through if already doing one
		return;
	}
	
	bReadyToMoveLocation = false;
	bIsChangingLocation = true;

	if (SnipingLocations.Num() >= 3)
	{
		int32 RandomIndex = FMath::RandRange(0, SnipingLocations.Num() - 1);

		if (SnipingLocations.IsValidIndex(RandomIndex))
		{
			AActor* Location = SnipingLocations[RandomIndex];
			ASniper_VantagePoint* VP = nullptr;
			if (Location)
			{
				VP = Cast<ASniper_VantagePoint>(Location);
			}

			if (VP && (Location == CurrentVantagePoint || Location == PreviousVantagePoint || !VP->bIsActive))
			{
				while (true)
				{
					// This could potentially go forever
					RandomIndex = FMath::RandRange(0, SnipingLocations.Num() - 1);

					if (SnipingLocations.IsValidIndex(RandomIndex))
					{
						Location = SnipingLocations[RandomIndex];
						VP = nullptr;
						VP = Cast<ASniper_VantagePoint>(Location);
						
						if (VP && Location != CurrentVantagePoint && Location != PreviousVantagePoint && VP->bIsActive)
						{
							NewVantagePoint = SnipingLocations[RandomIndex];
							MoveToNewVantagePoint();

							return;
						}

					}

				}

			}
			else
			{
				NewVantagePoint = SnipingLocations[RandomIndex];
				MoveToNewVantagePoint();
			}
		}

	}

}

void ASniper_Bot::MoveToNewVantagePoint()
{
	UWorld* World = GetWorld();
	
	float MontageDuration;

	if (bIsProne)
	{
		PlayMontage(ProneToStandMontage);
		MontageDuration = ProneToStandMontageDuration + 0.5f;

	}
	else
	{
		// assuming crouched
		PlayMontage(CrouchToStandMontage);
		MontageDuration = CrouchToStandMontageDuration + 0.5f;

	}
			
	
	if (World)
	{
		World->GetTimerManager().SetTimer(AnimationTimerHandle, this, &ASniper_Bot::IssueMoveCommand, MontageDuration, false);

	}

}

void ASniper_Bot::IssueMoveCommand()
{
	if (NewVantagePoint)
	{
		if (BotController)
		{
			BotController->ClearFocus(EAIFocusPriority::Gameplay);
			
			ChangeMoveSpeed(RunSpeed);
			
			PlayMontage(RunMontage);
			
			if (Health == 1)
			{
				// Drop smoke bomb if low on health
				DropSmokeBomb();
			}

			BotController->MoveToLocation(NewVantagePoint->GetActorLocation(), -1.f, true, true, false, false);

			// Change the values of previous and current vantage point
			PreviousVantagePoint = CurrentVantagePoint;
			CurrentVantagePoint = NewVantagePoint;

			CheckProgressOfMoveCommand();

			// To make bot crouch when near the vantage point the bot could pass through a 
			// trigger box that does this

		}
	}

}

void ASniper_Bot::CheckProgressOfMoveCommand()
{
	if (BotController)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			if (BotController->GetMoveStatus() != EPathFollowingStatus::Moving && !bHasBeenRecentlyHit)
			{
				// Has arrived at new sniping location
				
				// Magically restore clip ammo if bot knows player has sniper rifle
				if (bKnowsPlayerHasSniperRifle)
				{
					ClipAmmo = MaxClipAmmo;
				}
				
				ResumeSnipingRoutine();
			}
			else
			{
				FTimerHandle DummyTimerHandle;
				World->GetTimerManager().SetTimer(DummyTimerHandle, this, &ASniper_Bot::CheckProgressOfMoveCommand, 0.1f, false);
				
			}
		}
		
	}
	
	

}

void ASniper_Bot::ResumeSnipingRoutine()
{
	if (BotController && DefaultFocus)
	{

		BotController->SetFocalPoint(DefaultFocus->GetActorLocation());
	}

	if (CurrentVantagePoint)
	{
		
		ASniper_VantagePoint* VP = Cast<ASniper_VantagePoint>(CurrentVantagePoint);
		UWorld* World = GetWorld();

		if (VP && World)
		{
			
			FTimerHandle DummyTimerHandle;

			if (VP->bShouldGoProne)
			{
				bIsProne = true;
				PlayMontage(StandToProneMontage);

				// Time is time it takes to go from standing to prone and set up at new vantage point
				World->GetTimerManager().SetTimer(DummyTimerHandle, this, &ASniper_Bot::OnChangeLocationFinish, StandToProneMontageDuration + 0.2f, false);


			}
			else
			{
				PlayMontage(CrouchedIdleMontage);

				// Time is time it takes to set up at new vantage point when crouched
				World->GetTimerManager().SetTimer(DummyTimerHandle, this, &ASniper_Bot::OnChangeLocationFinish, 0.2f, false);

			}
		}

	}

}

void ASniper_Bot::OnChangeLocationFinish()
{
	bIsChangingLocation = false;

	UWorld* World = GetWorld();
	if (World)
	{
		// Move to a new location after a while. Firing at player is handled by trigger boxes spawned in world
		float Duration = FMath::RandRange(MinSnipingLocationTime, MaxSnipingLocationTime);

		World->GetTimerManager().SetTimer(MoveTimerHandle, this, &ASniper_Bot::MakeReadyToMoveLocationTrue, Duration, false);

	}

}

bool ASniper_Bot::CheckLoSToPlayer()
{
	// Move this to tick, then remove timer handles. If player has line of sight to bot then this 
	// may help with LoS checks too to determine if bot can see player

	if (BotController && Player)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			
			if (BotController->LineOfSightTo(Player, FVector(0.f)))
			{
				// Player is in LoS
				return true;

			}
			
		}
		
	}

	return false;

}

bool ASniper_Bot::CheckLoSFromPlayer()
{
	// Best use a raycast here that only hits blocking volumes and ignores things like leaves
	// Maybe, depends if I want trees to block line of sight or not
	
	// This needs to be a custom raycast. LineOfSightTo will not work when hiding behind rocks for example


	// raycast code starts here

	// First get distance from player
	if (Player)
	{
		const FVector FromPlayer = GetActorLocation() - Player->GetActorLocation();
		const float Distance = FromPlayer.Size();
		
		FVector StartTrace;

		UCameraComponent* PlayerCamera = Player->GetFirstPersonCameraComponent();
		if (PlayerCamera)
		{
			StartTrace = PlayerCamera->GetComponentLocation();
		}
		
		FVector EndTrace = GetActorLocation();

		FHitResult HitResult = FHitResult();
		FCollisionQueryParams CQP = FCollisionQueryParams();
		CQP.AddIgnoredActor(Player);

		UWorld* World = GetWorld();
		if (World)
		{
			if (World->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Visibility, CQP))
			{
				// Raycast has hit something
				if (HitResult.GetActor() && HitResult.GetActor()->IsA(ASniper_Bot::StaticClass()))
				{
					// Raycast hit sniper bot
					return true;
				}
				else
				{
					return false;
				}
			}
			
		}

	}
	
	return false;

	// raycast code stops here

}

void ASniper_Bot::OnHasSeenRifle()
{
	if (!bKnowsPlayerHasSniperRifle && BP_SeenSniperRifleMessage)
	{
		UWorld * World = GetWorld();
		if (World)
		{
			ATutorialMessage * Message = World->SpawnActor<ATutorialMessage>(BP_SeenSniperRifleMessage, FVector(0.f, 0.f, 11000.f), FRotator(0.f));
			if (Message)
			{
				Message->CreateMessage();
			}
		}

	}

	bKnowsPlayerHasSniperRifle = true;

}

bool ASniper_Bot::IsPlayerAimingWithSniper()
{
	if (Player)
	{
		if (Player->EquippedWeapon == 3 && !Player->bIsAimAnim1st)
		{
			return true;
		}
	}
	
	return false;
}

void ASniper_Bot::FireAtPlayer()
{
	if (Player && Player->CurrentHealth > 0.f && BotController)
	{
		// Line of sight check from bot to player removed
			
		// Actually fire shot
			
		ClipAmmo--;

		if (bIsProne)
		{
			PlayMontage(FireProneMontage);
		}
		else
		{
			PlayMontage(FireCrouchedMontage);
		}
			
		UWorld* World = GetWorld();
		if (World && BP_Projectile && SniperRifleMuzzleLocation)
		{
			World->SpawnActor<ASniper_Projectile>(BP_Projectile, SniperRifleMuzzleLocation->GetComponentToWorld().GetLocation(), SniperRifleMuzzleLocation->GetComponentRotation());
			//World->SpawnActor<ASniper_Projectile>(BP_Projectile, SniperRifleWeaponMesh->GetComponentLocation(), SniperRifleMuzzleLocation->GetComponentRotation());

		}

		float Damage = FMath::RandRange(39.f, 49.f);
	
		// Make sure player takes full damage no matter what difficulty
		float Multiplier = 1.f;
		UGameInstance * GameInstance = UGameplayStatics::GetGameInstance(this);
		if (GameInstance)
		{
			UMyGameInstance * MyGameInstance = Cast<UMyGameInstance>(GameInstance);
			if (MyGameInstance)
			{
				Multiplier /= MyGameInstance->DamageMultiplier;
			}
		}
		Player->ReceiveDamage(Multiplier * Damage);

		// Play sound
			
		// Muzzle flash
		if (SniperRifleMuzzleLocation && MuzzleFlash && World)
		{
			UParticleSystemComponent* MuzzleParticles = UGameplayStatics::SpawnEmitterAtLocation(World, MuzzleFlash->Template, SniperRifleMuzzleLocation->GetComponentTransform(), true);
			MuzzleParticles->SetWorldScale3D(FVector(0.2f));
		}


		// Knock player out of scope if they are in it
		Player->KnockOutOfScope();

		if (((Player->CurrentHealth / Player->MaxHealth > 0.39f && ClipAmmo == 1) ||
			(Player->CurrentHealth / Player->MaxHealth > 0.39f * 2 && ClipAmmo == 2))
			&& bKnowsPlayerHasSniperRifle)
		{
			// Player is still quite healthy, bot only has one round left and knows player has sniper rifle.
			// Bot will relocate
			MakeForcedMove();

			return;

		}
		else
		{
			if (ClipAmmo == 0)
			{
				if (bKnowsPlayerHasSniperRifle)
				{
					// Maybe put a small delay here
					MakeForcedMove();

				}
				else
				{
					Reload();
				}
			}
		}
		
	}

}

void ASniper_Bot::Reload()
{
	if (bIsReloading)
	{
		return;
	}
	
	bIsReloading = true;
	
	UWorld* World = GetWorld();
	if (World)
	{
		FTimerHandle DummyTimerHandle;
		
		if (bIsProne)
		{
			PlayMontage(ReloadProneMontage);
			World->GetTimerManager().SetTimer(DummyTimerHandle, this, &ASniper_Bot::OnReloadFinish, ReloadProneMontageDuration, false);

		}
		else
		{
			PlayMontage(ReloadCrouchedMontage);
			World->GetTimerManager().SetTimer(DummyTimerHandle, this, &ASniper_Bot::OnReloadFinish, ReloadCrouchedMontageDuration, false);

		}
		
	}

}

void ASniper_Bot::OnReloadFinish()
{
	ClipAmmo = MaxClipAmmo;

	bIsReloading = false;

	// Make sure both crouched and prone reload montages loop idle afterwards so no montage needs to play here

}

void ASniper_Bot::MakeReadyToMoveLocationTrue()
{
	bReadyToMoveLocation = true;

}

void ASniper_Bot::OnHit()
{
	UWorld* World = GetWorld();
	
	bHasBeenRecentlyHit = true;

	Health--;

	if (Health <= 0)
	{
		// destroyed
		OnZeroHealth();

		return;
	}
	else if (Health == 1)
	{
		FTimerHandle DummyTimerHandle;
		World->GetTimerManager().SetTimer(DummyTimerHandle, this, &ASniper_Bot::DropSmokeBomb, 0.1f, false);

	}

	if (BotController)
	{
		
		//BotController->StopMovement();
		//
		//if (!bIsProne)
		//{
		//	PlayMontage(TakeDamageMontage);
		//
		//	if (World)
		//	{
		//		FTimerHandle DummyTimerHandle;
		//		World->GetTimerManager().SetTimer(DummyTimerHandle, this, &ASniper_Bot::PlayRunMontage, 0.2f, false);
		//
		//	}
		//
		//}
		
	}

	// Move location
	if (World)
	{
		// Reset the timer handle that would make bot move location
		if (World->GetTimerManager().IsTimerActive(MoveTimerHandle))
		{
			World->GetTimerManager().ClearTimer(MoveTimerHandle);
		}
		
		FTimerHandle DummyTimerHandle;
		World->GetTimerManager().SetTimer(DummyTimerHandle, this, &ASniper_Bot::MakeForcedMove, 0.2f, false);

		World->GetTimerManager().SetTimer(RecentHitTimerHandle, this, &ASniper_Bot::ResetHasBeenRecentlyHit, 0.5f, false);

	}

}

void ASniper_Bot::DropSmokeBomb()
{
	if (SmokeBomb_PS && SmokeBomb_PS->Template)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			UGameplayStatics::SpawnEmitterAtLocation(World, SmokeBomb_PS->Template, GetActorTransform(), true);

		}
		
	}

}

void ASniper_Bot::ResetHasBeenRecentlyHit()
{
	bHasBeenRecentlyHit = false;

}

void ASniper_Bot::PlayRunMontage()
{
	PlayMontage(RunMontage);

}

void ASniper_Bot::MakeForcedMove()
{
	// This line should cancel any moves in progress. Actually I don't think it is needed, and it only 
	// makes the bot fire while moving
	//bIsChangingLocation = false;
	
	bReadyToMoveLocation = true;
	bIsReloading = false;
	bCanSeePlayer = false;	// May not be a correct statement. This is to get the bot to move lcoation on tick

}

void ASniper_Bot::PlayMontage(UAnimMontage * Montage)
{
	if (Montage)
	{
		USkeletalMeshComponent* BotMesh = GetMesh();
		if (BotMesh)
		{
			UAnimInstance* AnimInst = BotMesh->GetAnimInstance();
			if (AnimInst)
			{
				// With a blend out time of 0.25f on all montages these 5 lines make no difference

				//UAnimMontage* CurrentMontage = AnimInst->GetCurrentActiveMontage();
				//if (CurrentMontage)
				//{
				//	AnimInst->Montage_Stop(0.25f);
				//}

				AnimInst->Montage_Play(Montage);
			}
		}

	}

}

void ASniper_Bot::GoCrouched()
{
	bIsProne = false;
	
	UCharacterMovementComponent* CharMoveComp = GetCharacterMovement();
	if (CharMoveComp)
	{
		CharMoveComp->MaxWalkSpeed = 0.f;
	}
	
	PlayMontage(StandToCrouchMontage);

	UWorld* World = GetWorld();
	if (World)
	{
		FTimerHandle DummyTimerHandle;
		World->GetTimerManager().SetTimer(DummyTimerHandle, this, &ASniper_Bot::CrouchedWalk, StandToCrouchMontageDuration, false);

	}

}

void ASniper_Bot::CrouchedWalk()
{
	UCharacterMovementComponent* CharMoveComp = GetCharacterMovement();
	if (CharMoveComp)
	{
		CharMoveComp->MaxWalkSpeed = CrouchSpeed;
	}
	
	PlayMontage(CrouchedWalkMontage);

}

void ASniper_Bot::ChangeMoveSpeed(float NewMovespeed)
{
	UCharacterMovementComponent* CharMoveComp = GetCharacterMovement();
	if (CharMoveComp)
	{
		CharMoveComp->MaxWalkSpeed = NewMovespeed;
	}

}

void ASniper_Bot::OnZeroHealth()
{
	if (bHasZeroHealth)
	{
		return;
	}

	if (BotController)
	{
		BotController->StopMovement();
	}
	
	bHasZeroHealth = true;
	PlayMontage(ZeroHealthMontage);

	UWorld* World = GetWorld();
	if (World)
	{
		FTimerHandle DummyTimerHandle;
		World->GetTimerManager().SetTimer(DummyTimerHandle, this, &ASniper_Bot::FreezeAnimations, ZeroHealthMontageDuration * 0.75f, false);

		if (Player)
		{
			FTimerHandle LevelCompleteTimerHandle;
			World->GetTimerManager().SetTimer(LevelCompleteTimerHandle, Player, &AFPS_ProjectCharacter::OnSniperBotDefeated, 2.f, false);

		}

	}

}

void ASniper_Bot::FreezeAnimations()
{
	if (GetMesh())
	{
		GetMesh()->bNoSkeletonUpdate = true;
	}

}

void ASniper_Bot::PlayRifleFireSoundAfterDelay()
{
	UWorld * World = GetWorld();
	if (World && Player)
	{
		
		// Add delay to sound due to speed of sound being 343 m/s
		float Distance = (GetActorLocation() - Player->GetActorLocation()).Size();
		float Delay = Distance / 34300.f;

		FTimerHandle DummyTimerHandle;
		World->GetTimerManager().SetTimer(DummyTimerHandle, this, &ASniper_Bot::PlayRifleFireSound, Delay, false);

	}

}

void ASniper_Bot::PlayRifleFireSound()
{
	UMyStatics::PlayASound(this, RifleSound, GetActorLocation());

}

void ASniper_Bot::Init()
{
	UWorld* World = GetWorld();
	if (World)
	{
		ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0);
		if (Character)
		{
			Player = Cast<AFPS_ProjectCharacter>(Character);
		}

	}
	
	PopulateSnipingLocations();

	// Get default focus actor
	// This code causes editor to crash lots on recompiles, or so I thought

	if (World)
	{
		
		TArray<AActor*> FocusPoint;
	
		UGameplayStatics::GetAllActorsOfClass(World, AInvisibleMesh::StaticClass(), FocusPoint);
	
		for (AActor* Actor : FocusPoint)
		{
			
			DefaultFocus = Actor;
			break;
		}

	}

	// Begin behavior. Replace this with MoveToStartingVantagePoint()
	//MakeReadyToMoveLocationTrue();
	MoveToStartingVantagePoint();

}

void ASniper_Bot::PopulateSnipingLocations()
{
	UWorld* World = GetWorld();
	if (World)
	{
		/* Populate sniper location array */
		UGameplayStatics::GetAllActorsOfClass(World, ASniper_VantagePoint::StaticClass(), SnipingLocations);

	}
	
}

void ASniper_Bot::MoveToStartingVantagePoint()
{
	bReadyToMoveLocation = false;
	bIsChangingLocation = true;

	FVector TargetLocation = FVector(-50320.f, 18929.f, -1866.f);

	for (AActor * Location : SnipingLocations)
	{
		if (Location->GetActorLocation() == TargetLocation)
		{
			NewVantagePoint = Location;
			IssueMoveCommand();

		}
	}

}

