// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "Extreme_Bot.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTreeComponent.h"	// may not need this
#include "Extreme_BotController.h"


// Sets default values
AExtreme_Bot::AExtreme_Bot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// A variable not found in Extreme_Bot.h
	AIControllerClass = AExtreme_BotController::StaticClass();

	// A variable not found in Extreme_Bot.h
	bUseControllerRotationYaw = true;

	// Added by me
	bUseControllerRotationPitch = true;

	// Initialize senses
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	PawnSensingComp->SetPeripheralVisionAngle(70.f);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));

	GetCharacterMovement()->UseAccelerationForPathFollowing();
	GetCharacterMovement()->MaxAcceleration = 500.f;	// may need to change this

	WeaponSocket = TEXT("hand_rSocket");

	Health = 150.f;

	SprintSpeed = 700.f;
	JogSpeed = 400.f;
	WalkSpeed = 170.f;

	SprintAcceleration = 3000.f;
	JogAcceleration = GetCharacterMovement()->MaxAcceleration;
	WalkAcceleration = 300.f;

	MaxRifleClipAmmo = 15;
	RifleClipAmmo = MaxRifleClipAmmo;

	ReloadTime = 2.167f;
	FireAnimationLength = 0.233f;

	RotationSpeed = 8.f;

}

// Called when the game starts or when spawned
void AExtreme_Bot::BeginPlay()
{
	Super::BeginPlay();

	WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), WeaponSocket);
	WeaponMesh->SetHiddenInGame(false);
	WeaponMesh->SetVisibility(true);
	WeaponMesh->SetRelativeRotation(FRotator(0.f, 81.f, 0.f));

	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &AExtreme_Bot::OnPlayerCaught);
	}



}

// Called every frame
void AExtreme_Bot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Player)
	{
		Player = Cast<AFPS_ProjectCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}

	if (bIsStrafing && Player)
	{
		if (!BotController)
		{
			BotController = Cast<AExtreme_BotController>(GetController());
		}
		if (BotController)
		{
			BotController->SetFocus(Player);	//Should make pawn face player

		}
	}

}

void AExtreme_Bot::FaceRotation(FRotator NewRotation, float DeltaTime)
{
	if (bHasZeroHealth)
	{
		return;
	}

	FRotator CurrentRotation = FMath::RInterpTo(GetActorRotation(), NewRotation, DeltaTime, RotationSpeed);

	Super::FaceRotation(CurrentRotation, DeltaTime);

}

void AExtreme_Bot::OnPlayerCaught(APawn * Pawn)
{
	AExtreme_BotController* AIController = Cast<AExtreme_BotController>(GetController());

	// Could add a delay here so the player has to be seen for a full second before bot goes agro

	if (AIController)
	{
		AIController->SetPlayerCaught(Pawn);

		bIsAlerted = true;

	}

}

bool AExtreme_Bot::GoAlert()
{
	return false;
}

// Called to bind functionality to input
void AExtreme_Bot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AExtreme_Bot::CheckLoS(const FVector & Source, const FVector & Destination)
{
	
	
	return false;
}

void AExtreme_Bot::BotCrouch()
{
	

}

void AExtreme_Bot::FireWeapon()
{
	
	if (bIsFiring)
	{
		return;
	}
	
	if (Player && BotController)
	{
		BotController->SetFocus(Player);
	}
	

	if (Player)
	{
		if (bIsCrouching)
		{
			
			// Make the bot stand up first to see if it has line of sight
			bIsCrouching = false;

			FTimerHandle StandTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(StandTimerHandle, this, &AExtreme_Bot::CheckLoSWhileStanding, 0.610f, false);

		}
		// Bot is not crouched. Check line of sight and fire
		else if (PawnSensingComp->HasLineOfSightTo(Player))
		{
			// Determine number of shots in salvo
			NumShots = FMath::RandRange(4, 10);

			GetWorld()->GetTimerManager().SetTimer(FiringTimerHandle, this, &AExtreme_Bot::FireShot, 0.1f, false);

		}
	}
		

}

void AExtreme_Bot::FireShot()
{
	if (RifleClipAmmo <= 0)
	{
		Reload();
		return;
	}
	else
	{
		if (NumShots > 0)
		{
			
			RifleClipAmmo--;
			NumShots--;
			bIsFiring = true;
			GetWorld()->GetTimerManager().SetTimer(FiringTimerHandle, this, &AExtreme_Bot::FireShot, 0.233f, false);
		}
		else
		{
			StopFiring();
		}

	}

}

void AExtreme_Bot::CheckLoSWhileStanding()
{
	bIsStandingToCheckForLoS = true;

	if (PawnSensingComp->HasLineOfSightTo(Player))
	{
		// Determine number of shots in salvo
		NumShots = FMath::RandRange(2, 6);

		bWasCrouching = true;

		// Determine delay from standing to firing
		float FireDelay = FMath::RandRange(0.3f, 0.8f);
		
		GetWorld()->GetTimerManager().SetTimer(FiringTimerHandle, this, &AExtreme_Bot::FireShot, FireDelay, false);

	}
	else
	{
		bIsCrouching = true;
		bIsStandingToCheckForLoS = false;
	}

}

void AExtreme_Bot::SetCrouching()
{
	bIsCrouching = true;

}

void AExtreme_Bot::Reload()
{
	if (bIsReloading)
	{
		return;
	}
	
	bIsReloading = true;
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &AExtreme_Bot::ReloadComplete, ReloadTime, false);

}

void AExtreme_Bot::ReloadComplete()
{
	RifleClipAmmo = MaxRifleClipAmmo;
	bIsReloading = false;

	// Continue firing shots after reload
	if (bIsFiring)
	{
		FireShot();
	}

}

void AExtreme_Bot::StopFiring()
{
	
	bIsFiring = false;
	
	if (bWasCrouching)
	{
		bWasCrouching = false;
		
		// Add a delay to make the animation trasition to crouching
		FTimerHandle CrouchTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(CrouchTimerHandle, this, &AExtreme_Bot::SetCrouching, 0.1f, false);

	}

}

bool AExtreme_Bot::ReceiveDamage(float Amount)
{
	Health -= Amount;

	if (Health <= 0.f)
	{
		bHasZeroHealth = true;
		
		// Stop bot from using behavior tree
		AController* Controller = GetController();
		if (Controller)
		{
			AAIController* AI = Cast<AAIController>(Controller);

			AI->BrainComponent->StopLogic("Pawn has been destroyed");

			GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		}
	}
	
	return false;
}

bool AExtreme_Bot::FadeMouseoverDisplay(float DeltaTime)
{
	return false;
}
