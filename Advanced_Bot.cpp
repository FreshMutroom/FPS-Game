// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "Advanced_Bot.h"
#include "Advanced_BotController.h"
#include "FPS_ProjectCharacter.h"
#include "AIPatrolPoint.h"
#include "Runtime/AIModule/Classes/Perception/PawnSensingComponent.h"
#include "Ver2_Projectile.h"
#include "MyStatics.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "Ver2_MouseoverDisplay.h"


// Sets default values
AAdvanced_Bot::AAdvanced_Bot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// A variable not found in Advanced_Bot.h
	AIControllerClass = AAdvanced_BotController::StaticClass();

	// A variable not found in Advanced_Bot.h
	bUseControllerRotationYaw = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;

	BulletTrails = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Trail particle effect"));
	BulletTrails->AttachTo(RootComponent);

	Weapon_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	Weapon_MuzzleLocation->SetupAttachment(WeaponMesh);
	Weapon_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	MuzzleFlash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Muzzle flash particle effect"));
	
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	PawnSensingComp->SetPeripheralVisionAngle(70.f);

	MouseoverInfoBox = CreateDefaultSubobject<UWidgetComponent>(TEXT("Floating info box"));
	MouseoverInfoBox->bAbsoluteRotation = true;
	MouseoverInfoBox->bAbsoluteLocation = true;
	MouseoverInfoBox->SetDrawAtDesiredSize(true);
	MouseoverInfoBox->AttachTo(RootComponent);

	OverheadIconWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Alert icon"));
	OverheadIconWidget->bAbsoluteRotation = true;
	OverheadIconWidget->bAbsoluteLocation = true;
	OverheadIconWidget->SetDrawAtDesiredSize(true);
	OverheadIconWidget->AttachTo(RootComponent);

	MaxHealth = 100.f;
	MaxRange = 3500.f;

	MaxClipAmmo = 20;

	ReloadTime = 1.500f * (1.f / 0.7f);

	OverheadIconText = FString("");

}

// Called when the game starts or when spawned
void AAdvanced_Bot::BeginPlay()
{
	Super::BeginPlay();

	USkeletalMeshComponent* CharacterMesh = GetMesh();
	if (CharacterMesh)
	{
		WeaponMesh->AttachToComponent(CharacterMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("b_RightWeaponSocket"));
		WeaponMesh->SetHiddenInGame(false);
		WeaponMesh->SetVisibility(true);
		WeaponMesh->RelativeRotation = FRotator(0.f, 0.f, -90.f);

	}

	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &AAdvanced_Bot::OnSensePlayer);
		PawnSensingComp->SightRadius = MaxRange;


	}

	SetupMouseoverInfoBoxForBeginPlay();

	SetupOverheadIconForBeginPlay();

	SetupStrafeQueryOnBeginPlay();

	Health = MaxHealth;

	ClipAmmo = MaxClipAmmo;

}

// Called every frame
void AAdvanced_Bot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Player)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(World, 0);
			if (PlayerCharacter)
			{
				Player = Cast<AFPS_ProjectCharacter>(PlayerCharacter);
			}

		}
		
	}

	if (!BotController)
	{
		AController* Controller = GetController();
		if (Controller)
		{
			BotController = Cast<AAdvanced_BotController>(Controller);

		}
	}
	
	UpdateMouseoverDisplayPosition();

	UpdateOverheadIconLocation();

	UpdateOverheadIconRotation();

}

/*// Called to bind functionality to input
void AAdvanced_Bot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}*/

void AAdvanced_Bot::FaceRotation(FRotator NewRotation, float DeltaTime)
{
	if (bHasZeroHealth)
	{
		return;
	}
	
	FRotator CurrentRotation = FMath::RInterpTo(GetActorRotation(), NewRotation, DeltaTime, 8.0f);

	Super::FaceRotation(CurrentRotation, DeltaTime);

}


