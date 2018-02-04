// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "Ver2_Bot.h"
#include "Ver2_Controller.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "FPS_ProjectCharacter.h"
#include "InvisibleMesh.h"
#include "MyStatics.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "Ver2_MouseoverDisplay.h"
#include "DamagableBot.h"
#include "Ver2_MouseoverDisplay.h"


// Sets default values
AVer2_Bot::AVer2_Bot()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AVer2_Controller::StaticClass();

	bUseControllerRotationYaw = true;

	// Added by me
	//bUseControllerRotationPitch = true;

	// Initialize senses
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	PawnSensingComp->SetPeripheralVisionAngle(70.f);

	RifleWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));

	LauncherMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Launcher Mesh"));

	RifleMuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Rifle Muzzle Location"));

	LauncherMuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Launcher Muzzle Location"));

	RifleMuzzleflash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Rifle muzzleflash particles"));
	RifleMuzzleflash->AttachTo(RootComponent);	// This is only because mouseovers are picking up spawn location

	LauncherMuzzleFlash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Launcher muzzle flash"));
	LauncherMuzzleFlash->AttachTo(RootComponent);	// This is only because mouseovers are picking up spawn location

	LauncherMuzzleSmoke = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Launcher muzzle smoke"));
	LauncherMuzzleSmoke->AttachTo(RootComponent);	// This is only because mouseovers are picking up spawn location

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

	GetCharacterMovement()->UseAccelerationForPathFollowing();
	GetCharacterMovement()->MaxAcceleration = 800.f;
	GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
	
	WeaponSocket = TEXT("hand_rSocket");

	RotationSpeed = 8.f;

	MaxHealth = 1500.f;
	Health = MaxHealth;

	MaxClipAmmo = 15;	// for testing purposes
	ClipAmmo = MaxClipAmmo;

	MaxLauncherAmmo = 2;
	CurrentLauncherAmmo = MaxLauncherAmmo;

	ReloadTime = 2.167f + 0.25f;

	EquippedWeapon = EWeaponEnum::Rifle;

	LookAroundAmount = 25.f;

	bIsLookingStraightAhead = true;

	GrenadeLauncherEquipTime = 1.733f;

	LauncherReloadTime = 4.3f;

	RecentDamageTimeInterval = 4.f;

	StrafeAccuracyPenalty = 0.75f;

	OverheadIconText = FString("");

	RifleMuzzleOffset = FVector(76.f, 0.f, 10.f);
	LauncherMuzzleOffset = FVector(80.f, 0.f, 0.f);

	AlertOthersDistance = 2000.f;

}

// Called when the game starts or when spawned
void AVer2_Bot::BeginPlay()
{
	Super::BeginPlay();
	
	if (RifleWeaponMesh && RifleMuzzleLocation && RifleMuzzleflash)
	{
		// Attach rifle
		RifleWeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), WeaponSocket);
		RifleWeaponMesh->SetHiddenInGame(false);
		RifleWeaponMesh->SetVisibility(true);
		RifleWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		RifleWeaponMesh->SetRelativeRotation(FRotator(0.f, 81.f, 0.f));

		// Changed from setupattachment to try and fix bug of ammo not spawning in correct location,
		// and this fixed it
		RifleMuzzleLocation->AttachToComponent(RifleWeaponMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
		RifleMuzzleLocation->SetRelativeLocation(RifleMuzzleOffset);

		RifleMuzzleflash->SetupAttachment(RifleMuzzleLocation);

	}
	
	if (LauncherMesh && LauncherMuzzleLocation && LauncherMuzzleFlash && LauncherMuzzleSmoke)
	{
		LauncherMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), WeaponSocket);
		LauncherMesh->SetHiddenInGame(true);
		LauncherMesh->SetVisibility(false);
		LauncherMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		LauncherMesh->SetRelativeRotation(FRotator(0.f, 81.f, 0.f));

		LauncherMuzzleLocation->AttachToComponent(LauncherMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
		LauncherMuzzleLocation->SetRelativeLocation(LauncherMuzzleOffset);

		LauncherMuzzleFlash->AttachToComponent(LauncherMuzzleLocation, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
		LauncherMuzzleFlash->ActivateSystem(false);
		LauncherMuzzleFlash->SetVisibility(false);

		LauncherMuzzleSmoke->AttachToComponent(LauncherMuzzleLocation, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
		LauncherMuzzleSmoke->ActivateSystem(false);
		LauncherMuzzleSmoke->SetVisibility(false);

	}

	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &AVer2_Bot::OnSensePawn);
		PawnSensingComp->SightRadius = MaxRange;
	}

	if (StrafeQuery)
	{
		EQSStrafeQuery = FEnvQueryRequest(StrafeQuery, this);
	}
	
	if (StrafeQueryLauncher)
	{
		EQSStrafeQueryLauncher = FEnvQueryRequest(StrafeQueryLauncher, this);
	}

	if (FindCoverQuery)
	{
		EQSFindCoverQuery = FEnvQueryRequest(FindCoverQuery, this);
	}

	if (HighCoverStrafeQuery)
	{
		EQSHighCoverStrafeQuery = FEnvQueryRequest(HighCoverStrafeQuery, this);
	}

	// For gradual acceleration on move commands. Need to change MaxAcceleration too
	UMovementComponent* MovementComp = GetMovementComponent();
	
	if (MovementComp)
	{
		UCharacterMovementComponent* CharMoveComp = Cast<UCharacterMovementComponent>(MovementComp);
	
		if (CharMoveComp)
		{
			CharMoveComp->bRequestedMoveUseAcceleration = true;
		}
	
	}

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
				MouseoverDisplay->OwningPawn = this;
			}


			// Change widget opacity
			FLinearColor Color = Widget->ColorAndOpacity;
			
			// This line may not be needed
			Widget->SetVisibility(ESlateVisibility::Visible);

			Widget->SetColorAndOpacity(FLinearColor(Color.R, Color.G, Color.B, 0.f));
		
		}

	}

	if (OverheadIconWidget)
	{
		UUserWidget* Widget = OverheadIconWidget->GetUserWidgetObject();

		if (Widget)
		{
			
			OverheadIconUserWidget = Cast<UVer2_MouseoverDisplay>(Widget);

			if (OverheadIconUserWidget)
			{
				OverheadIconUserWidget->OwningPawn = this;

			}

		}

	}

}

// Called every frame
void AVer2_Bot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Player)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			Player = Cast<AFPS_ProjectCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));

		}
		
	}

	if (!BotController)
	{
		BotController = Cast<AVer2_Controller>(GetController());

	}

	
	// Rotate widget to face player
	if (Player)
	{
		FVector BotToPlayer = Player->GetActorLocation() - GetActorLocation();
	
		FRotator NewRotation = FRotationMatrix::MakeFromX(BotToPlayer).Rotator();
	
		if (MouseoverInfoBox)
		{
			MouseoverInfoBox->SetWorldRotation(NewRotation);
	
		}
	
		if (OverheadIconWidget)
		{
			OverheadIconWidget->SetWorldRotation(NewRotation);
		}
	
	}
	
	UpdateMouseoverDisplayPosition();
	
	UpdateOverheadIconLocation();

}

// Called to bind functionality to input
void AVer2_Bot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AVer2_Bot::DebugMsg(const FString Message, FColor Color)
{
	if (bIsDebugging)
	{
		UMyStatics::DebugMsg(Message, Color);
	}
}

void AVer2_Bot::OnSensePawn(APawn * Pawn)
{
	
	if (Pawn->IsA(AFPS_ProjectCharacter::StaticClass()))
	{
		UWorld* World = GetWorld();

		if (!bIsAlerted)
		{
			bIsAlerted = true;
			
			AVer2_Bot::DebugMsg(FString("Function change: OnSensePawn()"), FColor::Red);

			
			if (World)
			{
				if (!World->GetTimerManager().IsTimerActive(EnterAlertTimerHandle))
				{
					World->GetTimerManager().SetTimer(EnterAlertTimerHandle, this, &AVer2_Bot::EnterAlertPhase, 0.5f, false);

				}
				
				if (!World->GetTimerManager().IsTimerActive(AlertNearbyBotsAfterDamageTimerHandle))
				{
					World->GetTimerManager().SetTimer(AlertNearbyBotsAfterDamageTimerHandle, this, &AVer2_Bot::AlertNearbyBots, 0.75f, false);

				}

			}

		}
		else if (bIsCautious)
		{
			bIsCautious = false;
			
			bIsAlerted = true;

			if (World && Player && BotController)
			{
				BotController->SetFocus(Player);
				
				// If here bot may have been either shooting from cover, strafing or using launcher
				FTimerHandle DummyTimerHandle;

				if (EquippedWeapon == EWeaponEnum::Rifle)
				{
					World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::FireSalvo, 0.3f, false);
				}
				else
				{
					World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::FireLauncher, 0.3f, false);
				}

			}
			
		}

	}

}

void AVer2_Bot::StartFiringRangeRoutine()
{
	if (BotController)
	{
		
		// Assign value to FiringRangeTarget
		TArray<AActor*> Targets;

		UGameplayStatics::GetAllActorsOfClass(this, AFiringRangeTarget::StaticClass(), Targets);

		/* Sort array from least closest target to furtherest target */
		Targets.Sort([&](const AActor & A, const AActor & B)
		{
			return (A.GetActorLocation() - GetActorLocation()).SizeSquared() < (B.GetActorLocation() - GetActorLocation()).SizeSquared();
		});

		if (Targets.IsValidIndex(FiringRangeTargetDistance))
		{
			FiringRangeTarget = Targets[FiringRangeTargetDistance];
		}
		 
		if (FiringRangeTarget)
		{
			BotController->SetFocus(FiringRangeTarget);

			PrepareFireAtFiringRangeTarget();
		}

	}

}

void AVer2_Bot::PrepareFireAtFiringRangeTarget()
{
	UWorld* World = GetWorld();
	if (World)
	{
		NumShots = FMath::RandRange(3, 8);
		
		float Delay = FMath::RandRange(1.f, 2.5f);
		FTimerHandle DummyTimerHandle;
		World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::FireAtFiringRangeTarget, Delay, false);

	}

}

void AVer2_Bot::FireAtFiringRangeTarget()
{
	if (bIsAlerted)
	{
		return;
	}
	
	UWorld* World = GetWorld();

	if (World && BP_FiringRangeProjectile && RifleMuzzleLocation && BotController)
	{
		
		if (NumShots > 0)
		{
			
			if (ClipAmmo > 0)
			{
				NumShots--;
				ClipAmmo--;
				
				World->SpawnActor<AVer2_FiringRangeProjectile>(BP_FiringRangeProjectile, RifleMuzzleLocation->GetComponentLocation(), GetController()->GetControlRotation());
				PlayMontage(FireRifleIronsightsMontage);

				FTimerHandle DummyTimerHandle;
				World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::FireAtFiringRangeTarget, FireDelay, false);

				if (RifleWeaponMesh && RifleMuzzleflash)
				{
					// Create muzzle flash
					UGameplayStatics::SpawnEmitterAttached(RifleMuzzleflash->Template, RifleWeaponMesh, FName("MuzzleFlashSocket"), FVector(20.f, 0.f, 0.f), RifleMuzzleFlashRotationOffset);

					// Play sound
					UMyStatics::PlaySound(this, RifleFireSounds, RifleMuzzleLocation->GetComponentLocation(), 0.45f);

				}


			}
			else
			{
				// reload
				float ReloadNoticeDelay = FMath::RandRange(1.f, 2.f);
				FTimerHandle ReloadStartTimerHandle;
				World->GetTimerManager().SetTimer(ReloadStartTimerHandle, this, &AVer2_Bot::ReloadAtFiringRange, ReloadNoticeDelay, false);

				FTimerHandle DummyTimerHandle;
				World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::OnReloadAtFiringRangeFinish, ReloadTime + ReloadNoticeDelay, false);


			}
		}
		else
		{
			// Finished salvo. Start another salvo soon
			float Delay = FMath::RandRange(1.f, 2.5f);
			
			NumShots = FMath::RandRange(3, 8);

			FTimerHandle DummyTimerHandle;
			World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::FireAtFiringRangeTarget, Delay, false);

		}
	}

}