void AAdvanced_Bot::RemoveFromMouseoverArrays()
{
	// Causing errors with new windows update.
	return;
	
	// Remove from players mouseover arrays
	if (Player)
	{
		int32 Index = Player->MouseoverBots.Find(this);
		Player->MouseoverBots.RemoveAt(Index);
		Player->MouseoverBotsMouseoverTime.RemoveAt(Index);
		Player->MouseoverBotsFadeTime.RemoveAt(Index);
	}

}

void AAdvanced_Bot::Init()
{
	PopulatePatrolPointArray();

	BeginPatrolling();

}

void AAdvanced_Bot::PopulatePatrolPointArray()
{
	UWorld* World = GetWorld();
	if (World)
	{
		// Temp array that holds all patrol points
		TArray<AActor*> AllPatrolPoints;

		/* Populate patrol point array */
		UGameplayStatics::GetAllActorsOfClass(World, AAIPatrolPoint::StaticClass(), AllPatrolPoints);

		// Only assign points that have the correct ID
		for (AActor* Actor : AllPatrolPoints)
		{
			AAIPatrolPoint* Point = Cast<AAIPatrolPoint>(Actor);
			if (Point->ID == PatrolPointID)
			{
				PatrolPoints.Add(Point);
			}
		}
	}

	

}

void AAdvanced_Bot::BeginPatrolling()
{
	if (!bIsIdleGuard)
	{
		MoveToPatrolPoint();
	}
	
}

void AAdvanced_Bot::MoveToPatrolPoint()
{
	if (BotController)
	{
		if (PatrolPoints.Num() > 0)
		{
			NextPatrolPoint = nullptr;

			if (CurrentPatrolPoint != PatrolPoints.Num() - 1)
			{
				NextPatrolPoint = PatrolPoints[++CurrentPatrolPoint];
			}
			else // if there is not any more points to go to
			{
				NextPatrolPoint = PatrolPoints[0];
				CurrentPatrolPoint = 0;
			}

			if (NextPatrolPoint)
			{
				//StopMontages();

				BotController->MoveToLocation(NextPatrolPoint->GetActorLocation(), 10.f, true, true, true, false, 0, true);

				CheckProgressOfMoveTo();

			}
			else
			{
				// No patrol points exist for this bot
			}

		}

	}

}

void AAdvanced_Bot::CheckProgressOfMoveTo()
{
	UWorld* World = GetWorld();

	if (!bIsAlerted)
	{
		// Moving towards a patrol point
		if (BotController && BotController->GetMoveStatus() != EPathFollowingStatus::Moving)
		{
			// At patrol point
			
			if (bWasSpawnedAfterComputerEvent)
			{
				MoveToPatrolPoint();
			}
			else
			{
				float Delay = FMath::RandRange(3.f, 4.f);
				ActionAfterDelay(&AAdvanced_Bot::MoveToPatrolPoint, Delay);
			}

		}
		else
		{
			ActionAfterDelay(&AAdvanced_Bot::CheckProgressOfMoveTo, 0.1f);

		}
	}
	else
	{
		// Is alerted
		if (BotController && BotController->GetMoveStatus() != EPathFollowingStatus::Moving)
		{
			// At move location
			if (NumStrafes == 0)
			{
				PickMovementAction();
			}
			else
			{
				MoveToStrafePoint();
			}

		}
		else
		{
			ActionAfterDelay(&AAdvanced_Bot::CheckProgressOfMoveTo, 0.05f);

		}
	}
}

void AAdvanced_Bot::OnSensePlayer(APawn * Pawn)
{
	
	if (Pawn->IsA(AFPS_ProjectCharacter::StaticClass()))
	{
		if (!bIsAlerted)
		{
			
			bIsAlerted = true;

			if (BotController)
			{
				BotController->StopMovement();
				BotController->SetFocus(Pawn);

			}

			ActionAfterDelay(&AAdvanced_Bot::PickMovementAction, 0.4f);
			
			NumShots = FMath::RandRange(6, 10);
			FireActionAfterDelay(&AAdvanced_Bot::Fire, 0.6f);
				
			// Display exclamation point
			ShowExclamationPointIcon();

		}
	}
	

}