void AVer2_Bot::ReloadAtFiringRange()
{
	if (bIsAlerted)
	{
		return;
	}
	
	PlayMontage(ReloadRifleIronsightsMontage);

}

void AVer2_Bot::OnReloadAtFiringRangeFinish()
{
	if (bIsAlerted)
	{
		return;
	}
	
	ClipAmmo = MaxClipAmmo;

	UWorld* World = GetWorld();
	if (World)
	{
		float Delay = FMath::RandRange(0.5f, 1.f);

		NumShots = FMath::RandRange(3, 8);

		FTimerHandle DummyTimerHandle;
		World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::FireAtFiringRangeTarget, Delay, false);

	}

}

void AVer2_Bot::LowerWeapon()
{
	PlayMontage(IdleHipMontage);

}

void AVer2_Bot::StartPatrol()
{
	AVer2_Bot::DebugMsg(FString("Function change: StartPatrol()"), FColor::Red);
	
	if (!BotController)
	{
		BotController = Cast<AVer2_Controller>(GetController());
	}

	MoveToNextPatrolPoint();

}

void AVer2_Bot::MoveToNextPatrolPoint()
{

	if (bIsAlerted)
	{
		return;
	}
	
	AVer2_Bot::DebugMsg(FString("Function change: MoveToNextPatrolPoint()"), FColor::Red);

	bIsIdle = false;

	if (BotController)
	{
		if (BotController->PatrolPoints.Num() > 0)
		{
			NextPatrolPoint = nullptr;

			if (BotController->CurrentPatrolPoint != BotController->PatrolPoints.Num() - 1)
			{
				NextPatrolPoint = BotController->PatrolPoints[++BotController->CurrentPatrolPoint];
			}
			else // if there is not any more points to go to
			{
				NextPatrolPoint = BotController->PatrolPoints[0];
				BotController->CurrentPatrolPoint = 0;
			}

			if (NextPatrolPoint)
			{
				// Uncommented to get animation working pretty good
				StopMontages();

				EPathFollowingRequestResult::Type MoveResult = BotController->MoveToLocation(NextPatrolPoint->GetActorLocation(), 10.f, true, true, true, false, 0, true);

				if (MoveResult == EPathFollowingRequestResult::Failed)
				{
					// No path to cover location exists
					AVer2_Bot::DebugMsg(FString("In MoveToNextPatrolPoint(): MoveTo command did not find a path"), FColor::Blue);
				}

				CheckProgressOfPatrolPointMoveTo();

			}
			else
			{
				// No patrol points exist for this bot
			}

		}

	}

}

void AVer2_Bot::CheckProgressOfPatrolPointMoveTo()
{
	//AVer2_Bot::DebugMsg(FString("Function change: CheckProgressOfPatrolPointMoveTo()"), FColor::Red);

	if (NextPatrolPoint != nullptr)
	{

		UWorld* World = GetWorld();

		// Moving towards a patrol point
		if (BotController && BotController->GetMoveStatus() != EPathFollowingStatus::Moving)
		{
			WaitAtPatrolPoint();

		}
		else
		{
			if (World)
			{
				World->GetTimerManager().SetTimer(PatrolPointMoveToTimerHandle, this, &AVer2_Bot::CheckProgressOfPatrolPointMoveTo, 0.25f, false);

			}

		}

	}

}

void AVer2_Bot::IsIdle()
{
	AVer2_Bot::DebugMsg(FString("Function change: IsIdle()"), FColor::Red);
	
	bIsIdle = true;

	// Play idle montage
	float MontageSelector = FMath::RandRange(0.f, 1.f);
	if (MontageSelector < 0.5f)
	{
		PlayMontage(IdleMontage1);
	}
	else
	{
		PlayMontage(IdleMontage2);
	}

}

void AVer2_Bot::WaitAtPatrolPoint()
{
	if (bIsAlerted)
	{
		return;
	}
	
	AVer2_Bot::DebugMsg(FString("Function change: WaitAtPatrolPoint()"), FColor::Red);
	
	UWorld* World = GetWorld();
	if (World)
	{
		if (bIsAlwaysMoving)
		{
			// If here then bot was spawned at spawn point after computer event
			MoveToNextPatrolPoint();
		}
		else
		{
			float WaitTime = FMath::RandRange(4.0f, 9.0f);

			if (WaitTime > 7.0f)
			{
				// Enough time to play a idle animation
				FTimerHandle StartIdleAnimTimerHandle;
				World->GetTimerManager().SetTimer(StartIdleAnimTimerHandle, this, &AVer2_Bot::IsIdle, FMath::RandRange(1.0f, 1.5f), false);

			}

			FTimerHandle WaitTimerHandle;
			World->GetTimerManager().SetTimer(PatrolPointMoveToTimerHandle, this, &AVer2_Bot::MoveToNextPatrolPoint, WaitTime, false);

		}
		
	}

}

void AVer2_Bot::EnterAlertPhase()
{
	AVer2_Bot::DebugMsg(FString("Function change: EnterAlertPhase()"), FColor::Red);
	
	bIsIdle = false;
	
	if (BotController)
	{
		// Stop any movement. Patrol point movement is what I have in mind
		BotController->StopMovement();

		BotController->ClearFocus(EAIFocusPriority::Gameplay);

		// This may not be the right action to take here
		RunFindCoverQuery();

	}

	// For cosmetics only
	ShowExclamationPointIcon();

}

bool AVer2_Bot::GoAlert()
{
	if (!bIsAlerted)
	{
		EnterAlertPhase();
	}
	
	return false;
}

void AVer2_Bot::AlertNearbyBots()
{
	UWorld* World = GetWorld();
	if (World)
	{
		TArray<AActor*> AllBots;
		UGameplayStatics::GetAllActorsWithInterface(World, UDamagableBot::StaticClass(), AllBots);	// Note UDamagableBot, not IDamagableBot

		for (AActor* Bot : AllBots)
		{
			IDamagableBot* AsDamagable = Cast<IDamagableBot>(Bot);
			if (AsDamagable)
			{
				float Distance = (GetActorLocation() - Bot->GetActorLocation()).Size();
				if (Distance < AlertOthersDistance)
				{
					AsDamagable->GoAlert();
				}
				
			}

		}

	}

}

void AVer2_Bot::RunFindCoverQuery()
{
	AVer2_Bot::DebugMsg(FString("Function change: RunFindCoverQuery()"), FColor::Red);

	EQSFindCoverQuery.Execute(EEnvQueryRunMode::SingleResult, this, &AVer2_Bot::OnFindCoverQueryFinished);

}

void AVer2_Bot::OnFindCoverQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	AVer2_Bot::DebugMsg(FString("Function change: OnFindCoverQueryFinished()"), FColor::Red);

	CoverLocation = FVector(0.f);
	CoverLocation = Result->GetItemAsLocation(0);

	if (CoverLocation != FVector(0.f))
	{

		if (BotController)
		{
			
			ChangeMoveSpeed(RunSpeed);

			// Uncommented to get animation working pretty good
			StopMontages();

			// last param: AllowPartialPath
			EPathFollowingRequestResult::Type MoveResult = BotController->MoveToLocation(CoverLocation, 5.1f, true, true, false, false, 0, false);

			if (MoveResult == EPathFollowingRequestResult::Failed)
			{
				// MoveTo did consider the path moveable to
				AVer2_Bot::DebugMsg(FString("In OnFindCoverQueryFinished(): MoveToLocation did not consider path moveable to"), FColor::Blue);

			}
		}
		
		CheckProgressOfMoveToCover();

	}
	else
	{
		// I assume if here then no suitable cover location could be found. 
		// Do something else like run at player

		AVer2_Bot::DebugMsg(FString("Cover query did not find location"), FColor::Blue);

	}

}

void AVer2_Bot::CheckProgressOfMoveToCover()
{
	//AVer2_Bot::DebugMsg(FString("Function change: CheckProgressOfMoveToCover()"), FColor::Red);

	UWorld* World = GetWorld();

	if (CoverLocation != FVector(0.f))
	{
		// Moving towards patrol point
		
		if (BotController && BotController->GetMoveStatus() != EPathFollowingStatus::Moving)
		{
			// Bot has reached MoveTo location

			if (World)
			{
				
				if (bIsCrouching)
				{
					FTimerHandle StartCoverRoutineTimerHandle;
					World->GetTimerManager().SetTimer(StartCoverRoutineTimerHandle, this, &AVer2_Bot::BeginCoverRoutine, 0.5f, false);
				}
				else
				{
					BeginCoverRoutine();
				}
			}

		}
		else
		{
			if (World)
			{
				World->GetTimerManager().SetTimer(CheckIfAtCoverLocationTimerHandle, this, &AVer2_Bot::CheckProgressOfMoveToCover, 0.10f, false);

			}

		}

	}
	else
	{
		AVer2_Bot::DebugMsg(FString("In CheckProgressOfMoveToCover(), CoverLocation == FVector(0.f)"), FColor::Blue);
	}

}

void AVer2_Bot::DecideNextAction()
{
	// This function is called after finishing a salvo.
	// Now randomly decide what to do next:
	// - Continue firing from cover
	// - Move to new cover nearby
	// - Stand up to strafe and fire
	// - Equip grenade launcher

	AVer2_Bot::DebugMsg(FString("Function change: DecideNextAction()"), FColor::Red);

	int32 Action = FMath::RandRange(1, 100);

	float TimeTillNextAction = FMath::RandRange(1.f, 2.5f);

	UWorld* World = GetWorld();
	if (World)
	{
		FTimerHandle DecideWhatToDoTimerHandle;
		
		// If low on health chance to strafe and equip grenade launcher increases
		if (Health / MaxHealth < 0.4f)
		{
			if (Action <= 15 && !CheckCoverHeight())
			{
				// Continue firing from cover (StandAndFire)
				World->GetTimerManager().SetTimer(DecideWhatToDoTimerHandle, this, &AVer2_Bot::StandAndFire, TimeTillNextAction, false);

			}
			else if (Action <= 50)
			{
				// Strafe and fire
				World->GetTimerManager().SetTimer(DecideWhatToDoTimerHandle, this, &AVer2_Bot::StartStandAndStrafeRoutine, TimeTillNextAction, false);

			}
			else if (Action <= 60)
			{
				// Move to new Cover nearby
				World->GetTimerManager().SetTimer(DecideWhatToDoTimerHandle, this, &AVer2_Bot::BeginMoveToNewCoverRoutine, TimeTillNextAction, false);

			}
			else
			{
				// Grenade launcher routine
				World->GetTimerManager().SetTimer(DecideWhatToDoTimerHandle, this, &AVer2_Bot::BeginGrenadeLauncherRoutine, TimeTillNextAction, false);

			}

		}
		else
		{
			int NewCoverThreshold = 12;

			if (BotController && Player && BotController->LineOfSightTo(Player))
			{
				float Distance = (Player->GetActorLocation() - GetActorLocation()).Size();

				if (Distance < MaxRange)
				{
					// if crouched and has line of sight to player then cover is no longer relevant - increase
					// chance of moving to new cover. The first Action if below needs to do BeginMoveToNewCoverRoutine
					// instead of stand and fire and the second needs to be StandAndFire.
					NewCoverThreshold += 58;

				}
				
			}

			if (Action <= NewCoverThreshold)	
			{
				// Move to new Cover nearby
				World->GetTimerManager().SetTimer(DecideWhatToDoTimerHandle, this, &AVer2_Bot::BeginMoveToNewCoverRoutine, TimeTillNextAction, false);

			}
			else if (Action <= 70)
			{
				if (CheckCoverHeight())
				{
					// Behind high cover
					World->GetTimerManager().SetTimer(DecideWhatToDoTimerHandle, this, &AVer2_Bot::BeginHighCoverRoutine, TimeTillNextAction, false);

				}
				else
				{
					// Continue firing from low cover
					World->GetTimerManager().SetTimer(DecideWhatToDoTimerHandle, this, &AVer2_Bot::StandAndFire, TimeTillNextAction, false);

				}
			
			}
			else if (Action <= 85)
			{
				// Strafe and fire
				World->GetTimerManager().SetTimer(DecideWhatToDoTimerHandle, this, &AVer2_Bot::StartStandAndStrafeRoutine, TimeTillNextAction, false);

			}
			else
			{
				// Grenade launcher routine
				World->GetTimerManager().SetTimer(DecideWhatToDoTimerHandle, this, &AVer2_Bot::BeginGrenadeLauncherRoutine, TimeTillNextAction, false);

			}

		}
	}

}

void AVer2_Bot::DecideNextActionLauncher()
{
	AVer2_Bot::DebugMsg(FString("Function changed: DecideNextActionLauncher()"), FColor::Red);
	
	// Decide next action when launcher is equipped
	
	int32 Action = FMath::RandRange(1, 100);

	float TimeTillNextAction = FMath::RandRange(0.6f, 1.5f);

	UWorld* World = GetWorld();
	if (World)
	{
		FTimerHandle NextActionTimerHandle;

		if (Action <= 45)	// 45% chance
		{
			// Fire launcher
			World->GetTimerManager().SetTimer(NextActionTimerHandle, this, &AVer2_Bot::FireLauncher, TimeTillNextAction, false);

		}
		else if (Action <= 58)	// 13% chance
		{
			// Strafe
			World->GetTimerManager().SetTimer(NextActionTimerHandle, this, &AVer2_Bot::StrafeWithLauncher, TimeTillNextAction, false);

		}
		else if (Action <= 90)	// 32% chance
		{
			// Run at player
			World->GetTimerManager().SetTimer(NextActionTimerHandle, this, &AVer2_Bot::RunAtPlayer, TimeTillNextAction, false);

		}
		else   // 10% chance
		{
			// Put launcher away and fire with rifle
			bIsChangingWeapon = true;
			
			PlayMontage(SwitchWeaponsMontage);
			
			FTimerHandle ChangeWeaponTimerHandle;
			World->GetTimerManager().SetTimer(ChangeWeaponTimerHandle, this, &AVer2_Bot::EquipWeapon, WeaponChangeHalfTime, false);

			// Go to ironsights before firing so fire animation doesn't blend from a idle hip animation
			FTimerHandle GoIronsightsIdleTimerHandle;
			World->GetTimerManager().SetTimer(GoIronsightsIdleTimerHandle, this, &AVer2_Bot::PlayIdleStandingIronsightsMontage, GrenadeLauncherEquipTime + 0.1f, false);

			FTimerHandle EquipAnimFinishedTimerHandle;
			World->GetTimerManager().SetTimer(EquipAnimFinishedTimerHandle, this, &AVer2_Bot::FireSalvo, GrenadeLauncherEquipTime + 0.3f, false);

		}

	}
	
}

void AVer2_Bot::DecideNextActionFromStrafe()
{
	AVer2_Bot::DebugMsg(FString("Function change: DecideNextActionFromStrafe()"), FColor::Red);

	int32 Action = FMath::RandRange(1, 100);

	float TimeTillNextAction = FMath::RandRange(0.5f, 1.0f);

	UWorld* World = GetWorld();
	if (World)
	{
		FTimerHandle DecideWhatToDoTimerHandle;

		// If low on health chance to equip grenade launcher increases
		if (Health / MaxHealth < 0.3f)
		{
			if (Action <= 15)	// 15% chance
			{
				// Move to cover nearby
				World->GetTimerManager().SetTimer(DecideWhatToDoTimerHandle, this, &AVer2_Bot::RunFindCoverQuery, TimeTillNextAction, false);

			}
			else if (Action <= 60)	// 45% chance
			{
				// Strafe and fire
				World->GetTimerManager().SetTimer(DecideWhatToDoTimerHandle, this, &AVer2_Bot::RunStrafeQuery, TimeTillNextAction, false);

			}
			else    // 40% chance
			{
				// Grenade launcher routine
				World->GetTimerManager().SetTimer(DecideWhatToDoTimerHandle, this, &AVer2_Bot::BeginGrenadeLauncherRoutine, TimeTillNextAction, false);

			}

		}
		else
		{
			if (Action <= 10)	// 10% chance
			{

				// Find and move to cover
				World->GetTimerManager().SetTimer(DecideWhatToDoTimerHandle, this, &AVer2_Bot::RunFindCoverQuery, TimeTillNextAction, false);

			}
			else if (Action <= 80)	// 70% chance
			{
				// Continue strafing
				World->GetTimerManager().SetTimer(DecideWhatToDoTimerHandle, this, &AVer2_Bot::RunStrafeQuery, TimeTillNextAction, false);

			}
			else    // 20% chance
			{
				// Grenade launcher routine
				World->GetTimerManager().SetTimer(DecideWhatToDoTimerHandle, this, &AVer2_Bot::BeginGrenadeLauncherRoutine, TimeTillNextAction, false);

			}

		}
	}

}

void AVer2_Bot::BeginCoverRoutine()
{
	AVer2_Bot::DebugMsg(FString("Function changed: BeginCoverRoutine()"), FColor::Red);

	if (BotController)
	{
		BotController->StopMovement();
	}


	// Check if it is high or low cover
	// If low cover then continue
	// If high cover then do something else
	if (CheckCoverHeight())
	{
		// Behind high cover

		BeginHighCoverRoutine();

		return;

	}
	
	//ChangeMoveSpeed(CrouchMovementSpeed);

	UWorld* World = GetWorld();

	if (World)
	{
		FireUponReachingCover();

	}

}

bool AVer2_Bot::CheckCoverHeight()
{
	// This method assumes the bot is already behind some form of cover and is standing 

	if (BotController && Player)
	{
		if (!BotController->LineOfSightTo(Player))
		{
			// Raycast from head to in front by about 150 units to see if no LoS is due 
			// to something right in front of the bot. This means the bot is behind high cover.
			UWorld* World = GetWorld();
			if (GetMesh() && Player && World)
			{
				FHitResult HitResult = FHitResult();
				
				FVector StartTrace;
				if (bIsCrouching)
				{
					StartTrace = GetMesh()->GetSocketLocation(FName("headSocket")) + FVector(0.f, 0.f, StandingCrouchingHeightDifference);
				}
				
				else
				{
					StartTrace = GetMesh()->GetSocketLocation(FName("headSocket"));
				}
			
				const FVector ForwardVector = ( (Player->GetActorLocation() - GetActorLocation()) / (GetActorLocation() - Player->GetActorLocation()).Size() ) * 150.f;
				const FVector EndTrace = ForwardVector + StartTrace;
				FCollisionQueryParams CQP = FCollisionQueryParams();
				CQP.AddIgnoredActor(this);

				if (World->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Visibility, CQP))
				{
					// If here then line trace hit something
					
					DrawDebugLine(World, StartTrace, EndTrace, FColor::Red, false, 10.f, 0, 50.f);
					
					AVer2_Bot::DebugMsg(FString("Check cover height determined high cover"), FColor::Blue);
						
					// Something right in front of bot. Behind high cover
					return true;
					
				}
				else
				{
					// Player is the one hiding behind cover
					return false;
				}
			}
			
		}
		else
		{
			// behind low cover
			return false;
		}
	}
	
	return false;

}

void AVer2_Bot::BeginHighCoverRoutine()
{
	AVer2_Bot::DebugMsg(FString("Function change: BeginHighCoverRoutine()"), FColor::Red);
	
	HighCoverLocation = GetActorLocation();

	if (BotController && Player)
	{
		BotController->SetFocus(Player);
	}

	NumHighCoverPopOuts = FMath::RandRange(1, 2);

	//ChangeMoveSpeed(StrafeMovementSpeed);

	StandUp(bWasCrouched);

	RunHighCoverStrafeQuery();

}

void AVer2_Bot::RunHighCoverStrafeQuery()
{
	AVer2_Bot::DebugMsg(FString("Function change: RunHighCoverStrafeQuery()"), FColor::Red);
	
	EQSHighCoverStrafeQuery.Execute(EEnvQueryRunMode::SingleResult, this, &AVer2_Bot::OnHighCoverStrafeQueryFinished);

}

void AVer2_Bot::OnHighCoverStrafeQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	//AVer2_Bot::DebugMsg(FString("Function change: OnHighCoverStrafeQueryFinished()"), FColor::Red);

	MoveToLocation = FVector(0.f);
	MoveToLocation = Result->GetItemAsLocation(0);

	if (MoveToLocation != FVector(0.f))
	{
		if (BotController)
		{
			ChangeMoveSpeed(StrafeMovementSpeed);
			
			// Uncommented to get animation working pretty good
			StopMontages();

			BotController->MoveToLocation(MoveToLocation, 15.f);

			// Check if strafe has completed
			CheckProgressOfHighCoverStrafeMove();

		}

	}
	else
	{
		// I assume if here then no suitable strafe location could be found. 
		// Find cover or just stand and shoot or do something else

		AVer2_Bot::DebugMsg(FString("high cover strafe query did not find location"), FColor::Blue);

	}

}

void AVer2_Bot::CheckProgressOfHighCoverStrafeMove()
{
	//AVer2_Bot::DebugMsg(FString("Function change: CheckProgressOfHighCoverStrafeMove()"), FColor::Red);

	if (BotController)
	{
		if (BotController->GetMoveStatus() != EPathFollowingStatus::Moving)
		{
			// Strafe move complete
			FireHighCoverSalvo();

		}
		else
		{
			UWorld* World = GetWorld();
			if (World)
			{
				FTimerHandle DummyTimerHandle;
				World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::CheckProgressOfHighCoverStrafeMove, 0.25f, false);

			}
			
		}
	}

}

void AVer2_Bot::FireHighCoverSalvo()
{
	AVer2_Bot::DebugMsg(FString("Function change: FireHighCoverSalvo()"), FColor::Red);

	NumShots = FMath::RandRange(3, 7);

	FireHighCover();

}