bool AAdvanced_Bot::GoAlert()
{
	if (!bIsAlerted)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0);
			if (Character)
			{
				AFPS_ProjectCharacter* PlayerCharacter = Cast<AFPS_ProjectCharacter>(Character);
				if (PlayerCharacter)
				{
					OnSensePlayer(PlayerCharacter);
				}

			}

		}

	}

	return false;
}

void AAdvanced_Bot::Fire()
{
	UWorld* World = GetWorld();
	if (World)
	{
		
		if (BP_Projectile && Weapon_MuzzleLocation && MuzzleFlash && MuzzleFlash->Template)
		{
			if (Player && Controller && Controller->LineOfSightTo(Player))
			{
				// Has LoS to player
				
				// Check if hitting shot 

				World->SpawnActor<AVer2_Projectile>(BP_Projectile, Weapon_MuzzleLocation->GetSocketLocation(FName("MuzzleFlashSocket")) + FVector(0.f, 28.f, 20.f), Weapon_MuzzleLocation->GetComponentRotation());
				UGameplayStatics::SpawnEmitterAttached(MuzzleFlash->Template, Weapon_MuzzleLocation, FName("MuzzleFlashSocket"), FVector(0.f, 28.f, 20.f), FRotator(0.f, 90.f, 0.f));

				if (RifleFireSound)
				{
					UMyStatics::PlayASound(this, RifleFireSound, Weapon_MuzzleLocation->GetComponentLocation(), 0.45f);
				}

				ClipAmmo--;
				NumShots--;

				if (ClipAmmo <= 0)
				{
					// Reload
					FireActionAfterDelay(&AAdvanced_Bot::Reload, FMath::RandRange(0.2f, 0.5f));

				}
				else
				{
					if (NumShots <= 0)
					{
						// Salvo has finished. Fire next salvo soon
						float DelayBetweenSalvos = FMath::RandRange(SalvoFireRate * 0.5f, SalvoFireRate * 1.5f);
						NumShots = FMath::RandRange(6, 10);
						FireActionAfterDelay(&AAdvanced_Bot::Fire, DelayBetweenSalvos);

					}
					else
					{
						FireActionAfterDelay(&AAdvanced_Bot::Fire, TimeBetweenShots);
					}
				}

			}
			else
			{
				// Does not have LoS to player

				FireActionAfterDelay(&AAdvanced_Bot::Fire, TimeBetweenShots * 2.f);

			}
		
		}

		
	}
	

}

void AAdvanced_Bot::Reload()
{
	// Play animation montage
	PlayMontage(ReloadMontage);

	FireActionAfterDelay(&AAdvanced_Bot::OnReloadFinish, ReloadTime);

}

void AAdvanced_Bot::OnReloadFinish()
{
	ClipAmmo = MaxClipAmmo;

	NumShots = FMath::RandRange(6, 10);
	FireActionAfterDelay(&AAdvanced_Bot::Fire, FMath::RandRange(0.3f, 0.5f));


}

void AAdvanced_Bot::PickMovementAction()
{
	int32 Action = FMath::RandRange(1, 100);
	if (Action <= 20)
	{
		// Run at player
		RunAtPlayer();
	}
	else if (Action <= 50)
	{
		MoveNearPlayer();
	}
	else
	{
		Strafe();
	}

}

void AAdvanced_Bot::Strafe()
{
	NumStrafes = FMath::RandRange(2, 4);

	MoveToStrafePoint();

}

void AAdvanced_Bot::MoveToStrafePoint()
{
	
	if (BotController)
	{
		// run EQS query
		RunStrafeQuery();
	}

}