void AVer2_Bot::FireHighCover()
{
	// Check if shot hit or miss
	if (Player)
	{
		float DistanceToPlayer = (Player->GetActorLocation() - GetActorLocation()).Size();

		if (DistanceToPlayer <= MaxRange)
		{
			// Player is in range

			if (BotController && BotController->LineOfSightTo(Player))
			{
				// Player in LoS, shot will be fired

				// Can get a free shot, actually you can't 
				ClipAmmo--;

				NumShots--;

				// At MaxRange a ~10% chance to hit. At point blank a 99% chance to hit
				float Accuracy = (110.f - ((DistanceToPlayer / MaxRange) * 100.f)) * 0.9f;

				float Result = FMath::RandRange(0.f, 100.f);

				bIsFiring = true;

				if (Result <= Accuracy)
				{
					// Shot hit
					FireHittingShot(Player->GetActorLocation());

				}
				else
				{
					// Shot missed
					FireMissingShot(Player->GetActorLocation());

				}

				PlayMontage(FireRifleIronsightsMontage);

				// Call OnFireFinished after FireDelay
				UWorld* World = GetWorld();

				if (World)
				{
					World->GetTimerManager().SetTimer(FinishedShotTimerHandle, this, &AVer2_Bot::OnFireHighCoverFinish, FireDelay, false);

				}


			}
			else
			{
				// player not in LoS
				UWorld* World = GetWorld();

				if (World)
				{
					float StartLookingDelay = FMath::RandRange(1.2f, 2.8f);

					FTimerHandle StartLookingAroundTimerHandle;
					World->GetTimerManager().SetTimer(StartLookingAroundTimerHandle, this, &AVer2_Bot::LookAround, StartLookingDelay, false);

				}

			}

		}
		else
		{
			// player is not in range. I consider this the same case as not in LoS
			UWorld* World = GetWorld();

			if (World)
			{
				float StartLookingDelay = FMath::RandRange(1.2f, 2.8f);

				FTimerHandle StartLookingAroundTimerHandle;
				World->GetTimerManager().SetTimer(StartLookingAroundTimerHandle, this, &AVer2_Bot::LookAround, StartLookingDelay, false);

			}

		}

	}

}

void AVer2_Bot::OnFireHighCoverFinish()
{
	AVer2_Bot::DebugMsg(FString("Function change: OnFireHighCoverFinish()"), FColor::Red);

	UWorld* World = GetWorld();

	if (ClipAmmo > 0)
	{
		// Keep firing if more shots to come
		if (NumShots > 0)
		{
			FireHighCover();

		}
		else
		{
			// Salvo has finished
			bIsFiring = false;

			if (World)
			{
				FTimerHandle DummyTimerHandle;
				World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::ReturnToHighCover, 0.25f, false);

			}

		}

	}
	else
	{
		FTimerHandle DummyTimerHandle;
		World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::ReloadHighCover, 0.25f, false);

	}

}

void AVer2_Bot::ReloadHighCover()
{
	AVer2_Bot::DebugMsg(FString("Function changed: ReloadHighCover()"), FColor::Red);

	bIsFiring = false;

	bIsReloading = true;

	if (bIsCrouching)
	{
		PlayMontage(ReloadRifleMontage);
	}
	else
	{
		PlayMontage(ReloadRifleIronsightsMontage);
	}

	FTimerHandle DummyTimerHandle;

	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::OnReloadHighCoverFinish, ReloadTime, false);

	}

}

void AVer2_Bot::OnReloadHighCoverFinish()
{
	AVer2_Bot::DebugMsg(FString("Function change: OnReloadHighCoverFinish()"), FColor::Red);

	bIsReloading = false;

	ClipAmmo = MaxClipAmmo;

	if (NumShots > 0)
	{
		// Continue with salvo after reload
		FireHighCover();

	}
	else
	{
		ReturnToHighCover();
	}

}

void AVer2_Bot::ReturnToHighCover()
{
	AVer2_Bot::DebugMsg(FString("Function change: ReturnToHighCover()"), FColor::Red);
	
	if (HighCoverLocation != FVector(0.f))
	{
		if (BotController)
		{
			ChangeMoveSpeed(StrafeMovementSpeed);
			
			// Uncommented to get animation working pretty good
			StopMontages();

			BotController->MoveToLocation(HighCoverLocation, 5.f, true, true, false, true, 0, false);

			CheckProgressOfReturnToHighCover();

		}
		
	}
	
}

void AVer2_Bot::CheckProgressOfReturnToHighCover()
{
	//AVer2_Bot::DebugMsg(FString("Function change: CheckProgressOfReturnToHighCover()"), FColor::Red);

	if (HighCoverLocation != FVector(0.f))
	{
		if (BotController)
		{
			// Trying this check move status to not check for distance, only for move status

			if (BotController->GetMoveStatus() != EPathFollowingStatus::Moving)
			{
				// Has returned to location of high cover

				// Check if high cover is still high
				if (CheckCoverHeight())
				{
					UWorld* World = GetWorld();

					// Still behind high cover
					NumHighCoverPopOuts--;

					if (NumHighCoverPopOuts > 0)
					{
						// Pop out from high cover again
						if (World)
						{
							FTimerHandle PopOutAgainTimerHandle;
							World->GetTimerManager().SetTimer(PopOutAgainTimerHandle, this, &AVer2_Bot::RunHighCoverStrafeQuery, FMath::RandRange(0.8f, 1.6f), false);

						}
						
					}
					else
					{
						// Cover is still high
						DecideNextAction();

					}

				}
				else
				{
					// Now high cover is low
					CrouchInCover();

					DecideNextAction();

				}

			}
			else
			{
				UWorld* World = GetWorld();
				if (World)
				{
					FTimerHandle DummyTimerHandle;
					World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::CheckProgressOfReturnToHighCover, 0.25f, false);

				}
			}

		}
		
	}

}

void AVer2_Bot::FireUponReachingCover()
{
	// Decides whether to fire straight away or crouch straight away
	
	AVer2_Bot::DebugMsg(FString("Function change: FireUponReachingCover()"), FColor::Red);

	if (BotController && Player)
	{
		BotController->SetFocus(Player);
	}

	UWorld* World = GetWorld();
	if (World)
	{
		if (bIsCrouching)
		{
			// If crouching then have just moved from other cover
			
			float TimeTillAction = FMath::RandRange(1.f, 3.f);
			FTimerHandle ActionTimerHandle;
			
			if (CheckCoverHeight())
			{
				// Behind high cover
				World->GetTimerManager().SetTimer(ActionTimerHandle, this, &AVer2_Bot::BeginHighCoverRoutine, TimeTillAction, false);

			}
			else
			{
				World->GetTimerManager().SetTimer(ActionTimerHandle, this, &AVer2_Bot::StandAndFire, TimeTillAction, false);

			}
			
		}
		else
		{
			bWasCrouched = true;

			float FireStraightAway = FMath::RandRange(0.0f, 100.0f);

			if (FireStraightAway < 50.f && BotController && Player && BotController->LineOfSightTo(Player))
			{
				// Fire before crouching in cover
				float TimeTillFire = FMath::RandRange(0.1f, 0.2f);
				FTimerHandle FireSalvoTimerHandle;
				World->GetTimerManager().SetTimer(FireSalvoTimerHandle, this, &AVer2_Bot::FireSalvo, TimeTillFire, false);
			
			}
			else
			{
				// crouch upon getting to cover
				CrouchInCover();
				
				float TimeTillStandAndFire = FMath::RandRange(1.8f, 3.2f);
				FTimerHandle StandUpTimerHandle;
				World->GetTimerManager().SetTimer(StandUpTimerHandle, this, &AVer2_Bot::StandAndFire, TimeTillStandAndFire, false);

			}

		}
	}
	
}

void AVer2_Bot::StandAndFire()
{
	// This function can also be used to fire from standing in the first place 
	// and the bot should not crouch afterwards
	
	AVer2_Bot::DebugMsg(FString("Function change: StandAndFire()"), FColor::Red);

	if (BotController && Player)
	{
		BotController->SetFocus(Player);
	}
	
	if (bIsCrouching)
	{
		bWasCrouched = true;
	}
	
	StandUp(bWasCrouched);

	UWorld* World = GetWorld();
	if (World)
	{
		if (!World->GetTimerManager().IsTimerActive(FireAfterStandingTimerHandle))
		{
			float TimeTillFire = FMath::RandRange(0.5f, 1.2f);
			
			World->GetTimerManager().SetTimer(FireAfterStandingTimerHandle, this, &AVer2_Bot::FireSalvo, TimeTillFire, false);

		}

	}

}

void AVer2_Bot::OnFireFinished()
{
	AVer2_Bot::DebugMsg(FString("Function change: OnFireFinished()"), FColor::Red);

	UWorld* World = GetWorld();

	if (ClipAmmo > 0)
	{
		// Keep firing if more shots to come
		if (NumShots > 0)
		{
			Fire();

		}
		else
		{
			// Salvo has finished
			bIsFiring = false;

			if (bWasCrouched)
			{
				if (World)
				{
					FTimerHandle DummyTimerHandle;
					World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::CrouchInCover, 0.25f, false);

				}

			}

			DecideNextAction();

		}

	}
	else
	{
		if (World)
		{
			FTimerHandle DummyTimerHandle;
			World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::Reload, 0.25f, false);

		}

	}

}

void AVer2_Bot::CrouchInCover()
{
	AVer2_Bot::DebugMsg(FString("Function change: CrouchInCover()"), FColor::Red);
	
	ChangeMoveSpeed(CrouchMovementSpeed);

	bIsCrouching = true;
	bWasCrouched = false;

	PlayMontage(StandToCrouchMontage);

}

void AVer2_Bot::StandUp(bool bCrouchAfterwards)
{
	if (bIsCrouching)
	{
		bIsCrouching = false;
		bWasCrouched = bCrouchAfterwards;

		if (EquippedWeapon == EWeaponEnum::Rifle)
		{
			PlayMontage(CrouchToStandMontage);
		}
		else
		{
			PlayMontage(CrouchToStandHipMontage);
		}

	}

}

void AVer2_Bot::LookAround()
{
	bIsFiring = false;
	
	// This check does still counts the bot as seeing the player if they are say within MaxRange 
	// but behind the bot out in the open.
	if (BotController && Player)
	{
		float DistanceToPlayer = (Player->GetActorLocation() - GetActorLocation()).Size();
		
		if (BotController->LineOfSightTo(Player) && DistanceToPlayer < MaxRange)
		{
			// Player is in sight
			FireSalvo();
			return;
		}

	}

	bIsCautious = true;
	
	AVer2_Bot::DebugMsg(FString("Function change: LookAround()"), FColor::Red);
	
	NumLooks = FMath::RandRange(5, 8);

	int32 FirstLookDirection = FMath::RandRange(1, 2);

	switch (FirstLookDirection)
	{
	case 1:
		LookLeft();
		break;

	case 2:
		LookRight();
		break;

	default:
		break;

	}

}

void AVer2_Bot::LookLeft()
{
	if (!bIsCautious)
	{
		return;
	}

	AVer2_Bot::DebugMsg(FString("Function change: LookLeft()"), FColor::Red);
	
	UWorld* World = GetWorld();

	if (BotController)
	{
		
		BotController->ClearFocus(EAIFocusPriority::Gameplay);

		FVector NewFocalPoint = GetActorForwardVector();

		if (bIsLookingStraightAhead)
		{
			NewFocalPoint = NewFocalPoint.RotateAngleAxis(-LookAroundAmount, GetActorUpVector()) + GetActorLocation();

			if (World)
			{
				BotController->SetFocalPoint(NewFocalPoint);

			}
			
		}
		else if (bIsLookingRight)
		{
			NewFocalPoint = NewFocalPoint.RotateAngleAxis(LookAroundAmount, GetActorUpVector()) + GetActorLocation();
			
			if (World)
			{
				BotController->SetFocalPoint(NewFocalPoint);

			}
			
		}

		bIsLookingLeft = true;

		if (--NumLooks > 0)
		{

			if (World)
			{
				float LookTime = FMath::RandRange(MinLookTime, MaxLookTime);
				World->GetTimerManager().SetTimer(LookAroundTimerHandle, this, &AVer2_Bot::LookStraightAhead, LookTime, false);

			}

		}
		else
		{
			// finished looking around. Last look lasts a bit longer
			float LookTime = FMath::RandRange(MinLookTime, MaxLookTime) + 3.0f;
			World->GetTimerManager().SetTimer(LookAroundTimerHandle, this, &AVer2_Bot::GoIntoCautionPhase, LookTime, false);
		
			// Show question mark above head a bit before last look ends
			FTimerHandle ShowQuestionMarkTimerhandle;
			World->GetTimerManager().SetTimer(ShowQuestionMarkTimerhandle, this, &AVer2_Bot::ShowQuestionMarkIcon, LookTime - 2.5f, false);

		}
		
	}

}

void AVer2_Bot::LookStraightAhead()
{
	if (!bIsCautious)
	{
		return;
	}
	
	AVer2_Bot::DebugMsg(FString("Function change: LookStraightAhead()"), FColor::Red);
	
	UWorld* World = GetWorld();

	if (BotController)
	{
		
		BotController->ClearFocus(EAIFocusPriority::Gameplay);

		FVector NewFocalPoint = GetActorForwardVector();

		if (bIsLookingLeft)
		{
			NewFocalPoint = NewFocalPoint.RotateAngleAxis(LookAroundAmount, GetActorUpVector()) + GetActorLocation();

			if (World)
			{
				BotController->SetFocalPoint(NewFocalPoint);
			}

		}
		else if (bIsLookingRight)
		{
			NewFocalPoint = NewFocalPoint.RotateAngleAxis(-LookAroundAmount, GetActorUpVector()) + GetActorLocation();

			if (World)
			{
				BotController->SetFocalPoint(NewFocalPoint);
			}

		}
			
		bIsLookingStraightAhead = true;

		if (--NumLooks > 0)
		{
	
			if (World)
			{
				float LookTime = FMath::RandRange(MinLookTime, MaxLookTime);

				int32 Decision = FMath::RandRange(1, 2);
	
				switch (Decision)
				{
				case 1:
					World->GetTimerManager().SetTimer(LookAroundTimerHandle, this, &AVer2_Bot::LookLeft, LookTime, false);
					break;

				case 2:
					World->GetTimerManager().SetTimer(LookAroundTimerHandle, this, &AVer2_Bot::LookRight, LookTime, false);
					break;

				default:
					break;
				}


			}	

		}
		else
		{
			// finished looking around. Last look lasts a bit longer
			float LookTime = FMath::RandRange(MinLookTime, MaxLookTime) + 3.0f;
			World->GetTimerManager().SetTimer(LookAroundTimerHandle, this, &AVer2_Bot::GoIntoCautionPhase, LookTime, false);

			// Show question mark above head a bit before last look ends
			FTimerHandle ShowQuestionMarkTimerhandle;
			World->GetTimerManager().SetTimer(ShowQuestionMarkTimerhandle, this, &AVer2_Bot::ShowQuestionMarkIcon, LookTime - 2.5f, false);

		}
		
	}

}

void AVer2_Bot::LookRight()
{
	if (!bIsCautious)
	{
		return;
	}
	
	AVer2_Bot::DebugMsg(FString("Function change: LookRight()"), FColor::Red);
	
	UWorld* World = GetWorld();

	if (BotController)
	{
		
		BotController->ClearFocus(EAIFocusPriority::Gameplay);

		FVector NewFocalPoint = GetActorForwardVector();
			
		if (bIsLookingStraightAhead)
		{
			NewFocalPoint = NewFocalPoint.RotateAngleAxis(LookAroundAmount, GetActorUpVector()) + GetActorLocation();
				
			if (World)
			{
				BotController->SetFocalPoint(NewFocalPoint);
			}
		}
		else if (bIsLookingLeft)
		{
			NewFocalPoint = NewFocalPoint.RotateAngleAxis(-LookAroundAmount, GetActorUpVector()) + GetActorLocation();
				
			if (World)
			{
				BotController->SetFocalPoint(NewFocalPoint);
			}
		}
			
		bIsLookingRight = true;

		if (--NumLooks > 0)
		{

			if (World)
			{
				float LookTime = FMath::RandRange(MinLookTime, MaxLookTime);
				World->GetTimerManager().SetTimer(LookAroundTimerHandle, this, &AVer2_Bot::LookStraightAhead, LookTime, false);

			}

		}
		else
		{
			// finished looking around. Last look lasts a bit longer
			float LookTime = FMath::RandRange(MinLookTime, MaxLookTime) + 3.0f;
			World->GetTimerManager().SetTimer(LookAroundTimerHandle, this, &AVer2_Bot::GoIntoCautionPhase, LookTime, false);
			
			// Show question mark above head a bit before last look ends
			FTimerHandle ShowQuestionMarkTimerhandle;
			World->GetTimerManager().SetTimer(ShowQuestionMarkTimerhandle, this, &AVer2_Bot::ShowQuestionMarkIcon, LookTime - 2.5f, false);

		}
		
	}

}

void AVer2_Bot::GoIntoCautionPhase()
{
	AVer2_Bot::DebugMsg(FString("Function change: GoIntoCautionPhase()"), FColor::Red);
	
	if (BotController)
	{
		// Just go back to patrolling around patrol points
		BotController->ClearFocus(EAIFocusPriority::Gameplay);
		
		bIsCautious = false;

		bIsAlerted = false;

		StartPatrol();

	}

}

void AVer2_Bot::StartStandAndStrafeRoutine()
{

	AVer2_Bot::DebugMsg(FString("Function change: StartStandAndStrafeRoutine()"), FColor::Red);
	
	StandUp(false);
	//bWasCrouched = false;	// I think this is ok here

	if (BotController && Player)
	{
		BotController->SetFocus(Player);
	}

	RunStrafeQuery();

}

void AVer2_Bot::RunStrafeQuery()
{
	AVer2_Bot::DebugMsg(FString("Function change: RunStrafeQuery()"), FColor::Red);
	
	EQSStrafeQuery.Execute(EEnvQueryRunMode::SingleResult, this, &AVer2_Bot::OnStrafeQueryFinished);

}

void AVer2_Bot::OnStrafeQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{

	AVer2_Bot::DebugMsg(FString("Function change: OnStrafeQueryFinished()"), FColor::Red);

	MoveToLocation = FVector(0.f);
	MoveToLocation = Result->GetItemAsLocation(0);

	if (MoveToLocation != FVector(0.f))
	{
		if (BotController)
		{
			ChangeMoveSpeed(StrafeMovementSpeed);

			// Uncommented to get animation working pretty good
			StopMontages();

			BotController->MoveToLocation(MoveToLocation, 5.f);

			UWorld* World = GetWorld();
			if (World)
			{
				// Fire while strafing
				FireSalvoWhileStrafing();
			}

			CheckIfStrafeMoveToHasCompleted();

		}
		
	}
	else
	{
		// I assume if here then no suitable strafe location could be found. 
		// Find cover or just stand and shoot or do something else

		AVer2_Bot::DebugMsg(FString("Strafe query did not find location"), FColor::Blue);

	}
}

void AVer2_Bot::CheckIfStrafeMoveToHasCompleted()
{
	//AVer2_Bot::DebugMsg(FString("Function change: CheckIfStrafeMoveToHasCompleted()"), FColor::Red);

	if (MoveToLocation != FVector(0.f))
	{
		if (BotController->GetMoveStatus() != EPathFollowingStatus::Moving)
		{
			if (!bIsFiringAndStrafing)
			{
				UWorld* World = GetWorld();
				if (World)
				{
					FTimerHandle DummyTimerHandle;
					World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::DecideNextAction, 0.5f, false);

				}

			}
			else
			{
				// Nothing should be needed here. Bot should already be firing and will do something
				// else once that finishes.
			}

		}
		else
		{
			UWorld* World = GetWorld();
			if (World)
			{
				World->GetTimerManager().SetTimer(StrafeMoveTimerHandle, this, &AVer2_Bot::CheckIfStrafeMoveToHasCompleted, 0.25f, false);

			}
		}

	}

}

void AVer2_Bot::FireSalvoWhileStrafing()
{
	AVer2_Bot::DebugMsg(FString("Function change: FireSalvoWhileStrafing()"), FColor::Red);
	
	bIsFiringAndStrafing = true;

	NumShots = FMath::RandRange(4, 6);

	FireWhileStrafing();

}

void AVer2_Bot::FireWhileStrafing()
{
	AVer2_Bot::DebugMsg(FString("Function change: FireWhileStrafing()"), FColor::Red);

	/** All this below was copied from Fire() */

	// Check if shot hit or miss
	if (Player)
	{
		float DistanceToPlayer = (Player->GetActorLocation() - GetActorLocation()).Size();

		if (DistanceToPlayer <= MaxRange)
		{
			// Player is in range

			if (BotController && BotController->LineOfSightTo(Player))
			{
				// Player in LoS, shot will be fired

				// Could get a free shot maybe
				ClipAmmo--;

				NumShots--;

				float Accuracy = ( (110.f - ((DistanceToPlayer / MaxRange) * 100.f)) * 0.9f ) * StrafeAccuracyPenalty;

				float Result = FMath::RandRange(0.f, 100.f);

				bIsFiring = true;

				if (Result <= Accuracy)
				{
					// Shot hit
					FireHittingShot(Player->GetActorLocation());

				}
				else
				{
					// Shot missed
					FireMissingShot(Player->GetActorLocation());

				}

				PlayMontage(FireRifleIronsightsMontage);

				// Call OnFireFinished after FireDelay
				UWorld* World = GetWorld();

				if (World)
				{
					World->GetTimerManager().SetTimer(FinishedShotTimerHandle, this, &AVer2_Bot::OnStrafeFireFinished, FireDelay, false);

				}


			}
			else
			{
				// player not in LoS
				UWorld* World = GetWorld();

				if (World)
				{
					float StartLookingDelay = FMath::RandRange(1.2f, 2.8f);

					FTimerHandle StartLookingAroundTimerHandle;
					World->GetTimerManager().SetTimer(StartLookingAroundTimerHandle, this, &AVer2_Bot::LookAround, StartLookingDelay, false);

				}

			}

		}
		else
		{
			// player is not in range. I consider this the same case as not in LoS
			UWorld* World = GetWorld();

			if (World)
			{
				float StartLookingDelay = FMath::RandRange(1.2f, 2.8f);

				FTimerHandle StartLookingAroundTimerHandle;
				World->GetTimerManager().SetTimer(StartLookingAroundTimerHandle, this, &AVer2_Bot::LookAround, StartLookingDelay, false);

			}

		}

	}

}

void AVer2_Bot::OnStrafeFireFinished()
{
	/** Below is copied from OnFireFinished() */

	AVer2_Bot::DebugMsg(FString("Function change: OnStrafeFireFinished()"), FColor::Red);

	if (ClipAmmo > 0)
	{
		// Keep firing if more shots to come
		if (NumShots > 0)
		{
			Fire();

		}
		else
		{
			// Salvo has finished
			bIsFiring = false;

			bIsFiringAndStrafing = false;

			DecideNextActionFromStrafe();

		}

	}
	else
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FTimerHandle DummyTimerHandle;
			World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::ReloadWhileStrafing, 0.25f, false);

		}
		
	}

}