void AAdvanced_Bot::RunAtPlayer()
{
	if (BotController && Player)
	{
		BotController->MoveToActor(Player, FMath::RandRange(MinRunAtPlayerDistance, MaxRunAtPlayerDistance));

		CheckProgressOfMoveTo();

	}

}

void AAdvanced_Bot::MoveNearPlayer()
{
	if (BotController && Player)
	{
		const float SearchRadius = 1000.f;
		const FVector SearchOrigin = Player->GetActorLocation() + 600.0f * (GetActorLocation() - Player->GetActorLocation()).GetSafeNormal();

		const FVector Location = UNavigationSystem::GetRandomReachablePointInRadius(BotController, SearchOrigin, SearchRadius);
		
		if (Location != FVector::ZeroVector)
		{
			BotController->MoveToLocation(Location, 5.f);

			CheckProgressOfMoveTo();

		}

	}
	
}

void AAdvanced_Bot::ActionAfterDelay(void(AAdvanced_Bot:: * Function)(), float Delay, bool OverridePendingAction)
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (!OverridePendingAction && World->GetTimerManager().IsTimerActive(NextActionTimerHandle))
		{
			return;
		}

		World->GetTimerManager().SetTimer(NextActionTimerHandle, this, Function, Delay, false);

	}
}

void AAdvanced_Bot::FireActionAfterDelay(void(AAdvanced_Bot::* Function)(), float Delay, bool OverridePendingAction)
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (!OverridePendingAction && World->GetTimerManager().IsTimerActive(FireTimerHandle))
		{
			return;
		}

		World->GetTimerManager().SetTimer(FireTimerHandle, this, Function, Delay, false);

	}

}

void AAdvanced_Bot::RunStrafeQuery()
{
	EQSStrafeQuery.Execute(EEnvQueryRunMode::SingleResult, this, &AAdvanced_Bot::OnStrafeQueryFinished);

}

void AAdvanced_Bot::OnStrafeQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	FVector Location = FVector::ZeroVector;
	Location = Result->GetItemAsLocation(0);

	if (Location != FVector::ZeroVector)
	{
		NumStrafes--;
		
		BotController->MoveToLocation(Location, 5.f);

		CheckProgressOfMoveTo();

	}
	else
	{
		// No location found
		PickMovementAction();
	}
}

void AAdvanced_Bot::SetupStrafeQueryOnBeginPlay()
{
	if (StrafeQuery)
	{
		EQSStrafeQuery = FEnvQueryRequest(StrafeQuery, this);
	}

}

void AAdvanced_Bot::PlayMontage(UAnimMontage * Montage)
{
	if (Montage)
	{
		USkeletalMeshComponent* BotMesh = GetMesh();
		if (BotMesh)
		{
			UAnimInstance* AnimInst = BotMesh->GetAnimInstance();
			if (AnimInst)
			{
				UAnimMontage* CurrentMontage = AnimInst->GetCurrentActiveMontage();
				if (CurrentMontage)
				{
					AnimInst->Montage_Stop(0.2f/*CurrentMontage->BlendOutTriggerTime*/);
				}

				AnimInst->Montage_Play(Montage);
			}
		}

	}
}

void AAdvanced_Bot::EnableRagdoll()
{
	// Enable ragdoll
	USkeletalMeshComponent* CharacterMesh = GetMesh();
	if (CharacterMesh)
	{
		CharacterMesh->SetAllBodiesSimulatePhysics(true);
		CharacterMesh->SetSimulatePhysics(true);
		CharacterMesh->WakeAllRigidBodies();
	}

}