void AVer2_Bot::ReloadWhileStrafing()
{
	AVer2_Bot::DebugMsg(FString("Function changed: ReloadWhileStrafing()"), FColor::Red);

	/** Below is copied from Reload() */

	bIsFiring = false;

	bIsReloading = true;

	PlayMontage(ReloadRifleIronsightsMontage);

	FTimerHandle DummyTimerHandle;

	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::OnReloadWhileStrafingFinish, ReloadTime, false);

	}

}

void AVer2_Bot::OnReloadWhileStrafingFinish()
{
	AVer2_Bot::DebugMsg(FString("Function change: OnReloadWhileStrafingFinished()"), FColor::Red);

	/** Copied from OnReloadFinished */

	bIsReloading = false;

	ClipAmmo = MaxClipAmmo;

	if (NumShots > 0)
	{
		// Continue with salvo after reload
		FireWhileStrafing();

	}
	else 
	{
		bIsFiringAndStrafing = false;
		
		DecideNextActionFromStrafe();

	}

}

void AVer2_Bot::BeginGrenadeLauncherRoutine()
{
	AVer2_Bot::DebugMsg(FString("Function change: BeginGrenadeLauncherRoutine()"), FColor::Red);
	
	UWorld* World = GetWorld();

	if (World)
	{
		bIsChangingWeapon = true;
		
		bIsCrouching = false;

		PlayMontage(SwitchWeaponsMontage);
		
		FTimerHandle ChangeWeaponTimerHandle;
		World->GetTimerManager().SetTimer(ChangeWeaponTimerHandle, this, &AVer2_Bot::EquipWeapon, WeaponChangeHalfTime, false);
		
		FTimerHandle EquipAnimFinishedTimerHandle;
		World->GetTimerManager().SetTimer(EquipAnimFinishedTimerHandle, this, &AVer2_Bot::StartGrenadeLauncherMovement, GrenadeLauncherEquipTime, false);

	}
	
}

void AVer2_Bot::StartGrenadeLauncherMovement()
{
	AVer2_Bot::DebugMsg(FString("Function changed: StartGrenadeLauncherMovement()"), FColor::Red);
	
	//ChangeMoveSpeed(JogSpeed);

	if (bIsCrouching)
	{
		StandUp(false);
	}
	//else
	//{
	//	PlayMontage(IdleHipMontage);
	//}

	// Fire launcher shot 0.5s after finishing equipping it
	UWorld* World = GetWorld();
	if (World)
	{
		FTimerHandle FireLauncherTimerHandle;
		World->GetTimerManager().SetTimer(FireLauncherTimerHandle, this, &AVer2_Bot::FireLauncher, 0.5f, false);

	}

}

void AVer2_Bot::StrafeWithLauncher()
{
	AVer2_Bot::DebugMsg(FString("Function changed: StrafeWithLauncher()"), FColor::Red);
	
	bIsFiring = false;
	
	RunLauncherStrafeQuery();

}

void AVer2_Bot::RunAtPlayer()
{
	AVer2_Bot::DebugMsg(FString("Function changed: RunAtPlayer()"), FColor::Red);

	if (Player)
	{
		if (BotController)
		{
			ChangeMoveSpeed(JogSpeed);

			// Uncommented to get animation working pretty good
			StopMontages();

			BotController->MoveTo(Player);

			if (EquippedWeapon == EWeaponEnum::Launcher)
			{
				// This line makes the check loop know it is checking how close we are to the player, not a vector
				MoveToLocation = FVector(0.f);
				
				EnterMoveToCheckLoopLauncher();
			}
			else
			{
				// Enter a different checking loop
				MoveToLocation = FVector(0.f);

				UWorld* World = GetWorld();
				if (World)
				{
					// Fire while running at player
					float StartFire = FMath::RandRange(0.5f, 2.0f);
					World->GetTimerManager().SetTimer(FireRifleWhileRunningAtPlayerTimerHandle, this, &AVer2_Bot::FireRifleSalvoWhileRunningAtPlayer, StartFire, false);

				}

			}

		}

	}

}

void AVer2_Bot::FireRifleSalvoWhileRunningAtPlayer()
{
	AVer2_Bot::DebugMsg(FString("Function changed: FireRifleSalvoWhileRunningAtPlayer()"), FColor::Red);

	// Decide on a random number of shots
	NumShots = FMath::RandRange(5, 9);

	FireRunningAtPlayer();

}

void AVer2_Bot::FireRunningAtPlayer()
{
	AVer2_Bot::DebugMsg(FString("Function changed: FireRunningAtPlayer()"), FColor::Red);
	
	/** Below has been copied fron Fire() */

	// Check if shot hit or miss
	if (Player)
	{
		float DistanceToPlayer = (Player->GetActorLocation() - GetActorLocation()).Size();

		if (DistanceToPlayer <= MaxRange)
		{
			// Player is in range

			if (BotController && BotController->LineOfSightTo(Player))
			{
				// Player in LoS, shot will be fired

				// Can get a free shot
				ClipAmmo--;

				NumShots--;

				// At MaxRange a ~10% chance to hit. At point blank a 99% chance to hit
				float Accuracy = (110.f - ((DistanceToPlayer / MaxRange) * 100.f)) * 0.9f;

				float Result = FMath::RandRange(0.f, 100.f);

				bIsFiring = true;

				if (Result <= Accuracy)
				{
					// Shot hit
					FireHittingShot(Player->GetActorLocation());

				}
				else
				{
					// Shot missed
					FireMissingShot(Player->GetActorLocation());

				}

				PlayMontage(FireRifleHipMontage);

				// Call OnFireRunningAtPlayerFinish after FireDelay
				UWorld* World = GetWorld();

				if (World)
				{
					World->GetTimerManager().SetTimer(FinishedShotTimerHandle, this, &AVer2_Bot::OnFireRunningAtPlayerFinish, FireDelay, false);

				}


			}
			else
			{
				// player not in LoS
				UWorld* World = GetWorld();

				if (World)
				{
					float StartLookingDelay = FMath::RandRange(1.2f, 2.8f);

					FTimerHandle StartLookingAroundTimerHandle;
					World->GetTimerManager().SetTimer(StartLookingAroundTimerHandle, this, &AVer2_Bot::LookAround, StartLookingDelay, false);

				}

			}

		}
		else
		{
			// player is not in range. I consider this the same case as not in LoS
			UWorld* World = GetWorld();

			if (World)
			{
				float StartLookingDelay = FMath::RandRange(1.2f, 2.8f);

				FTimerHandle StartLookingAroundTimerHandle;
				World->GetTimerManager().SetTimer(StartLookingAroundTimerHandle, this, &AVer2_Bot::LookAround, StartLookingDelay, false);

			}

		}

	}

}

void AVer2_Bot::OnFireRunningAtPlayerFinish()
{
	AVer2_Bot::DebugMsg(FString("Function changed: OnFireRunningAtPlayerFinish()"), FColor::Red);
	
	/** Below copied from OnFireFinished*/

	if (ClipAmmo > 0)
	{
		// Keep firing if more shots to come
		if (NumShots > 0)
		{
			FireRunningAtPlayer();

		}
		else
		{
			// Salvo has finished
			bIsFiring = false;

			StartStandAndStrafeRoutine();

		}

	}
	else
	{
		ReloadWhileRunningAtPlayer();

	}

}

void AVer2_Bot::ReloadWhileRunningAtPlayer()
{
	AVer2_Bot::DebugMsg(FString("Function changed: ReloadWhileRunningAtPlayer()"), FColor::Red);
	
	bIsFiring = false;

	bIsReloading = true;

	PlayMontage(ReloadRifleMontage);

	FTimerHandle DummyTimerHandle;

	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::OnReloadRunningAtPlayerFinish, ReloadTime, false);

	}

}

void AVer2_Bot::OnReloadRunningAtPlayerFinish()
{
	AVer2_Bot::DebugMsg(FString("Function changed: OnReloadRunningAtPlayerFinish()"), FColor::Red);
	
	bIsReloading = false;

	ClipAmmo = MaxClipAmmo;

	if (NumShots > 0)
	{
		// Continue with salvo after reload
		FireRunningAtPlayer();

	}
	else
	{
		DecideNextActionFromStrafe();

	}

}

void AVer2_Bot::ReloadLauncher()
{
	
	AVer2_Bot::DebugMsg(FString("Function changed: ReloadLauncher()"), FColor::Red);

	bIsFiring = false;

	bIsReloading = true;

	PlayMontage(ReloadLauncherMontage);

	FTimerHandle DummyTimerHandle;

	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::OnLauncherReloadFinish, LauncherReloadTime, false);

	}

}

void AVer2_Bot::OnLauncherReloadFinish()
{
	
	AVer2_Bot::DebugMsg(FString("Function changed: OnLauncherReloadFinish()"), FColor::Red);
	
	bIsReloading = false;

	CurrentLauncherAmmo = MaxLauncherAmmo;

	UWorld* World = GetWorld();
	if (World)
	{
		float Delay = FMath::RandRange(0.5f, 1.2f);
		FTimerHandle StartStrafingTimerHandle;
		// Changed from StrafeWithLauncher so after reload bot will fire launcher immediately
		World->GetTimerManager().SetTimer(StartStrafingTimerHandle, this, &AVer2_Bot::FireLauncher, Delay, false);

	}

	


}

void AVer2_Bot::RunLauncherStrafeQuery()
{
	AVer2_Bot::DebugMsg(FString("Function change: RunLauncherStrafeQuery()"), FColor::Red);
	
	EQSStrafeQueryLauncher.Execute(EEnvQueryRunMode::SingleResult, this, &AVer2_Bot::OnLauncherStrafeQueryFinished);

}

void AVer2_Bot::OnLauncherStrafeQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	
	AVer2_Bot::DebugMsg(FString("Function change: OnStrafeQueryFinishedLauncher()"), FColor::Red);

	MoveToLocation = FVector(0.f);
	MoveToLocation = Result->GetItemAsLocation(0);

	if (MoveToLocation != FVector(0.f))
	{
		// Set blackboard key value
		if (BotController)
		{
			ChangeMoveSpeed(JogSpeed);

			// Uncommented to get animation working pretty good
			StopMontages();

			BotController->MoveToLocation(MoveToLocation, 10.f);

			// Check that we are in range of the location
			EnterMoveToCheckLoopLauncher();

		}

	}
	else
	{
		// I assume if here then no suitable strafe location could be found. 
		// Find cover or just stand and shoot or do something else

		AVer2_Bot::DebugMsg(FString("Strafe query did not find location"), FColor::Blue);

	}

}

void AVer2_Bot::EnterMoveToCheckLoopLauncher()
{

	//AVer2_Bot::DebugMsg(FString("Function change: EnterMoveToCheckLoopLauncher()"), FColor::Red);

	if (MoveToLocation != FVector(0.f))
	{

		if (BotController && BotController->GetMoveStatus() != EPathFollowingStatus::Moving)
		{
			// Don't think this line is needed so it has been commented
			//BotController->StopMovement();
			
			DecideNextActionLauncher();

		}
		else
		{
			UWorld* World = GetWorld();
			if (World)
			{
				World->GetTimerManager().SetTimer(LauncherMoveToTimerHandle, this, &AVer2_Bot::EnterMoveToCheckLoopLauncher, 0.25f, false);

			}
		}

	}
	else if (Player)
	{
		// Moving towards player
		float Distance = (Player->GetActorLocation() - GetActorLocation()).Size();

		float DistanceToStopAt = FMath::RandRange(RunAtPlayerStopDistanceMin, RunAtPlayerStopDistanceMax);

		if (Distance <= DistanceToStopAt)
		{
			if (BotController)
			{
				BotController->StopMovement();
			}
			
			DecideNextActionLauncher();

		}
		else
		{
			UWorld* World = GetWorld();
			if (World)
			{
				World->GetTimerManager().SetTimer(LauncherMoveToTimerHandle, this, &AVer2_Bot::EnterMoveToCheckLoopLauncher, 0.25f, false);

			}
		}

	}

}

void AVer2_Bot::BeginMoveToNewCoverRoutine()
{
	AVer2_Bot::DebugMsg(FString("Function change: BeginMoveToNewCoverRoutine()"), FColor::Red);

	// Move speed will speed up to run speed. This should be a different function.
	RunFindNewCoverQuery();

}

void AVer2_Bot::RunFindNewCoverQuery()
{
	AVer2_Bot::DebugMsg(FString("Function change: RunFindNewCoverQuery()"), FColor::Red);

	EQSFindCoverQuery.Execute(EEnvQueryRunMode::SingleResult, this, &AVer2_Bot::OnFindNewCoverQueryFinished);

}

void AVer2_Bot::OnFindNewCoverQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	AVer2_Bot::DebugMsg(FString("Function change: OnFindNewCoverQueryFinished()"), FColor::Red);

	CoverLocation = FVector(0.f);
	CoverLocation = Result->GetItemAsLocation(0);

	if (CoverLocation != FVector(0.f))
	{
		
		float DistanceToNewCover = (CoverLocation - GetActorLocation()).Size();
			
		if (DistanceToNewCover < 800.f)
		{
			// new cover is considered close - crouch move there
			ChangeMoveSpeed(CrouchMovementSpeed);

		}
		else
		{
			// new cover is considered far away - run there
			ChangeMoveSpeed(RunSpeed);
			StandUp(bWasCrouched);
		}

		if (BotController)
		{
			BotController->ClearFocus(EAIFocusPriority::Gameplay);
			
			// Uncommented to get animation working pretty good
			StopMontages();

			BotController->MoveToLocation(CoverLocation, 5.1f);

		}

		CheckProgressOfMoveToCover();

	}
	else
	{
		// I assume if here then no suitable cover location could be found. 
		// Do something else

		AVer2_Bot::DebugMsg(FString("Cover query did not find location"), FColor::Blue);

	}

}

void AVer2_Bot::FireSalvo()
{
	AVer2_Bot::DebugMsg(FString("Function change: FireSalvo()"), FColor::Red);
	
	// Decide on a random number of shots
	NumShots = FMath::RandRange(2, 6);

	Fire();

}

void AVer2_Bot::Fire()
{

	// Check if shot hit or miss
	if (Player)
	{
		float DistanceToPlayer = (Player->GetActorLocation() - GetActorLocation()).Size();

		if (DistanceToPlayer <= MaxRange)
		{
			// Player is in range
			
			if (BotController && BotController->LineOfSightTo(Player))
			{
				// Player in LoS, shot will be fired

				// Can get a free shot, actually you can't 
				ClipAmmo--;

				NumShots--;

				// At MaxRange a ~10% chance to hit. At point blank a 99% chance to hit
				float Accuracy = (110.f - ((DistanceToPlayer / MaxRange) * 100.f)) * 0.9f;

				float Result = FMath::RandRange(0.f, 100.f);

				bIsFiring = true;

				if (Result <= Accuracy)
				{
					// Shot hit
					FireHittingShot(Player->GetActorLocation());

				}
				else
				{
					// Shot missed
					FireMissingShot(Player->GetActorLocation());

				}

				PlayMontage(FireRifleIronsightsMontage);

				// Call OnFireFinished after FireDelay
				UWorld* World = GetWorld();

				if (World)
				{
					World->GetTimerManager().SetTimer(FinishedShotTimerHandle, this, &AVer2_Bot::OnFireFinished, FireDelay, false);

				}


			}
			else
			{
				// player not in LoS
				UWorld* World = GetWorld();
				
				if (World)
				{
					float StartLookingDelay = FMath::RandRange(1.2f, 2.8f);
					
					FTimerHandle StartLookingAroundTimerHandle;
					World->GetTimerManager().SetTimer(StartLookingAroundTimerHandle, this, &AVer2_Bot::LookAround, StartLookingDelay, false);

				}

			}

		}
		else
		{
			// player is not in range. I consider this the same case as not in LoS
			UWorld* World = GetWorld();

			if (World)
			{
				float StartLookingDelay = FMath::RandRange(1.2f, 2.8f);

				FTimerHandle StartLookingAroundTimerHandle;
				World->GetTimerManager().SetTimer(StartLookingAroundTimerHandle, this, &AVer2_Bot::LookAround, StartLookingDelay, false);

			}

		}

	}

}

void AVer2_Bot::FireLauncher()
{
	
	AVer2_Bot::DebugMsg(FString("Function changed: FireLauncher()"), FColor::Red);

	if (CurrentLauncherAmmo > 0 && LauncherMuzzleLocation)
	{
		// Check if there isn't anything right in fron of the bot that would cause the launcher to 
		// blow up instantly by doing a raycast
		UWorld* World = GetWorld();
		FHitResult HitResult = FHitResult();
		const FVector StartTrace = LauncherMuzzleLocation->GetComponentLocation();
		const FVector ForwardVector = ((Player->GetActorLocation() - GetActorLocation()) / (GetActorLocation() - Player->GetActorLocation()).Size()) * 600.f;
		const FVector EndTrace = ForwardVector + StartTrace;
		FCollisionQueryParams CQP = FCollisionQueryParams();
		CQP.AddIgnoredActor(this);
		// Check if Player is nullptr before doing this
		CQP.AddIgnoredActor(Player);

		AVer2_Bot::DebugMsg(FString("FireLauncher: about to do line trace"), FColor::Blue);

		DrawDebugLine(World, StartTrace, EndTrace, FColor::Green, false, 10.f, 0, 30.f);

		if (Player && World && !World->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Visibility, CQP))
		{
			// If here then the trace did not have a blocking hit i.e. nothing obstructing launcher fire

			if (BP_LauncherShot && LauncherMuzzleLocation)
			{
				AVer2_Bot::DebugMsg(FString("Launcher projectile spawned"), FColor::Blue);

				bIsFiring = true;

				CurrentLauncherAmmo--;

				World->SpawnActor<AVer2_LauncherProjectile>(BP_LauncherShot, LauncherMuzzleLocation->GetComponentLocation(), LauncherMuzzleLocation->GetComponentRotation());

				PlayMontage(FireLauncherMontage);
				
				if (LauncherMuzzleFlash)
				{
					LauncherMuzzleFlash->ActivateSystem(true);
					LauncherMuzzleFlash->SetVisibility(true);
				}

				if (LauncherMuzzleSmoke)
				{
					LauncherMuzzleSmoke->SetWorldRotation(LauncherMuzzleLocation->GetComponentRotation());
					LauncherMuzzleSmoke->ActivateSystem(true);
					LauncherMuzzleSmoke->SetVisibility(true);

					FTimerHandle HideSmokeTimerHandle;
					World->GetTimerManager().SetTimer(HideSmokeTimerHandle, this, &AVer2_Bot::MakeSmokeInvisible, 5.f, false);

				}

				// Play sound 
				UMyStatics::PlaySound(this, LauncherFireSounds, LauncherMuzzleLocation->GetComponentLocation(), 0.55f);

				// Move around
				float Time = FMath::RandRange(1.0f, 2.2f);
				FTimerHandle StartMovementTimerHandle;
				World->GetTimerManager().SetTimer(StartMovementTimerHandle, this, &AVer2_Bot::StrafeWithLauncher, Time, false);

			}
			
		}
		else
		{
			// Something in the way of launcher muzzle. Do something else.

			// Put launcher away and get out rifle
			bIsChangingWeapon = true;
			
			PlayMontage(SwitchWeaponsMontage);

			FTimerHandle ChangeWeaponTimerHandle;
			World->GetTimerManager().SetTimer(ChangeWeaponTimerHandle, this, &AVer2_Bot::EquipWeapon, WeaponChangeHalfTime, false);

			// Go to ironsights before firing so fire animation doesn't blend from a idle hip animation
			FTimerHandle GoIronsightsIdleTimerHandle;
			World->GetTimerManager().SetTimer(GoIronsightsIdleTimerHandle, this, &AVer2_Bot::PlayIdleStandingIronsightsMontage, GrenadeLauncherEquipTime + 0.1f, false);

			FTimerHandle EquipAnimFinishedTimerHandle;
			World->GetTimerManager().SetTimer(EquipAnimFinishedTimerHandle, this, &AVer2_Bot::FireSalvo, GrenadeLauncherEquipTime + 0.3f, false);

		}

	}
	else
	{
		// need to reload
		ReloadLauncher();

	}
	

}

void AVer2_Bot::MakeSmokeInvisible()
{
	if (LauncherMuzzleSmoke && LauncherMuzzleLocation)
	{	
		LauncherMuzzleSmoke->SetActive(false);
		LauncherMuzzleSmoke->SetVisibility(false);
	}

}

void AVer2_Bot::PlayIdleStandingIronsightsMontage()
{
	PlayMontage(IdleIronsightsMontage);

}

void AVer2_Bot::FireHittingShot(FVector PlayerLocation)
{
	UWorld* World = GetWorld();

	if (World)
	{

		World->SpawnActor<AVer2_Projectile>(BP_RifleHit, RifleMuzzleLocation->GetComponentLocation(), GetController()->GetControlRotation());

		if (RifleWeaponMesh && RifleMuzzleflash)
		{
			UGameplayStatics::SpawnEmitterAttached(RifleMuzzleflash->Template, RifleWeaponMesh, FName("MuzzleFlashSocket"), FVector(20.f, 0.f, 0.f), RifleMuzzleFlashRotationOffset);

			// Play sound
			UMyStatics::PlaySound(this, RifleFireSounds, RifleMuzzleLocation->GetComponentLocation(), 0.45f);

		}

	}
	
}

void AVer2_Bot::FireMissingShot(FVector PlayerLocation)
{
	UWorld* World = GetWorld();

	if (World)
	{
		
		World->SpawnActor<AVer2_Projectile>(BP_RifleMiss, RifleMuzzleLocation->GetComponentLocation(), GetController()->GetControlRotation());
		
		if (RifleWeaponMesh && RifleMuzzleflash)
		{
			UGameplayStatics::SpawnEmitterAttached(RifleMuzzleflash->Template, RifleWeaponMesh, FName("MuzzleFlashSocket"), FVector(20.f, 0.f, 0.f), RifleMuzzleFlashRotationOffset);
		
			// Play sound
			UMyStatics::PlaySound(this, RifleFireSounds, RifleMuzzleLocation->GetComponentLocation(), 0.45f);

		}

	}
	
}
 
void AVer2_Bot::Reload()
{
	
	AVer2_Bot::DebugMsg(FString("Function changed: Reload()"), FColor::Red);
	
	bIsFiring = false;

	bIsReloading = true;

	if (bIsCrouching)
	{
		PlayMontage(ReloadRifleMontage);
	}
	else
	{
		PlayMontage(ReloadRifleIronsightsMontage);
	}

	FTimerHandle DummyTimerHandle;

	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::OnReloadFinish, ReloadTime, false);

	}
	
}