void AAdvanced_Bot::DisableRagdoll()
{
	// Since I can't get ragdoll to disable properly I'm just going to destroy
	Destroy();
	return;
	
	
	USkeletalMeshComponent* CharacterMesh = GetMesh();
	if (CharacterMesh)
	{
		// These 4 lines will basically freeze the bot. May have to leave them out on packaging.
		// Means the mesh still simulates physics while lying there
		//CharacterMesh->bPauseAnims = true;
		//CharacterMesh->SetAllBodiesSimulatePhysics(false);
		//CharacterMesh->SetSimulatePhysics(false);
		//CharacterMesh->PutAllRigidBodiesToSleep();
		
		CharacterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Disable collision on capsule too
		UCapsuleComponent* Capsule = GetCapsuleComponent();
		if (Capsule)
		{
			Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		SetActorTickEnabled(false);

	}

}

void AAdvanced_Bot::SetupMouseoverInfoBoxForBeginPlay()
{
	if (MouseoverInfoBox)
	{
		UUserWidget* Widget = MouseoverInfoBox->GetUserWidgetObject();

		if (Widget)
		{
			// Set value of the OwningBot in the widget so it can get a reference to this bot
			// because from blueprints I cannot get a reference to this pawn
			UVer2_MouseoverDisplay* MouseoverDisplay = Cast<UVer2_MouseoverDisplay>(Widget);

			if (MouseoverDisplay)
			{
				MouseoverDisplay->OwningAdvancedBot = this;
			}


			// Change widget opacity
			FLinearColor Color = Widget->ColorAndOpacity;

			// This line may not be needed
			Widget->SetVisibility(ESlateVisibility::Visible);

			Widget->SetColorAndOpacity(FLinearColor(Color.R, Color.G, Color.B, 0.f));

		}

	}

}

void AAdvanced_Bot::UpdateMouseoverDisplayPosition()
{
	if (MouseoverInfoBox)
	{
		MouseoverInfoBox->SetWorldLocation(GetActorLocation() + MouseoverBoxOffset);
	}

}

void AAdvanced_Bot::MakeMouseoverDisplayInvisible()
{
}

void AAdvanced_Bot::ShowExclamationPointIcon()
{
	if (OverheadIconWidget)
	{

		OverheadIconWidget->SetWorldLocation(GetActorLocation() + OverheadIconOffset);

		OverheadIconTextColor = FSlateColor(FLinearColor(1.f, 0.f, 0.f, 1.f));

		OverheadIconText = FString("!");

		// Clear icon after a a little
		UWorld* World = GetWorld();
		if (World)
		{
			FTimerHandle DummyTimerHandle;
			World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AAdvanced_Bot::FadeOutOverheadExclamationPoint, 1.0f, false);

		}

	}

}

void AAdvanced_Bot::ShowQuestionMarkIcon()
{
	if (OverheadIconWidget)
	{
		OverheadIconWidget->SetWorldLocation(GetActorLocation() + OverheadIconOffset);

		OverheadIconTextColor = FSlateColor(FLinearColor(0.1f, 0.f, 1.0f, 1.f));

		OverheadIconText = FString("?");

		// Clear icon after a a little
		UWorld* World = GetWorld();
		if (World)
		{
			FTimerHandle DummyTimerHandle;
			World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AAdvanced_Bot::FadeOutOverheadQuestionMark, 3.f, false);

		}

	}

}

void AAdvanced_Bot::FadeOutOverheadExclamationPoint()
{
	// Create a animation in editor, make sure widgets parent class is the widget class I created
	// and play that animation. 

	if (OverheadIconUserWidget && OverheadIconUserWidget->Animation)
	{
		OverheadIconUserWidget->PlayAnimation(OverheadIconUserWidget->Animation, 0.f, 1);
	}

}

void AAdvanced_Bot::FadeOutOverheadQuestionMark()
{
	// Create a animation in editor, make sure widgets parent class is the widget class I created
	// and play that animation.

	if (OverheadIconUserWidget && OverheadIconUserWidget->Animation2)
	{
		OverheadIconUserWidget->PlayAnimation(OverheadIconUserWidget->Animation2, 0.f, 1);
	}

}

void AAdvanced_Bot::UpdateOverheadIconLocation()
{
	if (OverheadIconWidget)
	{
		OverheadIconWidget->SetWorldLocation(GetActorLocation() + OverheadIconOffset);
	}

}