void AVer2_Bot::OnReloadFinish()
{
	AVer2_Bot::DebugMsg(FString("Function change: OnReloadFinish()"), FColor::Red);
	
	bIsReloading = false;

	ClipAmmo = MaxClipAmmo;

	if (NumShots > 0)
	{
		// Continue with salvo after reload
		Fire();

	}
	else if (bWasCrouched)
	{
		CrouchInCover();

		DecideNextAction();

	}
	else
	{
		DecideNextAction();
	}

}

void AVer2_Bot::EquipWeapon()
{
	AVer2_Bot::DebugMsg(FString("Function change: EquipWeapon()"), FColor::Red);

	switch (EquippedWeapon)
	{
	case EWeaponEnum::Launcher:
		if (RifleWeaponMesh && LauncherMesh)
		{
			LauncherMesh->SetVisibility(false);
			LauncherMesh->SetHiddenInGame(true);

			RifleWeaponMesh->SetVisibility(true);
			RifleWeaponMesh->SetHiddenInGame(false);

			EquippedWeapon = EWeaponEnum::Rifle;

		}
		break;

	case EWeaponEnum::Rifle:
		if (RifleWeaponMesh && LauncherMesh)
		{
			RifleWeaponMesh->SetVisibility(false);
			RifleWeaponMesh->SetHiddenInGame(true);

			LauncherMesh->SetVisibility(true);
			LauncherMesh->SetHiddenInGame(false);

			EquippedWeapon = EWeaponEnum::Launcher;

		}
		break;
	
	default:
		break;
	}

}

void AVer2_Bot::ResetRecentDamage()
{
	RecentDamage = 0.0f;

}

void AVer2_Bot::PlayMontage(UAnimMontage * Montage)
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

void AVer2_Bot::PlayZeroHealthMontage()
{
	int32 Index = FMath::RandRange(1, 4);

	float MontageDuration = 0.f;

	switch (Index)
	{
	case 1:
		PlayMontage(ZeroHealthMontage1);
		MontageDuration = ZeroHealthMontage1->GetPlayLength();
		break;

	case 2: 
		PlayMontage(ZeroHealthMontage2);
		MontageDuration = ZeroHealthMontage2->GetPlayLength();
		break;

	case 3:
		PlayMontage(ZeroHealthMontage3);
		MontageDuration = ZeroHealthMontage3->GetPlayLength();
		break;

	case 4:
		PlayMontage(ZeroHealthMontage4);
		MontageDuration = ZeroHealthMontage4->GetPlayLength();
		break;

	default:
		break;
		
	}

	MontageDuration *= 0.85f;

	UWorld* World = GetWorld();
	if (World)
	{
		FTimerHandle DummyTimerHandle;
		World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::PauseAnimation, MontageDuration, false);

	}

}

void AVer2_Bot::PauseAnimation()
{
	USkeletalMeshComponent* BotMesh = GetMesh();
	if (BotMesh)
	{
		BotMesh->bNoSkeletonUpdate = true;
	}

}

void AVer2_Bot::ChangeMoveSpeed(float NewSpeed)
{
	// Stop Crouch to stand montage playing if it is
	USkeletalMeshComponent* BotMesh = GetMesh();
	if (BotMesh)
	{
		
		UAnimInstance* AnimInst = BotMesh->GetAnimInstance();
		if (AnimInst)
		{
			//if (AnimInst->IsPlayingSlotAnimation(CrouchToStandMontage, FName("DefaultGroup.FullBody")) || AnimInst->IsPlayingSlotAnimation(CrouchToStandHipMontage, FName("DefaultGroup.FullBody"))
			//	|| AnimInst->IsPlayingSlotAnimation(StandToCrouchMontage, FName("DefaultGroup.FullBody")) || NewSpeed == RunSpeed)
			//{
			//	AnimInst->Montage_Stop(0.2f);
			//}
			
			UAnimMontage* CurrentMontage = AnimInst->GetCurrentActiveMontage();
			if (CurrentMontage && (CurrentMontage == CrouchToStandMontage || CurrentMontage == StandToCrouchMontage || CurrentMontage == CrouchToStandHipMontage || NewSpeed == RunSpeed))
			{
				AnimInst->Montage_Stop(0.2f);
			}
			
			
		}

	}
	
	UCharacterMovementComponent* CharMoveComp = GetCharacterMovement();
	if (CharMoveComp)
	{
		CharMoveComp->MaxWalkSpeed = NewSpeed;
	}

}

void AVer2_Bot::StopMontages()
{
	USkeletalMeshComponent* CharMesh = GetMesh();	// Previously was named Mesh
	if (CharMesh)
	{
		UAnimInstance* AnimInst = CharMesh->AnimScriptInstance;
		if (AnimInst)
		{
			UAnimMontage* CurrentMontage = AnimInst->GetCurrentActiveMontage();
			if (CurrentMontage)
			{
				AnimInst->Montage_Stop(CurrentMontage->BlendOut.GetBlendTime());
			}
			
		}
		
	}
	
}

bool AVer2_Bot::FadeMouseoverDisplay(float DeltaTime)
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

void AVer2_Bot::UpdateMouseoverDisplayPosition()
{

	if (MouseoverInfoBox)
	{
		MouseoverInfoBox->SetWorldLocation(GetActorLocation() + MouseoverBoxOffset);
	}

}

void AVer2_Bot::MakeMouseoverDisplayInvisible()
{
	// This actually does nothing to the mouseover display, but it does make the overhead icon invisible

	if (OverheadIconUserWidget)
	{
		OverheadIconUserWidget->SetVisibility(ESlateVisibility::Hidden);
	}

}

void AVer2_Bot::RemoveFromMouseoverArrays()
{
	// Remove from players mouseover arrays
	int32 Index = Player->MouseoverBots.Find(this);
	Player->MouseoverBots.RemoveAt(Index);
	Player->MouseoverBotsMouseoverTime.RemoveAt(Index);
	Player->MouseoverBotsFadeTime.RemoveAt(Index);

}

void AVer2_Bot::ShowExclamationPointIcon()
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
			World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::FadeOutOverheadExclamationPoint, 1.0f, false);

		}

	}

}

void AVer2_Bot::ShowQuestionMarkIcon()
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
			World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::FadeOutOverheadQuestionMark, 3.f, false);

		}

	}

}

void AVer2_Bot::FadeOutOverheadExclamationPoint()
{
	// Create a animation in editor, make sure widgets parent class is the widget class I created
	// and play that animation. 

	if (OverheadIconUserWidget && OverheadIconUserWidget->Animation)
	{
		OverheadIconUserWidget->PlayAnimation(OverheadIconUserWidget->Animation, 0.f, 1);
	}

}

void AVer2_Bot::FadeOutOverheadQuestionMark()
{
	// Create a animation in editor, make sure widgets parent class is the widget class I created
	// and play that animation.
	
	if (OverheadIconUserWidget && OverheadIconUserWidget->Animation2)
	{
		OverheadIconUserWidget->PlayAnimation(OverheadIconUserWidget->Animation2, 0.f, 1);
	}

}

void AVer2_Bot::UpdateOverheadIconLocation()
{
	if (OverheadIconWidget)
	{
		OverheadIconWidget->SetWorldLocation(GetActorLocation() + OverheadIconOffset);
	}
}

void AVer2_Bot::FaceRotation(FRotator NewRotation, float DeltaTime)
{
	if (bHasZeroHealth)
	{
		return;
	}
	
	FRotator CurrentRotation = FMath::RInterpTo(GetActorRotation(), NewRotation, DeltaTime, 8.0f);

	Super::FaceRotation(CurrentRotation, DeltaTime);

}

bool AVer2_Bot::ReceiveDamage(float Amount)
{
	Health -= Amount;

	if (!bHasZeroHealth && Health <= 0.f)
	{
		// Zero health
		bHasZeroHealth = true;

		if (BotController)
		{
			BotController->StopMovement();

			UWorld* World = GetWorld();
			if (World)
			{
				World->GetTimerManager().ClearAllTimersForObject(this);
				
				// Hide launcher muzzle smoke if it is showing
				MakeSmokeInvisible();

				// Make mouseover display invisible
				MakeMouseoverDisplayInvisible();

				PlayZeroHealthMontage();
				
				UCapsuleComponent* Capsule = GetCapsuleComponent();
				if (Capsule)
				{
					Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				}

				USkeletalMeshComponent* CharacterMesh = GetMesh();
				if (CharacterMesh)
				{
					CharacterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				}

				// For bunker event
				if (Player)
				{
					Player->IncrementNumBotsDefeated();

					FTimerHandle DummyTimerHandle;
					World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AVer2_Bot::RemoveFromMouseoverArrays, 3.f, false);


					

				}

				return false;
			}
		}

	}
	
	RecentDamage += Amount;

	if (RecentDamage / MaxHealth > 0.5f)
	{
		// Lost 50% health recently
		bIsLoosingHealthFast = true;
	}

	UWorld* World = GetWorld();
	if (World)
	{
		if (World->GetTimerManager().IsTimerActive(RecentDamageTimerHandle))
		{
			World->GetTimerManager().ClearTimer(RecentDamageTimerHandle);
		}

		World->GetTimerManager().SetTimer(RecentDamageTimerHandle, this, &AVer2_Bot::ResetRecentDamage, RecentDamageTimeInterval, false);

		// Make alert if not already
		if (!bIsAlerted)
		{
			bIsAlerted = true;

			if (!World->GetTimerManager().IsTimerActive(MakeAlertAfterDamageTimerHandle))
			{
				// Play take damage montage here. For this to work there needs to be a slot in the animation blueprint.
				// This is currently disabled.
				if (GetMesh() && TakeDamageAnimation1 && TakeDamageAnimation2 && TakeDamageAnimation3 && TakeDamageAnimation4)
				{
					
					int32 RandomMontage = FMath::RandRange(1, 4);
					
					switch (RandomMontage)
					{
					case 1:
						TakeDamageMontage = TakeDamageAnimation1;
						break;

					case 2:
						TakeDamageMontage = TakeDamageAnimation2;
						break;

					case 3:
						TakeDamageMontage = TakeDamageAnimation3;
						break;
					
					case 4:
						TakeDamageMontage = TakeDamageAnimation4;
						break;

					default:
						break;
					}
					
					if (TakeDamageMontage)
					{
						GetMesh()->AnimScriptInstance->Montage_Play(TakeDamageMontage, 1.f);
						
						FTimerHandle StopMontageTimerHandle;
						World->GetTimerManager().SetTimer(StopMontageTimerHandle, this, &AVer2_Bot::StopMontages, 0.967f, false);


					}
					
					if (BotController)
					{
						BotController->StopMovement();
						BotController->SetFocalPoint(GetActorLocation() + GetActorForwardVector());

					}

				}
				
				World->GetTimerManager().SetTimer(MakeAlertAfterDamageTimerHandle, this, &AVer2_Bot::EnterAlertPhase, 1.0f, false);
 
				// Alert nearby bots too
				if (!World->GetTimerManager().IsTimerActive(AlertNearbyBotsAfterDamageTimerHandle))
				{
					World->GetTimerManager().SetTimer(AlertNearbyBotsAfterDamageTimerHandle, this, &AVer2_Bot::AlertNearbyBots, 1.25f, false);

				}

			}
			
		}

	}
	
	return false;

}