void AAdvanced_Bot::SetupOverheadIconForBeginPlay()
{
	if (OverheadIconWidget)
	{
		UUserWidget* Widget = OverheadIconWidget->GetUserWidgetObject();

		if (Widget)
		{

			OverheadIconUserWidget = Cast<UVer2_MouseoverDisplay>(Widget);

			if (OverheadIconUserWidget)
			{
				OverheadIconUserWidget->OwningAdvancedBot = this;

			}

		}

	}

}

void AAdvanced_Bot::UpdateOverheadIconRotation()
{
	if (Player)
	{
		FVector BotToPlayer = Player->GetActorLocation() - GetActorLocation();

		FRotator NewRotation = FRotationMatrix::MakeFromX(BotToPlayer).Rotator();

		if (OverheadIconWidget)
		{
			OverheadIconWidget->SetWorldRotation(NewRotation);
		}

	}

}


bool AAdvanced_Bot::ReceiveDamage(float Amount)
{
	Health -= Amount;

	if (Health <= 0.f && !bHasZeroHealth)
	{
		bHasZeroHealth = true;

		UWorld* World = GetWorld();
		if (World)
		{
			// Stop all timer handles running
			World->GetTimerManager().ClearAllTimersForObject(this);
		}
		
		// Could go ragdoll here instead
		PlayMontage(ZeroHealthMontage);

		UCapsuleComponent* Capsule = GetCapsuleComponent();
		if (Capsule)
		{
			Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		EnableRagdoll();

		FTimerHandle DummyTimerHandle;
		World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AAdvanced_Bot::DisableRagdoll, 60.f, false);

		// For bunker event
		if (Player)
		{
			Player->IncrementNumBotsDefeated();

			UWorld * World = GetWorld();
			if (World)
			{

				FTimerHandle DummyTimerHandle;
				World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AAdvanced_Bot::RemoveFromMouseoverArrays, 3.f, false);

			}
		}


	}
	else
	{
		OnHit();

		return false;

	}

	return false;

}

void AAdvanced_Bot::OnHit()
{
	UWorld * World = GetWorld();
	if (World)
	{
		float Delay = FMath::RandRange(0.2f, 0.4f);
		
		FTimerHandle DummyTimerHandle;
		World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AAdvanced_Bot::BeforeGoAlert, Delay, false);

	}

}

void AAdvanced_Bot::BeforeGoAlert()
{
	GoAlert();

}

bool AAdvanced_Bot::FadeMouseoverDisplay(float DeltaTime)
{
	if (MouseoverInfoBox)
	{

		// Change alpha value
		UUserWidget* Widget = MouseoverInfoBox->GetUserWidgetObject();

		if (Widget)
		{
			if (bHasZeroHealth)
			{
				Widget->SetVisibility(ESlateVisibility::Hidden);
				return false;
			}


			if (DeltaTime > 0.f && Widget->ColorAndOpacity.A < 1.f)
			{

				FLinearColor Color = Widget->ColorAndOpacity;

				Widget->SetColorAndOpacity(FLinearColor(Color.R, Color.G, Color.B, Widget->ColorAndOpacity.A + DeltaTime * MouseoverDisplayFadeInSpeed));

				if (Widget->ColorAndOpacity.A > 1.f)
				{
					Widget->ColorAndOpacity.A = 1.f;

				}

			}
			else if (DeltaTime < 0.f && Widget->ColorAndOpacity.A > 0.f)
			{
				FLinearColor Color = Widget->ColorAndOpacity;

				Widget->SetColorAndOpacity(FLinearColor(Color.R, Color.G, Color.B, Widget->ColorAndOpacity.A + DeltaTime * MouseoverDisplayFadeOutSpeed));

				if (Widget->ColorAndOpacity.A < 0.f)
				{
					Widget->ColorAndOpacity.A = 0.f;

				}
			}
		}

	}

	return false;
}
