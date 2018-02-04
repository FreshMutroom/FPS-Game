// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPS_Project.h"
#include "FPS_ProjectCharacter.h"
#include "FPS_ProjectProjectile.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "Projectile.h"
#include "CameraRobot.h"
#include "MyStatics.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "Classes/Kismet/KismetMathLibrary.h"
#include "Animation/WidgetAnimation.h"
#include "Ver2_MouseoverDisplay.h"
#include "MySaveGame.h"
#include "Runtime/LevelSequence/Public/LevelSequencePlayer.h"
#include "SniperCutsceneProjectile.h"
#include "FPS_ProjectGameMode.h"
#include "MyGameInstance.h"
#include "TutorialMessage.h"
#include "MyPlayerController.h"


DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AFPS_ProjectCharacter

AFPS_ProjectCharacter::AFPS_ProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 1.f;
	BaseLookUpRate = 1.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);	// original values: (1.9f, -19.19f, 5.2f)
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);	// original values: (-0.5f, -4.4f, -155.7f)

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;	// Added by me
	//FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));	// This was commented originally
	FP_Gun->SetupAttachment(RootComponent);
	
	/* Create meshes for the other 3 weapons*/
	FP_GunAutoRifle = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_GunAutoRifle"));
	FP_GunAutoRifle->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_GunAutoRifle->bCastDynamicShadow = false;
	FP_GunAutoRifle->CastShadow = false;
	FP_GunAutoRifle->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;	// Added by me
	//FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));	// This was commented originally
	FP_GunAutoRifle->SetupAttachment(RootComponent);
	
	FP_GunGrenadeLauncher = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_GunGrenadeLauncher"));
	FP_GunGrenadeLauncher->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_GunGrenadeLauncher->bCastDynamicShadow = false;
	FP_GunGrenadeLauncher->CastShadow = false;
	FP_GunGrenadeLauncher->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;	// Added by me
	//FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));	// This was commented originally
	FP_GunGrenadeLauncher->SetupAttachment(RootComponent);

	FP_GunSniperRifle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FP_GunSniperRifle"));
	FP_GunSniperRifle->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_GunSniperRifle->bCastDynamicShadow = false;
	FP_GunSniperRifle->CastShadow = false;
	//FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));	// This was commented originally
	FP_GunSniperRifle->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));	// default values: (0.2f, 48.4f, -10.6f)

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 8.0f, -2.0f);	// original values: (100.0f, 0.0f, 10.0f), my new values: (100.0f, 8.0f, -2.0f)

	bCanFire = true;

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->Hand = EControllerHand::Right;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;

	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Third person character mesh"));
	Mesh3P->SetVisibility(false);
	Mesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh3P->AttachTo(RootComponent);
	
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Overhead display"));
	OverheadWidget->SetupAttachment(GetCapsuleComponent());

	MaxHealth = 100.f;
	CurrentHealth = MaxHealth;
	
	InitialStamina = 100.0f;
	CurrentStamina = InitialStamina;
	StaminaRechargeDelay = 1.0f;
	StaminaRechargeRate = 0.2f;
	StaminaDepletionRate = 0.12f;
	bIsSprinting = false;
	
	CrouchMoveSpeed = 300.f;
	bIsCrouching = false;

	// A value of 255 means they do not have another weapon
	OtherWeapon = 255;

	WeaponSwitchCooldown = 0.72f;
	WeaponSwitchFiringTime = 0.58f;
	bCanFireAfterWeaponSwitch = true;

	HealthRechargeDelay = 5.0f;
	HealthRechargeRate = 20.0f;
	RecentDamage = 0.0f;
	RecentDamageTime = 1.0f;
	RecentDamageFadeTime = 0.7f;

	TypewriterDefaultDelay = 0.05f;
	TypewriterLineLength = 42;

	NumBotsDefeatedThreshold = 15;

}

void AFPS_ProjectCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UWorld* World = GetWorld();
	if (World)
	{
		GlobalTimeDilation = UGameplayStatics::GetGlobalTimeDilation(World);
	}

	// To be fixed: Holding shift, pressing forward then right/left, then releasing forward will let the 
	// character sprint sideways.
	HandleSprintForTick(DeltaTime);

	HandleHealthForTick(DeltaTime);

	// Still alternates between 1st and 2nd stage
	UpdateAimAnimStatus(DeltaTime);

	UpdateHUDInfoOnTick(DeltaTime);

	HandleCrouchAnimOnTick(DeltaTime);

	RaycastForMouseoverDisplaysOnTick(DeltaTime);

	RotateOverheadArrowOnTick(DeltaTime);

	UpdateRecoilAnimationOnTick(DeltaTime);

	HandleControlsMenuVisibilityOnTick(DeltaTime);

	CheckIfFallenOutOfWorldOnTick(DeltaTime);

	//if (GEngine)
	//{
	//	FVector Loc = GetActorLocation();
	//	GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Purple, FString::Printf(TEXT("Player location: ( %f , %f , %f )"), Loc.X, Loc.Y, Loc.Z));
	//}

}

void AFPS_ProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	AttachWeaponMeshesForBeginPlay();

	SetupWeaponValuesForBeginPlay();

	AdjustForMotionControls();

	// Input values for weapon aim animations
	SetupWeaponAimParameters();

	SetupCrouchAnimForBeginPlay();

	HideOverheadIconOnBeginPlay();

	SetupLowHealthAnimationOnBeginPlay();

	SetupRecoilForBeginPlay();

	ClampPitchLookAngleOnBeginPlay();

	SetupGameInstanceForBeginPlay();

	SetupTABControlsMenuOnBeginPlay();

	MakeMovementSpeedCorrectForBeginPlay();

	ChangeMouseSensitivity();

}

void AFPS_ProjectCharacter::SetupGameInstanceForBeginPlay()
{
	UGameInstance * GameInst = UGameplayStatics::GetGameInstance(this);
	if (GameInst)
	{
		GameInstance = Cast<UMyGameInstance>(GameInst);
	}

}

void AFPS_ProjectCharacter::PauseGame()
{
	
	if (GameInstance)
	{
		GameInstance->OnGamePaused();
		
	}

}

void AFPS_ProjectCharacter::ClampPitchLookAngleOnBeginPlay()
{
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(World, 0);
		if (CameraManager)
		{
			CameraManager->ViewPitchMax = 70.0f;
			CameraManager->ViewPitchMin = -70.0f;
		}

	}

}

//////////////////////////////////////////////////////////////////////////
// Input

void AFPS_ProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPS_ProjectCharacter::CheckJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Pickup", IE_Pressed, this, &AFPS_ProjectCharacter::BeginPickup);
	PlayerInputComponent->BindAction("Pickup", IE_Released, this, &AFPS_ProjectCharacter::EndPickup);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AFPS_ProjectCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AFPS_ProjectCharacter::StopSprinting);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AFPS_ProjectCharacter::Reload);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AFPS_ProjectCharacter::RMBPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AFPS_ProjectCharacter::RMBReleased);

	PlayerInputComponent->BindAction("SwitchWeapons", IE_Pressed, this, &AFPS_ProjectCharacter::SwitchWeapons);

	// These are for checking if allowed to sprint
	PlayerInputComponent->BindAction("W_Released", IE_Released, this, &AFPS_ProjectCharacter::W_Released);
	PlayerInputComponent->BindAction("A_Released", IE_Released, this, &AFPS_ProjectCharacter::A_Released);
	PlayerInputComponent->BindAction("D_Released", IE_Released, this, &AFPS_ProjectCharacter::D_Released);

	PlayerInputComponent->BindAction("LCTRLPressed", IE_Pressed, this, &AFPS_ProjectCharacter::LCTRLPressed);
	PlayerInputComponent->BindAction("LCTRLPressed", IE_Released, this, &AFPS_ProjectCharacter::LCTRLReleased);

	PlayerInputComponent->BindAction("CameraRobot", IE_Pressed, this, &AFPS_ProjectCharacter::ZeroKeyPressed);

	FInputActionBinding Input = PlayerInputComponent->BindAction("PauseGame", IE_Pressed, this, &AFPS_ProjectCharacter::PauseGame);
	Input.bConsumeInput = false;

	PlayerInputComponent->BindAction("Load", IE_Pressed, this, &AFPS_ProjectCharacter::LPressed);

	PlayerInputComponent->BindAction("ControlsMenu", IE_Pressed, this, &AFPS_ProjectCharacter::OnTABPressed);
	PlayerInputComponent->BindAction("ControlsMenu", IE_Released, this, &AFPS_ProjectCharacter::OnTABReleased);

	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AFPS_ProjectCharacter::TouchStarted);
	if (EnableTouchscreenMovement(PlayerInputComponent) == false)
	{
		PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPS_ProjectCharacter::OnFire);
		PlayerInputComponent->BindAction("Fire", IE_Released, this, &AFPS_ProjectCharacter::ReleasedTrigger);
	}

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AFPS_ProjectCharacter::OnResetVR);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFPS_ProjectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPS_ProjectCharacter::MoveRight);

	/** For moving camera robot */
	PlayerInputComponent->BindAxis("MoveForwardWhileUsingCameraRobot", this, &AFPS_ProjectCharacter::MoveForwardWhileUsingCameraRobot);
	PlayerInputComponent->BindAxis("MoveRightWhileUsingCameraRobot", this, &AFPS_ProjectCharacter::MoveRightWhileUsingCameraRobot);
	PlayerInputComponent->BindAxis("TurnCameraRobot", this, &AFPS_ProjectCharacter::TurnCameraRobot);
	PlayerInputComponent->BindAxis("LookUpCameraRobot", this, &AFPS_ProjectCharacter::LookUpCameraRobot);


	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &AFPS_ProjectCharacter::CheckBeforeAddingYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFPS_ProjectCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AFPS_ProjectCharacter::CheckBeforeAddingPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFPS_ProjectCharacter::LookUpAtRate);
}

float AFPS_ProjectCharacter::GetInitialStamina()
{
	return InitialStamina;

}

float AFPS_ProjectCharacter::GetCurrentStamina()
{
	return CurrentStamina;
}

void AFPS_ProjectCharacter::UpdateCurrentStamina(float Stamina)
{
	CurrentStamina = CurrentStamina + Stamina;

	if (CurrentStamina > InitialStamina)
	{
		CurrentStamina = InitialStamina;
	}
	else if (CurrentStamina < 0.0f)
	{
		CurrentStamina = 0.0f;
	}

}

void AFPS_ProjectCharacter::Sprint()
{
	if (!bIsUsingCameraRobot)
	{
		bWantsToSprint = true;
		bStaminaRechargingFromZero = false;
	}
	else
	{
		// Make camera robot move faster
		if (CameraRobot)
		{
			UCharacterMovementComponent* RobotMoveComp = CameraRobot->GetCharacterMovement();
			if (RobotMoveComp)
			{
				RobotMoveComp->MaxFlySpeed = CameraRobot->BoostedFlySpeed;
			}
		}
	}

}

void AFPS_ProjectCharacter::StopSprinting()
{
	if (!bIsUsingCameraRobot)
	{
		bWantsToSprint = false;

		if (!bStaminaRechargeDelayTimerHandleIsRunning && !bCanStaminaRecharge)
		{
			bStaminaRechargeDelayTimerHandleIsRunning = true;
			GetWorld()->GetTimerManager().SetTimer(StaminaRechargeDelayTimerHandle, this, &AFPS_ProjectCharacter::AllowStaminaToRecharge, 1.0f, false);
		}

		// Aim if right mouse button is pressed
		if (bWantsToAim)
		{
			RMBPressed();
		}
	}
	else
	{
		if (CameraRobot)
		{
			UCharacterMovementComponent* RobotMoveComp = CameraRobot->GetCharacterMovement();
			if (RobotMoveComp)
			{
				RobotMoveComp->MaxFlySpeed = CameraRobot->FlySpeed;
			}
		}
	}
	
}

void AFPS_ProjectCharacter::W_Released()
{
	bWantsToMoveForward = false;

}

void AFPS_ProjectCharacter::A_Released()
{
	bWantsToMoveLeft = false;

}

void AFPS_ProjectCharacter::D_Released()
{
	bWantsToMoveRight = false;

}

void AFPS_ProjectCharacter::AllowStaminaToRecharge()
{
	if (CurrentStamina <= 0.f)
	{
		// This means if you use all your stamina and are still holding shift you will not sprint again 
		// until you release it
		bStaminaRechargingFromZero = true;
	}
	
	GetWorldTimerManager().ClearTimer(StaminaRechargeDelayTimerHandle);
	bCanStaminaRecharge = true;
	bStaminaRechargeDelayTimerHandleIsRunning = false;

}

void AFPS_ProjectCharacter::HealthCanRecharge()
{
	bIsHealthRecharging = true;

}

void AFPS_ProjectCharacter::StopFireAnimation()
{
	if (FireAnimation)
	{
		Mesh1P->AnimScriptInstance->Montage_Stop(FireAnimation->BlendOut.GetBlendTime(), FireAnimation);
	}

}

void AFPS_ProjectCharacter::SetupWeaponAimParameters()
{
	int Index;
	
	// This stuff should be assigned on weapon change. Actually I think here in beginplay is good
	//FOnTimelineFloat ProgressFunction;

	ProgressFunction.BindUFunction(this, FName("HandleAimAnimationProgress"));
	
	ProgressFunctionPart2.BindUFunction(this, FName("HandleAimAnimationProgressPart2"));

	// These here in begin play may not be the right place
	LaserRifleTimelineRotation.AddInterpFloat(LaserRifleAnimCurveRotation, ProgressFunction);
	LaserRifleTimelineRotation.SetLooping(false);
	
	LaserRifleTimelineLocation.AddInterpFloat(LaserRifleAnimCurveLocation, ProgressFunction);
	LaserRifleTimelineLocation.SetLooping(false);

	LaserRifleTimelineRotationPart2.AddInterpFloat(LaserRifleAnimCurveRotationPart2, ProgressFunctionPart2);
	LaserRifleTimelineRotationPart2.SetLooping(false);
							  
	LaserRifleTimelineLocationPart2.AddInterpFloat(LaserRifleAnimCurveLocationPart2, ProgressFunctionPart2);
	LaserRifleTimelineLocationPart2.SetLooping(false);

	AutoRifleTimelineRotation.AddInterpFloat(AutoRifleAnimCurveRotation, ProgressFunction);
	AutoRifleTimelineRotation.SetLooping(false);
	
	AutoRifleTimelineLocation.AddInterpFloat(AutoRifleAnimCurveLocation, ProgressFunction);
	AutoRifleTimelineLocation.SetLooping(false);
	
	AutoRifleTimelineRotationPart2.AddInterpFloat(AutoRifleAnimCurveRotationPart2, ProgressFunctionPart2);
	AutoRifleTimelineRotationPart2.SetLooping(false);
							 
	AutoRifleTimelineLocationPart2.AddInterpFloat(AutoRifleAnimCurveLocationPart2, ProgressFunctionPart2);
	AutoRifleTimelineLocationPart2.SetLooping(false);

	GrenadeLauncherTimelineRotation.AddInterpFloat(GrenadeLauncherAnimCurveRotation, ProgressFunction);
	GrenadeLauncherTimelineRotation.SetLooping(false);
	
	GrenadeLauncherTimelineLocation.AddInterpFloat(GrenadeLauncherAnimCurveLocation, ProgressFunction);
	GrenadeLauncherTimelineLocation.SetLooping(false);

	GrenadeLauncherTimelineRotationPart2.AddInterpFloat(GrenadeLauncherAnimCurveRotationPart2, ProgressFunctionPart2);
	GrenadeLauncherTimelineRotationPart2.SetLooping(false);
								   
	GrenadeLauncherTimelineLocationPart2.AddInterpFloat(GrenadeLauncherAnimCurveLocationPart2, ProgressFunctionPart2);
	GrenadeLauncherTimelineLocationPart2.SetLooping(false);

	SniperRifleTimelineRotation.AddInterpFloat(SniperRifleAnimCurveRotation, ProgressFunction);
	SniperRifleTimelineRotation.SetLooping(false);
	
	SniperRifleTimelineLocation.AddInterpFloat(SniperRifleAnimCurveLocation, ProgressFunction);
	SniperRifleTimelineLocation.SetLooping(false);

	SniperRifleTimelineRotationPart2.AddInterpFloat(SniperRifleAnimCurveRotationPart2, ProgressFunctionPart2);
	SniperRifleTimelineRotationPart2.SetLooping(false);
							   
	SniperRifleTimelineLocationPart2.AddInterpFloat(SniperRifleAnimCurveLocationPart2, ProgressFunctionPart2);
	SniperRifleTimelineLocationPart2.SetLooping(false);
	

	// Setup laser rifle animations
	if (LaserRifleAnimCurveRotation && LaserRifleAnimCurveLocation)
	{
		Index = 0;

		// These values are only "right" for the laser rifle
		WeaponAimRotInitial.Insert(Mesh1P->RelativeRotation, Index);
		WeaponAimLocInitial.Insert(Mesh1P->RelativeLocation, Index);
		WeaponAimRotTarget.Insert(WeaponAimRotInitial[Index] + FRotator(0.f, 0.f, 0.f), Index);
		WeaponAimLocTarget.Insert(WeaponAimLocInitial[Index] + FVector(-9.5f, -6.7f, 4.f), Index);
		WeaponAimRotTargetPart2.Insert(WeaponAimRotTarget[Index] + FRotator(0.f, 0.f, -6.f), Index);
		WeaponAimLocTargetPart2.Insert(WeaponAimLocTarget[Index] + FVector(3.f, 0.f, 8.f), Index);	// 3: down/up

		WeaponLocTimelines.Insert			(LaserRifleTimelineLocation, Index);
		WeaponRotTimelines.Insert			(LaserRifleTimelineRotation, Index);
		WeaponLocTimelinesPart2.Insert		(LaserRifleTimelineLocationPart2, Index);
		WeaponRotTimelinesPart2.Insert		(LaserRifleTimelineRotationPart2, Index);

		RotationAnimInstances.Insert		(LaserRifleAnimCurveRotation, Index);
		LocationAnimInstances.Insert		(LaserRifleAnimCurveLocation, Index);
		RotationAnimInstancesPart2.Insert	(LaserRifleAnimCurveRotationPart2, Index);
		LocationAnimInstancesPart2.Insert	(LaserRifleAnimCurveLocationPart2, Index);

	}

	// Setup auto rifle animations
	if (AutoRifleAnimCurveRotation && AutoRifleAnimCurveLocation)
	{
		Index = 1;
		
		// These values are only "right" for the laser rifle
		WeaponAimRotInitial.Insert(Mesh1P->RelativeRotation, Index);
		WeaponAimLocInitial.Insert(Mesh1P->RelativeLocation, Index);
		WeaponAimRotTarget.Insert(WeaponAimRotInitial[Index] + AutoRifleAimRotationPart1, Index);
		WeaponAimLocTarget.Insert(WeaponAimLocInitial[Index] + AutoRifleAimLocationPart1, Index);
		WeaponAimRotTargetPart2.Insert(WeaponAimRotTarget[Index] + AutoRifleAimRotationPart2, Index);
		WeaponAimLocTargetPart2.Insert(WeaponAimLocTarget[Index] + AutoRifleAimLocationPart2, Index);	// 3: down/up

		WeaponLocTimelines.Insert		(AutoRifleTimelineLocation, 		Index);
		WeaponRotTimelines.Insert		(AutoRifleTimelineRotation,			Index);
		WeaponLocTimelinesPart2.Insert	(AutoRifleTimelineLocationPart2,	Index);
		WeaponRotTimelinesPart2.Insert	(AutoRifleTimelineRotationPart2,	Index);

		RotationAnimInstances.Insert		(AutoRifleAnimCurveRotation, Index);
		LocationAnimInstances.Insert		(AutoRifleAnimCurveLocation, Index);
		RotationAnimInstancesPart2.Insert	(AutoRifleAnimCurveRotationPart2, Index);
		LocationAnimInstancesPart2.Insert	(AutoRifleAnimCurveLocationPart2, Index);

	}

	// Setup grenade launcher animations
	if (GrenadeLauncherAnimCurveRotation && GrenadeLauncherAnimCurveLocation)
	{
		Index = 2;

		// These values are only "right" for the laser rifle
		WeaponAimRotInitial.Insert(Mesh1P->RelativeRotation, Index);
		WeaponAimLocInitial.Insert(Mesh1P->RelativeLocation, Index);
		WeaponAimRotTarget.Insert(WeaponAimRotInitial[Index] + FRotator(0.f, 0.f, 0.f), Index);
		WeaponAimLocTarget.Insert(WeaponAimLocInitial[Index] + FVector(-9.5f, -6.7f, 4.f), Index);
		WeaponAimRotTargetPart2.Insert(WeaponAimRotTarget[Index] + FRotator(0.f, 0.f, -16.f), Index);
		WeaponAimLocTargetPart2.Insert(WeaponAimLocTarget[Index] + FVector(3.f, 0.f, 18.f), Index);	// 3: down/up

		WeaponLocTimelines.Insert		(GrenadeLauncherTimelineLocation, Index);
		WeaponRotTimelines.Insert		(GrenadeLauncherTimelineRotation, Index);
		WeaponLocTimelinesPart2.Insert	(GrenadeLauncherTimelineLocationPart2, Index);
		WeaponRotTimelinesPart2.Insert	(GrenadeLauncherTimelineRotationPart2, Index);

		RotationAnimInstances.Insert		(GrenadeLauncherAnimCurveRotation, Index);
		LocationAnimInstances.Insert		(GrenadeLauncherAnimCurveLocation, Index);
		RotationAnimInstancesPart2.Insert	(GrenadeLauncherAnimCurveRotationPart2, Index);
		LocationAnimInstancesPart2.Insert	(GrenadeLauncherAnimCurveLocationPart2, Index);

	}

	// Setup sniper rifle animations
	if (SniperRifleAnimCurveRotation && SniperRifleAnimCurveLocation)
	{
		Index = 3;

		// These values are only "right" for the laser rifle
		WeaponAimRotInitial.Insert(Mesh1P->RelativeRotation, Index);
		WeaponAimLocInitial.Insert(Mesh1P->RelativeLocation, Index);
		WeaponAimRotTarget.Insert(WeaponAimRotInitial[Index] + SniperRifleAimRotationPart1, Index);
		WeaponAimLocTarget.Insert(WeaponAimLocInitial[Index] + SniperRifleAimLocationPart1, Index);
		WeaponAimRotTargetPart2.Insert(WeaponAimRotTarget[Index] + SniperRifleAimRotationPart2, Index);
		WeaponAimLocTargetPart2.Insert(WeaponAimLocTarget[Index] + SniperRifleAimLocationPart2, Index);

		WeaponLocTimelines.Insert		(SniperRifleTimelineLocation, Index);
		WeaponRotTimelines.Insert		(SniperRifleTimelineRotation, Index);
		WeaponLocTimelinesPart2.Insert	(SniperRifleTimelineLocationPart2, Index);
		WeaponRotTimelinesPart2.Insert	(SniperRifleTimelineRotationPart2, Index);	

		RotationAnimInstances.Insert		(SniperRifleAnimCurveRotation, Index);
		LocationAnimInstances.Insert		(SniperRifleAnimCurveLocation, Index);
		RotationAnimInstancesPart2.Insert	(SniperRifleAnimCurveRotationPart2, Index);
		LocationAnimInstancesPart2.Insert	(SniperRifleAnimCurveLocationPart2, Index);

	}

	// Lastly set up the current aim animations for the laser rifle which is the starting weapon
	
	uint8 WType = 0;
	
	CurrentRotAimAnim = WeaponRotTimelines[WType];
	CurrentLocAimAnim = WeaponLocTimelines[WType];
	CurrentRotAimAnimPart2 = WeaponRotTimelinesPart2[WType + 1];	// This + 1 makes the 2nd part work
	CurrentLocAimAnimPart2 = WeaponLocTimelinesPart2[WType + 1];

	CurrentInitialRot = &WeaponAimRotInitial[WType];
	CurrentInitialLoc = &WeaponAimLocInitial[WType];
	CurrentTargetRot = &WeaponAimRotTarget[WType];
	CurrentTargetLoc = &WeaponAimLocTarget[WType];
	CurrentTargetRotPart2 = &WeaponAimRotTargetPart2[WType];
	CurrentTargetLocPart2 = &WeaponAimLocTargetPart2[WType];

	bIsAimAnim1st = true;

}

void AFPS_ProjectCharacter::UpdateAimAnimStatus(float DeltaTime)
{
	if (CurrentRotAimAnim.IsPlaying() || CurrentLocAimAnim.IsPlaying())
	{
		if (bIsReloading || bIsSprinting)
		{
			// Stop both animations from playing and reset the start point to the beginning
			CurrentRotAimAnim.Stop();
			CurrentLocAimAnim.Stop();
			CurrentRotAimAnim.SetNewTime(0.f);
			CurrentLocAimAnim.SetNewTime(0.f);
			
			CurrentRotAimAnimPart2.Stop();
			CurrentLocAimAnimPart2.Stop();
			CurrentRotAimAnimPart2.SetNewTime(0.f);
			CurrentLocAimAnimPart2.SetNewTime(0.f);

			bIsAimAnim1st = true;
			
			// Reset the location of the weapon
			ResetMesh1P();

		}
		else
		{
			
			if (bIsAimAnim1st)
			{
				// Tick 1st timeline
				
				// I don't think the location line here is needed
				CurrentRotAimAnim.TickTimeline(DeltaTime * GlobalTimeDilation);
				//CurrentLocAimAnim.TickTimeline(DeltaTime * GlobalTimeDilation);
			}
			else
			{
				// Tick 2nd timeline

				CurrentRotAimAnimPart2.TickTimeline(DeltaTime * GlobalTimeDilation);
			}
		}
	}
	
}

void AFPS_ProjectCharacter::RMBPressed()
{	
	bWantsToAim = true;
	
	if (!bIsReloading && !bIsSprinting)
	{
		
		// Play 1st animation
		//float RotationTime = CurrentRotAimAnim.GetPlaybackPosition();
		//float LocationTime = CurrentLocAimAnim.GetPlaybackPosition();

		//CurrentRotAimAnim.SetNewTime(RotationTime);
		//CurrentLocAimAnim.SetNewTime(LocationTime);

		CurrentRotAimAnim.Play();
		CurrentLocAimAnim.Play();

		
		// Play 2nd animation
		//RotationTime = CurrentRotAimAnimPart2.GetPlaybackPosition();
		//LocationTime = CurrentLocAimAnimPart2.GetPlaybackPosition();

		// Commented this out. Seems to not change anything
		//CurrentRotAimAnimPart2.SetNewTime(RotationTime);
		//CurrentLocAimAnimPart2.SetNewTime(LocationTime);

		CurrentRotAimAnimPart2.Play();
		CurrentLocAimAnimPart2.Play();
		
	}

}

void AFPS_ProjectCharacter::RMBReleased()
{
	bWantsToAim = false;
	
	ToggleSniperScope(false);

	// Reverse 1st animation
	float RotationTime = CurrentRotAimAnim.GetPlaybackPosition();
	float LocationTime = CurrentLocAimAnim.GetPlaybackPosition();

	//CurrentRotAimAnim.SetNewTime(RotationTime);
	//CurrentLocAimAnim.SetNewTime(LocationTime);

	CurrentRotAimAnim.Reverse();
	CurrentLocAimAnim.Reverse();
	
	// Reverse 2nd animation
	RotationTime = CurrentRotAimAnimPart2.GetPlaybackPosition();
	LocationTime = CurrentLocAimAnimPart2.GetPlaybackPosition();

	// Commenting this got it working
	//CurrentRotAimAnimPart2.SetNewTime(RotationTime);
	//CurrentLocAimAnimPart2.SetNewTime(LocationTime);

	CurrentRotAimAnimPart2.Reverse();
	CurrentLocAimAnimPart2.Reverse();

	

}

void AFPS_ProjectCharacter::HandleAimAnimationProgress(float Value)
{
	// The parameter Value is 0.0f when at the start of the curve and close to 1.0f when at 
	// the end of the curve.

	if (!bIsAimAnim1st)
	{
		return;
	}

	FRotator NewRotation;
	FVector NewLocation;

	if (Value >= .99f && bWantsToAim && EquippedWeapon != 1)	// Do not allow auto rifle to go onto part 2
	{
		bIsAimAnim1st = false;

		return;

		// Move on to the second animation
				
		//ProgressFunction.Unbind();
		//
		//CurrentRotAimAnim->AddInterpFloat(RotationAnimInstancesPart2[EquippedWeapon], ProgressFunction);
		//CurrentRotAimAnim->SetLooping(false);
		//
		//CurrentLocAimAnim->AddInterpFloat(LocationAnimInstancesPart2[EquippedWeapon], ProgressFunction);
		//CurrentLocAimAnim->SetLooping(false);
	}
	else
	{
		// Stay on 1st animation
		NewRotation = FMath::Lerp(*CurrentInitialRot, *CurrentTargetRot, Value);
		NewLocation = FMath::Lerp(*CurrentInitialLoc, *CurrentTargetLoc, Value);

		Mesh1P->SetRelativeRotation(NewRotation);
		Mesh1P->SetRelativeLocation(NewLocation);

	}
		
	

}

void AFPS_ProjectCharacter::AssignAimAnimsOnWeaponChange(uint8 WType)
{
	// Stop the aim animation if it is playing
	CurrentRotAimAnim.Stop();
	CurrentLocAimAnim.Stop();
	CurrentRotAimAnimPart2.Stop();
	CurrentLocAimAnimPart2.Stop();
	CurrentRotAimAnim.SetNewTime(0.f);
	CurrentLocAimAnim.SetNewTime(0.f);
	CurrentRotAimAnimPart2.SetNewTime(0.f);
	CurrentLocAimAnimPart2.SetNewTime(0.f);

	// Assign new aim animation values
	CurrentRotAimAnim = WeaponRotTimelines[WType];
	CurrentLocAimAnim = WeaponLocTimelines[WType];
	CurrentRotAimAnimPart2 = WeaponRotTimelinesPart2[WType];
	CurrentLocAimAnimPart2 = WeaponLocTimelinesPart2[WType];

	CurrentInitialRot = &WeaponAimRotInitial[WType];
	CurrentInitialLoc = &WeaponAimLocInitial[WType];
	CurrentTargetRot = &WeaponAimRotTarget[WType];
	CurrentTargetLoc = &WeaponAimLocTarget[WType];
	CurrentTargetRotPart2 = &WeaponAimRotTargetPart2[WType];
	CurrentTargetLocPart2 = &WeaponAimLocTargetPart2[WType];

	bIsAimAnim1st = true;

	// Reset Mesh1P to the starting location/rotation
	ResetMesh1P();

}

void AFPS_ProjectCharacter::ResetMesh1P()
{
	Mesh1P->SetRelativeLocation(Mesh1POriginalLocation);
	Mesh1P->SetRelativeRotation(Mesh1POriginalRotation);

}

void AFPS_ProjectCharacter::HandleAimAnimationProgressPart2(float Value)
{
	if (bIsAimAnim1st)
	{
		return;
	}

	FRotator NewRotation;
	FVector NewLocation;

	if (Value <= 0.01f && !bWantsToAim)
	{

		bIsAimAnim1st = true;

		return;

		//ProgressFunction.Unbind();
		//
		//CurrentRotAimAnim->AddInterpFloat(RotationAnimInstancesPart2[EquippedWeapon], ProgressFunction);
		//CurrentRotAimAnim->SetLooping(false);
		//
		//CurrentLocAimAnim->AddInterpFloat(LocationAnimInstancesPart2[EquippedWeapon], ProgressFunction);
		//CurrentLocAimAnim->SetLooping(false);
	}
	else
	{
		if (EquippedWeapon == 3)
		{
			if (Value > 0.99f)
			{
				ToggleSniperScope(true);

			}
		
		}
		
		
		// Stay on 2nd animation
		NewRotation = FMath::Lerp(*CurrentTargetRot, *CurrentTargetRotPart2, Value);
		NewLocation = FMath::Lerp(*CurrentTargetLoc, *CurrentTargetLocPart2, Value);

		Mesh1P->SetRelativeRotation(NewRotation);
		Mesh1P->SetRelativeLocation(NewLocation);

	}

}

void AFPS_ProjectCharacter::ZeroKeyPressed()
{
	if (!bIsUsingCameraRobot)
	{
		SpawnCameraRobot();
	}
	else
	{
		DestroyCameraRobot();
	}
}

bool AFPS_ProjectCharacter::SpawnCameraRobot()
{
	if (BP_CameraRobot)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			CameraRobot = World->SpawnActor<ACameraRobot>(BP_CameraRobot, GetActorLocation() + FVector(0.f, 0.f, 500.f), GetActorRotation());

			if (CameraRobot)
			{
				// Change view to camera robot - done
				// Remove sniper overlay - done
				// Uncrouch
				// hide hud - done
				// Add arrow above player head? - done

				// Make HUD controls message
				if (CameraRobot->HUDMessage != FString(""))
				{
					CreateHUDMessage(CameraRobot->HUDMessage, false, false);
				}

				HideLowHealthWarning();

				StopSprinting();

				ToggleSniperScope(false);

				bIsUsingCameraRobot = true;

				AController* Controller = GetController();

				if (Controller)
				{
					APlayerController* PlayerControl = Cast<APlayerController>(Controller);

					if (PlayerControl)
					{
						PlayerControl->SetViewTargetWithBlend(CameraRobot, 2.f);

						FTimerHandle DummyTimerHandle;
						World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AFPS_ProjectCharacter::MakeMesh3PVisible, 0.8f, false);
				
						FTimerHandle MakeArrowVisibleTimerHandle;
						World->GetTimerManager().SetTimer(MakeArrowVisibleTimerHandle, this, &AFPS_ProjectCharacter::MakeOverheadArrowVisible, 1.95f, false);

					}

					return true;

				}
				
			}

		}

	}
	
	return false;
}

void AFPS_ProjectCharacter::MakeMesh3PVisible()
{
	if (Mesh3P)
	{
		Mesh3P->SetVisibility(true);

	}
	if (Mesh1P)
	{
		Mesh1P->SetVisibility(false);
	}

	bCanDestroyCameraRobot = true;

}

void AFPS_ProjectCharacter::MakeOverheadArrowVisible()
{
	if (OverheadWidget && bIsUsingCameraRobot)
	{
		UUserWidget* UserWidget = OverheadWidget->GetUserWidgetObject();
		if (UserWidget)
		{
			UWidget* Widget = UserWidget->GetRootWidget();
			if (Widget)
			{
				Widget->SetVisibility(ESlateVisibility::Visible);
			}
		}

	}

}

bool AFPS_ProjectCharacter::DestroyCameraRobot()
{
	if (bCanDestroyCameraRobot)
	{
		if (CameraRobot)
		{
			AController* Controller = GetController();

			if (Controller)
			{
				APlayerController* PlayerControl = Cast<APlayerController>(Controller);

				if (PlayerControl)
				{
					if (Mesh3P)
					{
						Mesh3P->SetVisibility(false);

					}
					if (Mesh1P)
					{
						Mesh1P->SetVisibility(true);
					}
					
					PlayerControl->SetViewTarget(this);

					if (CameraRobot->CameraOverlay)
					{
						CameraRobot->CameraOverlay->RemoveFromViewport();
						CameraRobot->CameraOverlay = nullptr;

					}

					MakeOverheadArrowInvisible();

					bCanHUDTextBeOverwritten = true;
					ClearHUDText();

					bCanDestroyCameraRobot = false;
					bIsUsingCameraRobot = false;
					
					if (CurrentHealth / MaxHealth < LowHealthPercentage)
					{
						ShowLowHealthWarning();
					}
					
					CameraRobot->Destroy();

					return true;

				}

			}
			
		}

	}
	
	return false;

}

void AFPS_ProjectCharacter::MakeOverheadArrowInvisible()
{
	if (OverheadWidget)
	{
		UUserWidget* UserWidget = OverheadWidget->GetUserWidgetObject();
		if (UserWidget)
		{
			UWidget* Widget = UserWidget->GetRootWidget();
			if (Widget)
			{
				Widget->SetVisibility(ESlateVisibility::Hidden);
			}
		}

	}

}

void AFPS_ProjectCharacter::RotateOverheadArrowOnTick(float DeltaTime)
{
	if (bIsUsingCameraRobot && CameraRobot && OverheadWidget)
	{
		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CameraRobot->GetActorLocation());
		FRotator NewRotation = FRotator(0.f, LookAtRot.Yaw, 0.f);

		OverheadWidget->SetWorldRotation(NewRotation);

		// Also set location
		OverheadWidget->SetWorldLocation(GetActorLocation() + OverheadIconOffset);

	}

}

void AFPS_ProjectCharacter::HideOverheadIconOnBeginPlay()
{
	if (OverheadWidget)
	{
		UUserWidget* UserWidget = OverheadWidget->GetUserWidgetObject();
		if (UserWidget)
		{
			UWidget* Widget = UserWidget->GetRootWidget();
			if (Widget)
			{
				Widget->SetVisibility(ESlateVisibility::Hidden);
			}
		}

	}

}

void AFPS_ProjectCharacter::MoveCameraRobotForward(float Value)
{
	
	if (bIsUsingCameraRobot && Value != 0.0f)
	{
		// Check to stay within Z bounds done in robots tick, not here

		if (CameraRobot && CameraRobot->RobotController)
		{
			CameraRobot->AddMovementInput(CameraRobot->RobotController->GetActorForwardVector(), Value * GlobalTimeDilation);

		}

		if (!PlayerController)
		{
			PlayerController = Cast<APlayerController>(GetController());
		}

		if (PlayerController)
		{
			PlayerController->SetViewTarget(CameraRobot);
		}

	}

}

void AFPS_ProjectCharacter::MoveCameraRobotRight(float Value)
{
	
	if (bIsUsingCameraRobot && Value != 0.0f)
	{

		if (CameraRobot && CameraRobot->RobotController)
		{
			CameraRobot->AddMovementInput(CameraRobot->RobotController->GetActorRightVector(), Value * GlobalTimeDilation);

		}
		
		if (!PlayerController)
		{
			PlayerController = Cast<APlayerController>(GetController());
		}

		if (PlayerController)
		{
			PlayerController->SetViewTarget(CameraRobot);
		}

	}

}

void AFPS_ProjectCharacter::MoveForwardWhileUsingCameraRobot(float Value)
{
	
	if (!bIsUsingCameraRobot)
	{
		return;
	}

	/** This has been copied from MoveForward() */

	if (Value > 0.0f && !bIsShowingCutscene)	// !bIsShowingCutscene can probably be removed
	{
		bWantsToMoveForward = true;

		// Only allow sprinting when moving forward and not crouching
		if (bWantsToSprint && !bStaminaRechargingFromZero && CurrentStamina > 0.0f && !bIsCrouching)
		{
			if (Value < 0.0f)
			{
				bIsSprinting = false;
			}
			else
			{
				bIsSprinting = true;
			}
		}
		else
		{
			bIsSprinting = false;
		}

		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value * GlobalTimeDilation);
	}
	// Character wants to move backwards
	else
	{
		if (GetCharacterMovement()->MaxWalkSpeed == 1000.0f)
		{
			Value *= 0.6f;
		}

		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value * GlobalTimeDilation);
	}

}

void AFPS_ProjectCharacter::MoveRightWhileUsingCameraRobot(float Value)
{
	
	if (!bIsUsingCameraRobot)
	{
		return;
	}

	/** This has been copied from MoveRight() */

	// Do not allow faster left/right movement when sprinting
	if (Value != 0.0f && !bIsShowingCutscene)	// !bIsShowingCutscene can probably be removed
	{
		bWantsToMoveLeft = true;
		bWantsToMoveRight = true;

		if (!bWantsToSprint || (bWantsToSprint && bStaminaRechargingFromZero))
		{

			// add movement in that direction
			AddMovementInput(GetActorRightVector(), Value * GlobalTimeDilation);
		}
		else if (bWantsToSprint && CurrentStamina > 0.0f && !bWantsToMoveForward)
		{

			// add movement in that direction
			AddMovementInput(GetActorRightVector(), Value * GlobalTimeDilation);
		}
	}

}

void AFPS_ProjectCharacter::TurnCameraRobot(float Rate)
{
	if (bIsUsingCameraRobot && Rate != 0.0f)
	{
		if (CameraRobot)
		{

			CameraRobot->ChangeYaw(Rate * CameraRobot->BaseTurnRate * GetWorld()->GetDeltaSeconds());

			if (!PlayerController)
			{
				PlayerController = Cast<APlayerController>(GetController());
			}

			if (PlayerController)
			{
				PlayerController->SetViewTarget(CameraRobot);
			}
		}

	}

}

void AFPS_ProjectCharacter::LookUpCameraRobot(float Rate)
{
	if (bIsUsingCameraRobot && Rate != 0.0f)
	{
		if (CameraRobot)
		{
			CameraRobot->ChangePitch(Rate * CameraRobot->BaseTurnRate * GetWorld()->GetDeltaSeconds());

			if (!PlayerController)
			{
				PlayerController = Cast<APlayerController>(GetController());
			}

			if (PlayerController)
			{
				PlayerController->SetViewTarget(CameraRobot);
			}
		}

	}

}

void AFPS_ProjectCharacter::CheckBeforeAddingYawInput(float Rate)
{
	if (!bIsUsingCameraRobot)
	{
		AddControllerYawInput(Rate * BaseLookUpRate);
	}

}

void AFPS_ProjectCharacter::CheckBeforeAddingPitchInput(float Rate)
{
	if (!bIsUsingCameraRobot)
	{
		AddControllerPitchInput(Rate * BaseTurnRate);
	}

}

void AFPS_ProjectCharacter::SetupLowHealthAnimationOnBeginPlay()
{
	if (BP_LowHealthWidget)
	{
		
		UWorld* World = GetWorld();
		if (World)
		{
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);
			if (PlayerController)
			{

				UUserWidget* Widget = CreateWidget<UUserWidget>(PlayerController, BP_LowHealthWidget);

				LowHealthWidget = Cast<UVer2_MouseoverDisplay>(Widget);
				if (LowHealthWidget)
				{
					
					LowHealthWidget->AddToViewport();

					LowHealthWidgetAnimation = LowHealthWidget->Animation;
					
				}

			}
			
		}
		
	}

}

void AFPS_ProjectCharacter::ShowLowHealthWarning()
{
	if (bIsLowHealthAnimPlaying || bIsUsingCameraRobot)
	{
		return;
	}

	if (LowHealthWidget && LowHealthWidgetAnimation)
	{
			
		LowHealthWidget->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
		LowHealthWidget->PlayAnimation(LowHealthWidgetAnimation, 0.f, 0);
		bIsLowHealthAnimPlaying = true;
	}

}

void AFPS_ProjectCharacter::HideLowHealthWarning()
{
	if (!bIsLowHealthAnimPlaying)
	{
		return;
	}

	if (LowHealthWidget && LowHealthWidgetAnimation)
	{
		
		LowHealthWidget->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.f));
		LowHealthWidget->StopAnimation(LowHealthWidgetAnimation);
		bIsLowHealthAnimPlaying = false;

		
	}

}

void AFPS_ProjectCharacter::LPressed()
{
	LoadGame();
}

void AFPS_ProjectCharacter::SaveGame()
{
	USaveGame* Save = UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass());
	if (Save)
	{
		UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(Save);
		if (SaveGameInstance)
		{
			if (SaveGameInstance->Save(this))
			{
				UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("AutoSave"), 0);
			}
		}
	}

}

void AFPS_ProjectCharacter::LoadGame()
{
	
	USaveGame* Load = UGameplayStatics::LoadGameFromSlot(TEXT("AutoSave"), 0);
	if (Load)
	{
		UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(Load);
		if (SaveGameInstance)
		{
			UGameplayStatics::SetGamePaused(this, false);
			
			SaveGameInstance->Load(this);

		}
	}
	

}

void AFPS_ProjectCharacter::AdjustWeaponValuesOnLoad()
{

	bOkToSwitchWeapons = true;
	bCanFire = true;
	bIsReloading = false;
	bHasRecentlySwitchedWeapons = false;
	bCanFireAfterWeaponSwitch = true;
	bHoldingTrigger = false;

	// Stop any montages that are playing
	UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(0.01f);
	}
	
	// Stop all timer handles for player
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearAllTimersForObject(this);
	}

	ClearHUDText();

	// Update the aim animation info
	AssignAimAnimsOnWeaponChange(EquippedWeapon);

	WeaponRateOfFire = RatesOfFire[EquippedWeapon];
	bEquippedWeaponIsAutomatic = AutomaticStatus[EquippedWeapon];
	ChangeWeaponMesh(EquippedWeapon);

}

void AFPS_ProjectCharacter::AdjustValuesOnLoad()
{
	CurrentHealth = MaxHealth;
	
	bIsSprinting = false;
	bCanStaminaRecharge = true;
	bStaminaRechargingFromZero = false;
	bWantsToSprint = false;
	bCanHUDTextBeOverwritten = true;
	bIsNewTypewriterMessageWaiting = false;
	bShouldTypewriterBlockFlash = false;
	bShouldHUDTextFlash = false;
	bIsAiming = false;
	bIsShowingCutscene = false;


	bIsPickingUpWeapon = false;
	bIsPickingUp = false;
	bIsInteracting = false;
	bWantsToMoveForward = bWantsToMoveLeft = bWantsToMoveRight = false;
	bStaminaRechargeDelayTimerHandleIsRunning = false;
	bIsHealthRecharging = true;
	bWantsToAim = false;
	bIsAimAnim1st = true;


	bIsUsingCameraRobot = false;
	bCanDestroyCameraRobot = false;
	bIsLowHealthAnimPlaying = false;


	// Non player related variables
	UWorld* World = GetWorld();
	if (World)
	{
		UGameplayStatics::SetGlobalTimeDilation(World, 1.f);
	}

	// Load audio/sensitivity settings. Video settings should be saves to disk elsewhere
	// and will load automatically 


	// Maybe also reset the typewriter text to nothing. Right now if a typewriter message is playing
	// then it will freeze on load
	TypewriterText = FString("");

	ResetAIOnLoad();
	
}

void AFPS_ProjectCharacter::ResetAIOnLoad()
{
	
	MouseoverBots.Empty();
	MouseoverBotsMouseoverTime.Empty();
	MouseoverBotsFadeTime.Empty();

	// For enemy characters they should be Destroyed then respawned at predetermined locations
	GameInstance->SpawnAI();

}

void AFPS_ProjectCharacter::OnSniperBotDefeated()
{
	if (bHasDefeatedSniperBot)
	{
		return;
	}
	else
	{
		bHasDefeatedSniperBot = true;
	}
	
	
	UWorld * World = GetWorld();
	if (World)
	{
		// Play closing message
		float MessageDuration = 12.f;
		
		if (BP_EndOfLevelMessage)
		{
			ATutorialMessage * Message = World->SpawnActor<ATutorialMessage>(BP_EndOfLevelMessage, FVector(0.f, 0.f, 3000.f), FRotator(0.f));
			if (Message)
			{
				Message->CreateMessage();
			}
		
		}

		// Fade out camera and show credits
		FTimerHandle PlayCreditsTimerHandle;
		World->GetTimerManager().SetTimer(PlayCreditsTimerHandle, this, &AFPS_ProjectCharacter::PlayCredits, MessageDuration, false);

	}


}

void AFPS_ProjectCharacter::PlayCredits()
{
	float FadeDuration = 4.f;

	// Fade camera before credits
	APlayerCameraManager * CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (CameraManager)
	{
		CameraManager->StartCameraFade(0.f, 1.f, FadeDuration, FColor::Black, true, true);
	}

	UWorld * World = GetWorld();
	if (World && GameInstance)
	{
		FTimerHandle DummyTimerHandle;
		World->GetTimerManager().SetTimer(DummyTimerHandle, GameInstance, &UMyGameInstance::PlayCreditsMovie, FadeDuration + 1.f, false);

	}

}

void AFPS_ProjectCharacter::IncrementNumBotsDefeated()
{
	if (bHasAccessedComputer)
	{
		NumBotsDefeated++;

		if (NumBotsDefeated >= NumBotsDefeatedThreshold)
		{
			bHasDefendedBunker = true;

			if (BP_BunkerDefendSuccessMessage)
			{
				UWorld * World = GetWorld();
				if (World)
				{
					FTimerHandle DummyTimerHandle;
					World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AFPS_ProjectCharacter::PlayBunkerDefendSuccessMessage, 2.f, false);
		
				}
			}

		}

	}

}

void AFPS_ProjectCharacter::PlayBunkerDefendSuccessMessage()
{
	UWorld * World = GetWorld();
	if (World)
	{
		ATutorialMessage * Message = World->SpawnActor<ATutorialMessage>(BP_BunkerDefendSuccessMessage, FVector(0.f, 0.f, 4000.f), FRotator(0.f));
		if (Message)
		{
			// Play typewriter message
			Message->Player = this;
			Message->bIsPlayerInRange = true;
		}

	}

}

void AFPS_ProjectCharacter::OnTABPressed()
{
	bIsTABPressed = true;

}

void AFPS_ProjectCharacter::OnTABReleased()
{
	bIsTABPressed = false;

}

void AFPS_ProjectCharacter::SetupTABControlsMenuOnBeginPlay()
{
	if (BP_ControlsMenu && GameInstance)
	{
		APlayerController * PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		
		TABControlsMenu = CreateWidget<UUserWidget>(PlayerController, BP_ControlsMenu);

		TABControlsMenu->AddToViewport(-4);

		TABControlsMenu->SetVisibility(ESlateVisibility::Hidden);

	}

}

void AFPS_ProjectCharacter::HandleControlsMenuVisibilityOnTick(float DeltaTime)
{
	if (TABControlsMenu)
	{
		if (bIsTABPressed)
		{
			/* Fade in controls menu widget */
			if (!TABControlsMenu->IsVisible())
			{
				TABControlsMenu->SetVisibility(ESlateVisibility::Visible);
			}

			FLinearColor Color = TABControlsMenu->ColorAndOpacity;
			
			TABControlsMenu->SetColorAndOpacity(FLinearColor(Color.R, Color.G, Color.B, Color.A + DeltaTime * ControlsMenuFadeRate));

			if (TABControlsMenu->ColorAndOpacity.A > 1.f)
			{
				Color = TABControlsMenu->ColorAndOpacity;
				TABControlsMenu->SetColorAndOpacity(FLinearColor(Color.R, Color.G, Color.B, 1.f));

			}

		}
		else
		{
			/* Fade out controls menu widget */

			FLinearColor Color = TABControlsMenu->ColorAndOpacity;

			TABControlsMenu->SetColorAndOpacity(FLinearColor(Color.R, Color.G, Color.B, Color.A - DeltaTime * ControlsMenuFadeRate * 1.5f));

			if (TABControlsMenu->ColorAndOpacity.A < 0.f)
			{
				Color = TABControlsMenu->ColorAndOpacity;
				TABControlsMenu->SetColorAndOpacity(FLinearColor(Color.R, Color.G, Color.B, 0.f));

				TABControlsMenu->SetVisibility(ESlateVisibility::Hidden);

			}
		}
	}
	

}

void AFPS_ProjectCharacter::ChangeMouseSensitivity(float NewMouseSensitivity)
{
	if (NewMouseSensitivity == -1.f)
	{
		if (GameInstance)
		{
			BaseLookUpRate = BaseTurnRate = GameInstance->MouseSensitivity;
		}
	}
	else
	{
		// Use value passed in as parameter

		if (NewMouseSensitivity > 0.f && NewMouseSensitivity <= 100.f)
		{
			BaseLookUpRate = BaseTurnRate = NewMouseSensitivity;
		}

	}

}

void AFPS_ProjectCharacter::MakeMovementSpeedCorrectForBeginPlay()
{
	UCharacterMovementComponent * CharMoveComp = GetCharacterMovement();
	if (CharMoveComp)
	{
		CharMoveComp->MaxWalkSpeed = 600.f;
	}

}

void AFPS_ProjectCharacter::CheckIfFallenOutOfWorldOnTick(const float DeltaTime)
{
	FVector Location = GetActorLocation();
	
	if (Location.Z < -4180.f && GameInstance)
	{
		ReceiveDamage(101.f / GameInstance->DamageMultiplier);
	}

}

void AFPS_ProjectCharacter::MakeFiringAfterSwitchWeaponsOk()
{
	bCanFireAfterWeaponSwitch = true;

}

void AFPS_ProjectCharacter::TryStartStaminaRechargeTimer()
{
	if (!bStaminaRechargeDelayTimerHandleIsRunning)
	{
		bStaminaRechargeDelayTimerHandleIsRunning = true;
		GetWorld()->GetTimerManager().SetTimer(StaminaRechargeDelayTimerHandle, this, &AFPS_ProjectCharacter::AllowStaminaToRecharge, StaminaRechargeDelay, false);
	}

}

void AFPS_ProjectCharacter::HandleSprintForTick(float DeltaTime)
{
	// Stamina
	if (bWantsToSprint && !bIsCrouching)
	{
		if (CurrentStamina <= 0.0f)
		{
			GetCharacterMovement()->MaxWalkSpeed = 600.0f;

			bIsSprinting = false;
			TryStartStaminaRechargeTimer();

			if (bCanStaminaRecharge)
			{
				UpdateCurrentStamina(DeltaTime * StaminaRechargeRate * InitialStamina);
			}

		}
		else
		{
			//FRotator Rotation = GetActorRotation();
			//FVector Velocity = Rotation.UnrotateVector(GetVelocity());	// X = vel fwd, Y = vel right, Z = vel up

			// Only decrease stamina if actually moving forward and on the ground, IsMovingOnGround actually only 
			// checks if airborne.
			if (!bStaminaRechargingFromZero && GetMovementComponent()->IsMovingOnGround()
				&& bWantsToMoveForward)
			{
				GetCharacterMovement()->MaxWalkSpeed = 1000.0f;

				bCanStaminaRecharge = false;
				UpdateCurrentStamina(-DeltaTime * StaminaDepletionRate * InitialStamina);
			}
			else
			{
				// If here then shift is held down but character is either:
				// - standing still
				// - moving left/right
				// - in the air
				// so stamina can recharge
				TryStartStaminaRechargeTimer();

				GetCharacterMovement()->MaxWalkSpeed = 600.0f;

				bIsSprinting = false;

				// Check if stamina can recharge
				if (bCanStaminaRecharge)
				{
					UpdateCurrentStamina(DeltaTime * StaminaRechargeRate * InitialStamina);

				}

			}
		}
	}
	else
	{
		if (bIsCrouching)
		{
			GetCharacterMovement()->MaxWalkSpeed = CrouchMoveSpeed;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		}

		bIsSprinting = false;

		if (bCanStaminaRecharge)
		{
			UpdateCurrentStamina(DeltaTime * StaminaRechargeRate * InitialStamina);

		}
	}

}

void AFPS_ProjectCharacter::OnFire()
{
	
	// Return if sprinting or switching weapons
	if (bIsSprinting || !bCanFireAfterWeaponSwitch)
	{
		return;
	}

	// Needs to be set for all weapons but only affects automatic ones
	bHoldingTrigger = true;

	// Holds how long the fire animation is
	float AnimationDuration = 0.0f;

	if (bCanFire && !bIsReloading)
	{
		
		bCanFire = false;

		// try and fire a projectile
		if (ProjectileClass != NULL)
		{

			UWorld* const World = GetWorld();
			if (World != NULL)
			{
				
				if (bUsingMotionControllers)
				{
					const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
					const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
					World->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);	// changed from <AFPS_ProjectProjectile>
				}
				else
				{
					if (GetCurrentClipAmmo(EquippedWeapon) == 0)
					{
						// Reload automatically after 0.2 seconds if there is still ammo
						if (GetCurrentNonClipAmmo(EquippedWeapon) > 0)
						{
							if (!GetWorld()->GetTimerManager().IsTimerActive(NoticeNeedToReloadDelayTimerHandle))
							{
								GetWorld()->GetTimerManager().SetTimer(NoticeNeedToReloadDelayTimerHandle, this, &AFPS_ProjectCharacter::Reload, 0.2f, false);
							}
							
						}
						else
						{
							// Completely out of ammo
							FString Message = FString("Out of ammo");
							
							CreateHUDMessage(Message, true, true);

							FTimerHandle TextTimerHandle;
							World->GetTimerManager().SetTimer(TextTimerHandle, this, &AFPS_ProjectCharacter::ClearHUDText, 2.f, false);

						}
						
						bCanFire = true;

						return;
					}
					else
					{
						// Take away 1 ammo from clip
						AdjustCurrentClipAmmo(EquippedWeapon, -1);
					}

					const FRotator SpawnRotation = GetControlRotation();
					// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
					const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

					//Set Spawn Collision Handling Override
					FActorSpawnParameters ActorSpawnParams;
					ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

					// spawn the projectile at the muzzle
					AProjectile* Bullet = World->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);	// changed from <AFPS_ProjectProjectile>
				
					// Play fire animation montage
					if (FireAnimation)
					{
						if (Mesh1P)
						{
							UAnimInstance* AnimInst = Mesh1P->GetAnimInstance();
							if (AnimInst)
							{
								// Only play montage if not aiming with an automatic weapon, which is only the auto rifle right now
								if (bEquippedWeaponIsAutomatic)
								{
									if (!bWantsToAim/*CurrentLocAimAnim.GetPlaybackPosition() < 0.01f*/)
									{
										if (FireAnimation)
										{
											AnimInst->Montage_Play(FireAnimation);
										}
									}
									
								}
								else
								{
									if (FireAnimationNoLoop)
									{
										if (EquippedWeapon == 0 || EquippedWeapon == 3)
										{
											AnimInst->Montage_Play(FireAnimationNoLoop, 0.5f);
										}
										else
										{
											AnimInst->Montage_Play(FireAnimationNoLoop);
										}
									}
								}
							}
						}
					}

					// Add camera shake
					PlayOnFireCameraShake();

					// Add recoil
					AddRecoilOnFire();

					// Spawn muzzle flash
					CreateMuzzleFlash();

					// try and play the sound if specified
					if (EquippedWeapon != 1)
					{
						if (WeaponFireSoundEffects.IsValidIndex(EquippedWeapon))
						{
							UMyStatics::PlayASound(this, WeaponFireSoundEffects[EquippedWeapon], GetActorLocation());
						}
					}
					else
					{
						// Play auto rifle fire sound at random
						PlaySoundAtRandom(AutoRifleFireSounds);
					}

					// param1: timer to use, param2: ??, param3: function to call after time, param4: time of timer in seconds, param5: loop?
					GetWorld()->GetTimerManager().SetTimer(FireDelayTimerHandle, this, &AFPS_ProjectCharacter::ResetFire, WeaponRateOfFire, false);
				}
			}
		}

	}
}

void AFPS_ProjectCharacter::ResetFire()
{
	if (Mesh1P)
	{
		UAnimInstance* AnimInst = Mesh1P->GetAnimInstance();
		if (AnimInst)
		{
			AnimInst->Montage_Stop(0.01f, FireAnimation);
			AnimInst->Montage_Stop(0.01f, FireAnimationNoLoop);
		}
	}
	
	bCanFire = true;
	GetWorldTimerManager().ClearTimer(FireDelayTimerHandle);

	// Continue firing if the weapon is automatic
	if (bEquippedWeaponIsAutomatic && bHoldingTrigger)
	{
		OnFire();
	}
}

void AFPS_ProjectCharacter::LCTRLPressed()
{
	if (bIsCrouching)
	{
		return;
	}

	CrouchPlayer();

}

void AFPS_ProjectCharacter::LCTRLReleased()
{
	if (!bIsCrouching)
	{
		return;
	}

	// Do a line trace upwards to see if it is ok to uncrouch
	
	FVector RayTraceStart = GetFirstPersonCameraComponent()->GetComponentLocation();
	FVector RayTraceEnd = (GetActorUpVector()) * ((StandingHalfHeight - CrouchingHalfHeight) * 1.5f) + RayTraceStart;
	
	FHitResult HitResult;
	FCollisionQueryParams CQP;
	CQP.AddIgnoredComponent(GetCapsuleComponent());

	UWorld* World = GetWorld();
	if (World->LineTraceSingleByChannel(HitResult, RayTraceStart, RayTraceEnd, ECC_Visibility, CQP))
	{
		DrawDebugLine(World, RayTraceStart, RayTraceEnd, FColor(255, 0, 0), true);
		
		// Line trace hit something
		World->GetTimerManager().SetTimer(UnCrouchTimerHandle, this, &AFPS_ProjectCharacter::LCTRLReleased, 0.05f, false);

	}
	else
	{
		// Line trace did not hit anything
		UnCrouchPlayer();
	}

	

}

void AFPS_ProjectCharacter::CrouchPlayer()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(UnCrouchTimerHandle);
	}
	
	bIsCrouching = true;

	CrouchTimeline.Play();
	
}

void AFPS_ProjectCharacter::UnCrouchPlayer()
{
	bIsCrouching = false;

	CrouchTimeline.Reverse();

}

void AFPS_ProjectCharacter::HandleCrouchProgress(float Value)
{
	float NewHeight = FMath::Lerp(StandingHalfHeight, CrouchingHalfHeight, Value);

	GetCapsuleComponent()->SetCapsuleHalfHeight(NewHeight);
	
	// Make the camera go even lower
	if (CrouchTimeline.IsPlaying())
	{
		if (!CrouchTimeline.IsReversing())
		{
			GetFirstPersonCameraComponent()->SetRelativeLocation(GetFirstPersonCameraComponent()->RelativeLocation - FVector(0.f, 0.f, NewHeight * 0.01f));
		}
		else
		{
			GetFirstPersonCameraComponent()->SetRelativeLocation(GetFirstPersonCameraComponent()->RelativeLocation + FVector(0.f, 0.f, NewHeight * 0.01f));
		}
	}
	
	

}

void AFPS_ProjectCharacter::HandleCrouchAnimOnTick(float DeltaTime)
{
	CrouchTimeline.TickTimeline(DeltaTime);

}

void AFPS_ProjectCharacter::SetupCrouchAnimForBeginPlay()
{
	StandingHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	
	CrouchingHalfHeight = StandingHalfHeight / 2.4f;

	if (CrouchAnimCurve)
	{
		FOnTimelineFloat ProgressFunction;

		ProgressFunction.BindUFunction(this, FName("HandleCrouchProgress"));

		CrouchTimeline.AddInterpFloat(CrouchAnimCurve, ProgressFunction);
		CrouchTimeline.SetLooping(false);

	}

}

void AFPS_ProjectCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AFPS_ProjectCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AFPS_ProjectCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = false;
}

void AFPS_ProjectCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void AFPS_ProjectCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void AFPS_ProjectCharacter::MoveForward(float Value)
{
	if (bIsUsingCameraRobot)
	{
		MoveCameraRobotForward(Value);
		return;
	}
	
	if (Value > 0.0f && !bIsShowingCutscene)	// !bIsShowingCutscene can probably be removed
	{	
		bWantsToMoveForward = true;
		
		// Only allow sprinting when moving forward and not crouching
		if (bWantsToSprint && !bStaminaRechargingFromZero && CurrentStamina > 0.0f && !bIsCrouching)
		{
			if (Value < 0.0f)
			{
				bIsSprinting = false;
			}
			else
			{
				bIsSprinting = true;
			}
		}
		else
		{
			bIsSprinting = false;
		}

		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value * GlobalTimeDilation);
	}
	// Character wants to move backwards
	else
	{
		if (GetCharacterMovement()->MaxWalkSpeed == 1000.0f)
		{
			Value *= 0.6f;
		}
		
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value * GlobalTimeDilation);
	}
}

void AFPS_ProjectCharacter::MoveRight(float Value)
{
	if (bIsUsingCameraRobot)
	{
		MoveCameraRobotRight(Value);
		return;
	}
	
	// Do not allow faster left/right movement when sprinting
	if (Value != 0.0f && !bIsShowingCutscene)	// !bIsShowingCutscene can probably be removed
	{
		bWantsToMoveLeft = true;
		bWantsToMoveRight = true;
		
		if (!bWantsToSprint || (bWantsToSprint && bStaminaRechargingFromZero))
		{
			
			// add movement in that direction
			AddMovementInput(GetActorRightVector(), Value * GlobalTimeDilation);
		}
		else if (bWantsToSprint && CurrentStamina > 0.0f && !bWantsToMoveForward)
		{

			// add movement in that direction
			AddMovementInput(GetActorRightVector(), Value * GlobalTimeDilation);
		}
	}
}

void AFPS_ProjectCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate);// *GetWorld()->GetDeltaSeconds());
}

void AFPS_ProjectCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate);// *GetWorld()->GetDeltaSeconds());
}

void AFPS_ProjectCharacter::BeginPickup()
{
	bIsPickingUp = true;

	// For opening gates and stuff. Makes F button redundent
	bIsInteracting = true;

}

void AFPS_ProjectCharacter::EndPickup()
{
	bIsPickingUp = false;

	bIsInteracting = false;

}

bool AFPS_ProjectCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	bool bResult = false;
	if (FPlatformMisc::GetUseVirtualJoysticks() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		bResult = true;
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AFPS_ProjectCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AFPS_ProjectCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AFPS_ProjectCharacter::TouchUpdate);
	}
	return bResult;
}

void AFPS_ProjectCharacter::CheckJump()
{
	if (!bIsShowingCutscene && !bIsCrouching)
	{
		ACharacter::Jump();
	}

}

void AFPS_ProjectCharacter::MakeOkToSwitchWeaponsTrue()
{
	bOkToSwitchWeapons = true;

}

void AFPS_ProjectCharacter::CreateHUDMessage(FString & Text, bool bCanBeOverwritten, bool bShouldFlash)
{
	// If clearing the message by passing an empty string is your goal use ClearHUDText() instead
	if (Text != FString(""))
	{
		if (bCanHUDTextBeOverwritten)
		{
			HUDText = Text;

			bCanHUDTextBeOverwritten = bCanBeOverwritten;

			bShouldHUDTextFlash = bShouldFlash;

			if (bShouldHUDTextFlash)
			{
				// Make HUD text flash with animation
				if (HUDWidget && HUDTextFlashAnimation)
				{
					
					HUDWidget->PlayAnimation(HUDTextFlashAnimation, 0.f, 4);
				}

			}

		}
	}

}

void AFPS_ProjectCharacter::CreateTypewriterMessage(FString & Message, TArray<float> Delays, TArray<int32> WordLengths, float EraseTimer)
{
	// New method structure: add message to array. Check if message is already being displayed
	// If it is then return - message will be played automatically when current message  finishes
	// If no message is playing then start message.
	
	FTypewriterMessage * NewMessage = new FTypewriterMessage(Message, Delays, WordLengths, EraseTimer);

	TypewriterQueue.Enqueue(NewMessage);

	if (TypewriterLock.TryLock())
	{
		// No message currently being displayed
		// Give it a slight delay before message starts
		UWorld * World = GetWorld();
		if (World)
		{
			FTimerHandle DummyTimerHandle;

			World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AFPS_ProjectCharacter::DisplayNextTypewriterMessage, 0.5f, false);

		}
			
	}
	else
	{
		// do nothing
	}
	

	return;

	// New code stops here
	
	bIsNewTypewriterMessageWaiting = true;
	
	TypewriterLock.Lock();

	TypewriterFullText = Message;

	TypewriterDelays = Delays;

	TypewriterWordLengths = WordLengths;

	TypewriterEraseTime = EraseTimer;

	AppendTypewriterMessage();

}

void AFPS_ProjectCharacter::DisplayNextTypewriterMessage()
{
	// I think these 2 lines are causing crashes
	FTypewriterMessage * FrontOfQueue;
	TypewriterQueue.Peek(FrontOfQueue);

	//bIsNewTypewriterMessageWaiting = true;
	if (FrontOfQueue)
	{
		// Not neccessary - can probably be removed. If here lock is already acquired
		TypewriterLock.Lock();

		TypewriterFullText = FrontOfQueue->Message;

		TypewriterDelays = FrontOfQueue->Delays;

		TypewriterWordLengths = FrontOfQueue->WordLengths;

		TypewriterEraseTime = FrontOfQueue->EraseTimer;

		AppendTypewriterMessage();
	}

}

void AFPS_ProjectCharacter::AppendTypewriterMessage()
{	

	//	 Commented for new code
	//if (bIsNewTypewriterMessageWaiting && TypewriterIndex != 0)
	//{
	//	TypewriterText = FString("");
	//	
	//	bShouldTypewriterBlockFlash = false;
	//	
	//	ClearTypewriterMessage();
	//
	//	bIsNewTypewriterMessageWaiting = false;
	//
	//	TypewriterModifyLock.Unlock();
	//	
	//	TypewriterLock.Unlock();
	//
	//	return;
	//
	//}
	//else if (bIsNewTypewriterMessageWaiting)
	//{
	//	bIsNewTypewriterMessageWaiting = false;
	//
	//}

	TypewriterModifyLock.Lock();

	if (TypewriterFullText.IsValidIndex(TypewriterIndex))
	{
		// Remove ending block
		TypewriterText.RemoveFromEnd(Block);
		
		// Check to see if starting a new word
		if (TypewriterIndex != 0 && TypewriterFullText[TypewriterIndex - 1] == Space)
		{
			// Starting a new word
			TypewriterWordIndex++;
			
			// Check to see if new word will go wrap over to new line when it is fully typed 
			if (TypewriterWordLengths[TypewriterWordIndex] + TypewriterLineCharNum > TypewriterLineLength
				&& TypewriterWordLengths[TypewriterWordIndex] < TypewriterLineLength)
			{
				// Go to a new line
				TypewriterText.AppendChar(*TEXT("\n"));
				TypewriterLineCharNum = 1;
			}
			else
			{
				TypewriterText.AppendChar(TypewriterFullText[TypewriterIndex++]);
				TypewriterLineCharNum++;
			}

		}
		else
		{
			TypewriterText.AppendChar(TypewriterFullText[TypewriterIndex++]);
			TypewriterLineCharNum++;
		}
		
		// Add ending block
		TypewriterText.AppendChar(*TEXT("\u2588"));

		// If delay is > DefaultDelay then make ending block flash
		if (TypewriterDelays[TypewriterIndex] > TypewriterDefaultDelay)
		{
			bShouldTypewriterBlockFlash = true;
			ToggleEndingBlock();
		}
		else
		{
			bShouldTypewriterBlockFlash = false;
		}

		TypewriterModifyLock.Unlock();

		GetWorld()->GetTimerManager().SetTimer(TypewriterTimerHandle, this, &AFPS_ProjectCharacter::AppendTypewriterMessage, TypewriterDelays[TypewriterIndex], false);
	
	}
	else
	{
		/* Message has finished. */

		// Make ending block flash by replacing it with a space back and forth
		bShouldTypewriterBlockFlash = true;
		
		ToggleEndingBlock();

		TypewriterWordIndex = 0;

		TypewriterIndex = 0;

		TypewriterLineCharNum = 0;

		TypewriterModifyLock.Unlock();

		TypewriterLock.Unlock();

		//Remove it after EraseTimer seconds
		GetWorld()->GetTimerManager().SetTimer(TypewriterEraseTimerHandle, this, &AFPS_ProjectCharacter::ClearTypewriterMessage, TypewriterEraseTime, false);

	}
	
}

TArray<int32> AFPS_ProjectCharacter::FindWordLengths(FString & Input)
{
	int32 WordLength = 0;
	
	TArray<int32> ToReturn;

	int32 index = 0;

	for (int i = 0; i < Input.Len(); i++)
	{
		if (Input[i] == Space)
		{
			WordLength = 0;
		}
		else
		{
			WordLength++;
			ToReturn.Insert(WordLength, index++);
		}
	}

	return ToReturn;

}

void AFPS_ProjectCharacter::ClearTypewriterMessage()
{
	TypewriterLock.Lock();
	
	TypewriterWordIndex = 0;

	TypewriterIndex = 0;

	TypewriterLineCharNum = 0;

	TypewriterText = FString("");

	// New code starts here

	FTypewriterMessage * Temp;
	TypewriterQueue.Dequeue(Temp);
	delete(Temp);

	// Check to see if any messages are waiting and play one if one is
	if (!TypewriterQueue.IsEmpty())
	{
		DisplayNextTypewriterMessage();
		
	}
	else
	{
		TypewriterLock.Unlock();
		
	}
	
	return;
	// New code stops here

	TypewriterLock.Unlock();

}

void AFPS_ProjectCharacter::ToggleEndingBlock()
{
	
	TypewriterModifyLock.Lock();
	
	if (bShouldTypewriterBlockFlash)
	{
		if (TypewriterText.Len() > 0)
		{

			if (TypewriterText.RemoveFromEnd(FString(" ")))
			{
				TypewriterText.AppendChar(*TEXT("\u2588"));

			}
			else if (TypewriterText.RemoveFromEnd(Block))
			{
				TypewriterText.AppendChar(Space);

			}

			TypewriterModifyLock.Unlock();

			UWorld* World = GetWorld();
			if (World)
			{
				World->GetTimerManager().SetTimer(TypewriterBlockFlashTimerHandle, this, &AFPS_ProjectCharacter::ToggleEndingBlock, 0.5f, false);
			}

		}

		TypewriterModifyLock.Unlock();

	}
	else
	{
		TypewriterModifyLock.Unlock();

	}

}

void AFPS_ProjectCharacter::ResetAHUDInfoText(FHUDInfoText & HUDInfoText)
{
	HUDInfoText.Text = FString("");
	HUDInfoText.AmmoAmount = 0;
	HUDInfoText.AmmoID = -1;

}

// This should take a parameter that is a FString to check that the right message is being cleared.
// Also it should take a boolean that says whether it should clear it no matter what regardless
// of bCanHUDTextBeOverwrittens value
void AFPS_ProjectCharacter::ClearHUDText()
{
	if (bCanHUDTextBeOverwritten)
	{
		HUDText = FString("");
		bShouldHUDTextFlash = false;
	}

}

void AFPS_ProjectCharacter::UpdateHUDInfoOnTick(float DeltaTime)
{
	
	if (HUDInfoText3.AmmoID != -1)
	{
		HUDInfoText3.Time -= DeltaTime;
		if (HUDInfoText3.Time <= 0.0f)
		{
			ResetAHUDInfoText(HUDInfoText3);

		}
	}

	if (HUDInfoText2.AmmoID != -1)
	{
		HUDInfoText2.Time -= DeltaTime;
		if (HUDInfoText2.Time <= 0.0f)
		{
			if (HUDInfoText3.AmmoID != -1)
			{
				// Message 3 is not empty so shift it up
				CopyFHUDInfoText(HUDInfoText2, HUDInfoText3);
				ResetAHUDInfoText(HUDInfoText3);
			}
			else
			{
				// Delete message 2
				ResetAHUDInfoText(HUDInfoText2);
			}
		}
	}

	if (HUDInfoText1.AmmoID != -1)
	{
		HUDInfoText1.Time -= DeltaTime;
		if (HUDInfoText1.Time <= 0.0f)
		{
			if (HUDInfoText2.AmmoID != -1)
			{
				// Message 2 is not empty so shift it up
				CopyFHUDInfoText(HUDInfoText1, HUDInfoText2);
				ResetAHUDInfoText(HUDInfoText2);

				if (HUDInfoText3.AmmoID != -1)
				{
					// Message 3 is not empty so shift it up
					CopyFHUDInfoText(HUDInfoText2, HUDInfoText3);
					ResetAHUDInfoText(HUDInfoText3);
				}

			}
			else
			{
				// Delete message 1
				ResetAHUDInfoText(HUDInfoText1);
			}
		}
	}

}

int AFPS_ProjectCharacter::GetEmptyIndexForHUDInfo()
{
	if (HUDInfoText1.AmmoID == -1)
	{
		return 1;
	}
	else if (HUDInfoText2.AmmoID == -1)
	{
		return 2;
	}
	else
	{
		return 3;
	}

}
/*
void AFPS_ProjectCharacter::CreateHUDMessage(const FString & Text, const float Length, bool bShouldFlash)
{

	if (HUDText != Text)
	{
		bShouldHUDTextFlash = bShouldFlash;

		HUDText = Text;

		if (Length != 0.0f)
		{
			FTimerHandle TextTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TextTimerHandle, this, &AFPS_ProjectCharacter::ClearHUDText, Length, false);
		}
	}

}

void AFPS_ProjectCharacter::ClearHUDInfoText()
{
	HUDInfoText = FString("");

}

FString AFPS_ProjectCharacter::GetWeaponTypeString(uint8 Type, float MessageLength)
{
	switch (Type)
	{
	case 0:
		return FString(" Laser Rifle");

	case 1:
		return FString(" Auto Rifle");

	case 2:
		return FString(" Grenade Launcher");

	case 3:
		return FString(" Sniper Rifle");

	default:
		return FString(" Unknown weapon type");
	}
}
*/
// This should be the first function called when wanting to write text to HUD
void AFPS_ProjectCharacter::CreateFinalHUDInfoMessage(int32 Index, int32 AmmoAmount, float MessageLength)
{
	// Check if a message in the message box already contains the the same type of ammo.
	
	int PreviousAmmo;
	
	int i = 0;
	if (HUDInfoText1.AmmoID == Index)
	{
		PreviousAmmo = HUDInfoText1.AmmoAmount;
		i = 1;
	}
	else if (HUDInfoText2.AmmoID == Index)
	{
		PreviousAmmo = HUDInfoText2.AmmoAmount;
		i = 2;
	}
	else if (HUDInfoText3.AmmoID == Index)
	{
		PreviousAmmo = HUDInfoText3.AmmoAmount;
		i = 3;
	}
	
	int AvailableIndex;

	switch (i)
	{
	case 0:
		// Create a new message
		AvailableIndex = GetEmptyIndexForHUDInfo();
		break;

	case 1:
		HUDInfoText1.Text = GetMessageFromID(Index, PreviousAmmo + AmmoAmount, MessageLength);
		HUDInfoText1.AmmoAmount = PreviousAmmo + AmmoAmount;
		HUDInfoText1.Time = MessageLength;
		HUDInfoText1.AmmoID = Index;
		return;

	case 2:
		HUDInfoText2.Text = GetMessageFromID(Index, PreviousAmmo + AmmoAmount, MessageLength);
		HUDInfoText2.AmmoAmount = PreviousAmmo + AmmoAmount;
		HUDInfoText2.Time = MessageLength;
		HUDInfoText2.AmmoID = Index;
		return;

	case 3:
		HUDInfoText3.Text = GetMessageFromID(Index, PreviousAmmo + AmmoAmount, MessageLength);
		HUDInfoText3.AmmoAmount = PreviousAmmo + AmmoAmount;
		HUDInfoText3.Time = MessageLength;
		HUDInfoText3.AmmoID = Index;
		return;

	default:
		break;
	}
	
	// Put the message in the info HUD
	switch (AvailableIndex)
	{
	case 1:
		HUDInfoText1.Text = GetMessageFromID(Index, AmmoAmount, MessageLength);
		HUDInfoText1.AmmoAmount = AmmoAmount;
		HUDInfoText1.Time = MessageLength;
		HUDInfoText1.AmmoID = Index;	
		return;

	case 2:
		HUDInfoText2.Text = GetMessageFromID(Index, AmmoAmount, MessageLength);
		HUDInfoText2.AmmoAmount = AmmoAmount;
		HUDInfoText2.Time = MessageLength;
		HUDInfoText2.AmmoID = Index;
		return;

	case 3:
		HUDInfoText3.Text = GetMessageFromID(Index, AmmoAmount, MessageLength);
		HUDInfoText3.AmmoAmount = AmmoAmount;
		HUDInfoText3.Time = MessageLength;
		HUDInfoText3.AmmoID = Index;
		return;

	default:
		// All 3 message slots are full. Replace 3rd message and shift others up. 
		// Do this by making the current 1st message time equal to zero so it gets 
		// removed on the next tick. 

		return;
	}

}

FString AFPS_ProjectCharacter::GetMessageFromID(int32 Index, int32 AmmoAmount, float MessageLength)
{

	FString S;
	
	if (AmmoAmount > 1)
	{
		// Pluralise message
		S = FString("s");
	}

	switch (Index)
	{
	// case 0 - 4 are for ammo pickups
	case 0:
		return FString("Picked up ") + FString::FromInt(AmmoAmount) + FString(" Laser Cell") + S;

	case 1:
		return FString("Picked up ") + FString::FromInt(AmmoAmount) + FString(" Auto Rifle Round") + S;

	case 2:
		return FString("Picked up ") + FString::FromInt(AmmoAmount) + FString(" Grenade Launcher Round") + S;

	case 3:
		return FString("Picked up ") + FString::FromInt(AmmoAmount) + FString(" Sniper Rifle Round") + S;

	case 4:
		// Max ammo message for laser rifle ammo
		return FString("Max Laser Rifle ammo");

	case 5:
		// Max ammo message for laser rifle ammo
		return FString("Max Auto Rifle ammo");
	
	case 6:
		// Max ammo message for laser rifle ammo
		return FString("Max Grenade Launcher ammo");
	
	case 7:
		// Max ammo message for laser rifle ammo
		return FString("Max Sniper Rifle ammo");

	default:
		// Add more cases to cater for other message types
		return FString(" Unknown ammo type");
	}

}

void AFPS_ProjectCharacter::CopyFHUDInfoText(FHUDInfoText & ToBeChanged, FHUDInfoText & ToBeCloned)
{
	ToBeChanged.Text = ToBeCloned.Text;
	ToBeChanged.Time = ToBeCloned.Time;
	ToBeChanged.AmmoAmount = ToBeCloned.AmmoAmount;
	ToBeChanged.AmmoID = ToBeCloned.AmmoID;

}

// This function will not be used but is referenced in blueprints. The references should toggle the value of bWantsToAim instead.
void AFPS_ProjectCharacter::ToggleSniperScope(bool Value)
{
	if (Value)
	{
		ShowSniperOverlay();
		GetFirstPersonCameraComponent()->SetFieldOfView(10.f);
		Mesh1P->SetVisibility(false);
		FP_GunSniperRifle->SetVisibility(false);

	}
	else
	{
		HideSniperOverlay();
		GetFirstPersonCameraComponent()->SetFieldOfView(90.f);
		Mesh1P->SetVisibility(true);
		
		if (EquippedWeapon == 3)
		{
			FP_GunSniperRifle->SetVisibility(true);
		}

	}

}

void AFPS_ProjectCharacter::PlayOnFireCameraShake()
{
	if (WeaponCameraShakes.IsValidIndex(EquippedWeapon))
	{
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), WeaponCameraShakes[EquippedWeapon], GetActorLocation(), 0.f, 200.f);
		
	}

}

void AFPS_ProjectCharacter::AddRecoilOnFire()
{
	// If experiencing recoil then make it not ok to switch weapons. Weapon switching while experiencing recoil
	// may mess up the camera. I'll try it with this commented but switching while experiencing recoil
	// may be not allowed
	//bOkToSwitchWeapons = false;
	
	if (!RecoilAmounts.IsValidIndex(EquippedWeapon) || !RecoilAmounts.IsValidIndex(EquippedWeapon + 4))
	{
		return;
	}

	FRotator Min = RecoilAmounts[EquippedWeapon];
	FRotator Max = RecoilAmounts[EquippedWeapon + 4];

	float Yaw = FMath::RandRange(Min.Yaw, Max.Yaw);
	float Pitch = FMath::RandRange(Min.Pitch, Max.Pitch);
	// If you want roll adjust the camera shake

	// 75% chance of yaw going to the right
	if (FMath::RandRange(0, 3) != 0)
	{
		Yaw = -Yaw;
	}

	CurrentRecoilAmount = FRotator(Pitch, Yaw, 0.f);

	if (bIsCrouching)
	{
		// If crouching recoil is reduced by half
		CurrentRecoilAmount *= 0.5f;
	}

	RecoilTimeline.PlayFromStart();

}

void AFPS_ProjectCharacter::SetupRecoilForBeginPlay()
{
	RecoilProgressFunction.BindUFunction(this, FName("HandleRecoilAnimationProgress"));

	RecoilTimeline.AddInterpFloat(LaserRifleRecoilCurve, RecoilProgressFunction);
	RecoilTimeline.SetLooping(false);

}

void AFPS_ProjectCharacter::HandleRecoilAnimationProgress(float Value)
{
	UCameraComponent* Camera = GetFirstPersonCameraComponent();
	if (Camera)
	{
		FRotator NewRotation = FMath::Lerp(FRotator(0.f), -CurrentRecoilAmount, Value);

		AddControllerYawInput(NewRotation.Yaw);
		AddControllerPitchInput(NewRotation.Pitch);

	}

}

void AFPS_ProjectCharacter::UpdateRecoilAnimationOnTick(float DeltaTime)
{
	if (RecoilTimeline.IsPlaying())
	{
		RecoilTimeline.TickTimeline(DeltaTime * GlobalTimeDilation);
	}
}

void AFPS_ProjectCharacter::CreateMuzzleFlash()
{
	if (MuzzleFlashes.IsValidIndex(EquippedWeapon))
	{
		UWorld* World = GetWorld();
		if (World)
		{
			if (EquippedWeapon == 1)
			{
				UGameplayStatics::SpawnEmitterAttached(MuzzleFlashes[EquippedWeapon], FP_GunAutoRifle, FName("MuzzleFlashSocket"));
			}
			else if (EquippedWeapon == 2)
			{
				UGameplayStatics::SpawnEmitterAttached(MuzzleFlashes[EquippedWeapon], FP_GunGrenadeLauncher, FName("MuzzleFlashSocket"));

			}
			else if (EquippedWeapon == 3)
			{
				UGameplayStatics::SpawnEmitterAttached(MuzzleFlashes[EquippedWeapon], FP_GunSniperRifle, NAME_None, FVector(0.f, -45.8f, 2.65f), FRotator(0.f, -90.f, 0.f));

			}
		}
		
	}

}

void AFPS_ProjectCharacter::ChangeMuzzleLocation(uint8 Type)
{
	if (MuzzleLocations.IsValidIndex(Type))
	{
		FP_MuzzleLocation->SetRelativeLocation(MuzzleLocations[Type]);
	}

}

void AFPS_ProjectCharacter::KnockOutOfScope()
{
	ToggleSniperScope(false);

	// Reset aim animation to start. This should do it. Only does it for sniper rifle
	if (EquippedWeapon == 3)
	{
		AssignAimAnimsOnWeaponChange(EquippedWeapon);
	}
	
	// Play a camera shake no matter what weapon is equipped
	UWorld* World = GetWorld();
	if (World && BP_SniperRifleScopeLossCameraShake)
	{
		UGameplayStatics::PlayWorldCameraShake(World, BP_SniperRifleScopeLossCameraShake, GetActorLocation(), 0.f, 200.f);

	}
	
}

bool AFPS_ProjectCharacter::ReceiveDamage(float Amount)
{
	UWorld* World = GetWorld();
	
	World->GetTimerManager().ClearTimer(HealthRechargeTimerHandle);
	World->GetTimerManager().ClearTimer(RecentDamageTimerHandle);

	CurrentHealth -= (Amount * GameInstance->DamageMultiplier);
	bIsHealthRecharging = false;

	if (CurrentHealth <= 0.0f)
	{
		GameOver();

		return false;

	}
	else
	{
		if (CurrentHealth / MaxHealth < LowHealthPercentage)
		{
			ShowLowHealthWarning();
		}
		
		// Set delay for when health can recharge again
		World->GetTimerManager().SetTimer(HealthRechargeTimerHandle, this, &AFPS_ProjectCharacter::HealthCanRecharge, HealthRechargeDelay, false);

	}
	
	RecentDamage += Amount;
	World->GetTimerManager().SetTimer(RecentDamageTimerHandle, this, &AFPS_ProjectCharacter::ResetRecentDamage, RecentDamageFadeTime, false);
	
	return false;

}

void AFPS_ProjectCharacter::HandleHealthForTick(float DeltaTime)
{
	// Recharge health if no damage taken recently
	if (bIsHealthRecharging)
	{
		CurrentHealth += (HealthRechargeRate * DeltaTime);

		if (CurrentHealth > MaxHealth)
		{
			CurrentHealth = MaxHealth;
		}

		if (CurrentHealth / MaxHealth >= LowHealthPercentage)
		{
			HideLowHealthWarning();
		}

	}

}

void AFPS_ProjectCharacter::ResetRecentDamage()
{
	FadingRecentDamage = RecentDamage;
	RecentDamage = 0;

}

void AFPS_ProjectCharacter::GameOver()
{
	APlayerController * PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController)
	{
		AMyPlayerController * MyPlayerController = Cast<AMyPlayerController>(PlayerController);
		if (MyPlayerController)
		{
			MyPlayerController->EnableMainMenuMode();
		}
	}

	/* Make red flashing low health widget invisible */
	HideLowHealthWarning();

	/* Make screen fade out. Make sure animation is set up in BP */
	if (BP_GameOverScreenColor)
	{
		APlayerController * PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if (PlayerController)
		{
			GameOverColorWidget = CreateWidget<UVer2_MouseoverDisplay>(PlayerController, BP_GameOverScreenColor);
			if (GameOverColorWidget)
			{
				GameOverColorWidget->AddToViewport(-1);
				if (GameOverColorWidget->Animation)
				{
					GameOverColorWidget->PlayAnimation(GameOverColorWidget->Animation, 0.f, 1.f);
				}
			}
		}
		
	}


	UWorld * World = GetWorld();
	if (World)
	{
		FTimerHandle DummyTimerHandle;
		World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AFPS_ProjectCharacter::ShowGameOverScreen, 2.5f, false);

	}

}

void AFPS_ProjectCharacter::ShowGameOverScreen()
{
	if (GameInstance)
	{
		GameInstance->ShowGameOverScreen();

		// Player controller does not get destroyed right. I don't think it does since widgets stay on viewport
		// Commented so widgets can be removed from viewport by referencing players widgets
		//Destroy();
	}

}

bool AFPS_ProjectCharacter::GetShowingCutscene()
{
	return bIsShowingCutscene;

}

void AFPS_ProjectCharacter::SetShowingCutscene(bool value)
{
	bIsShowingCutscene = value;

}

/** None of these sniper cutscene methods are called I think. They are handled by MyGameInstance instead */
void AFPS_ProjectCharacter::PlaySniperStartCutscene(AActor * StartViewTarget)
{
	if (bHasSniperCutscenePlayed)
	{
		return;
	}
	
	bHasSniperCutscenePlayed = true;

	SniperFireDuringCutscene();	// This is in the wrong place

	if (!PlayerController)
	{
		AController* Controller = GetController();
		if (Controller)
		{
			PlayerController = Cast<APlayerController>(Controller);
		}
	}

	if (PlayerController)
	{
		DisableInput(PlayerController);

		AActor* SniperCutsceneEndBlendTarget = PlayerController->GetViewTarget();

		PlayerController->SetViewTargetWithBlend(StartViewTarget, SniperCutsceneStartBlendTime);

		UWorld* World = GetWorld();
		if (World)
		{
			FTimerHandle DummyTimerHandle;
			World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AFPS_ProjectCharacter::ContinueSniperCutscene, SniperCutsceneStartBlendTime, false);

		}
	}
}

void AFPS_ProjectCharacter::ContinueSniperCutscene()
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (SniperCutsceneSequenceAsset && SequencePlayer == nullptr)
		{
			// To make this compile on 4.17 - a level sequence actor that is not used
			ALevelSequenceActor * Unused;
			
			SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(World, SniperCutsceneSequenceAsset, FMovieSceneSequencePlaybackSettings(), Unused);
			if (SequencePlayer)
			{
				float CutsceneLength = SequencePlayer->GetLength();
				
				SequencePlayer->SetPlaybackPosition(0.f);
				SequencePlayer->Play();

				FTimerHandle DummyTimerHandle;
				World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AFPS_ProjectCharacter::PlaySniperCutsceneEndBlend, CutsceneLength, false);

			}

		}

	}

}

void AFPS_ProjectCharacter::PlaySniperCutsceneEndBlend()
{
	if (SniperCutsceneEndBlendTarget && PlayerController)
	{
		PlayerController->SetViewTargetWithBlend(SniperCutsceneEndBlendTarget, SniperCutsceneEndBlendTime);
		
		TeleportTo(SniperCutsceneEndBlendTarget->GetActorLocation(), FRotator(0.f));

		PlayerController->SetControlRotation(SniperCutsceneEndBlendTarget->GetActorRotation());

		PlayerController->SetViewTarget(SniperCutsceneEndBlendTarget);

	}
	
	UWorld* World = GetWorld();
	if (World)
	{
		FTimerHandle DummyTimerHandle;
		World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AFPS_ProjectCharacter::EnablePlayerInput, SniperCutsceneEndBlendTime, false);

	}

	

}

void AFPS_ProjectCharacter::EnablePlayerInput()
{
	EnableInput(PlayerController);
	
}

void AFPS_ProjectCharacter::SniperFireDuringCutscene()
{
	UWorld* World = GetWorld();
	if (World && BP_SniperRifleScopeLossCameraShake && BP_SniperCutsceneProjectile)
	{
		UGameplayStatics::PlayWorldCameraShake(World, BP_SniperRifleScopeLossCameraShake, GetActorLocation(), 0.f, 200.f);

		World->SpawnActor<ASniperCutsceneProjectile>(BP_SniperCutsceneProjectile, GetActorLocation(), GetActorRotation());

	}

}

void AFPS_ProjectCharacter::AdjustForMotionControls()
{
	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}

}

void AFPS_ProjectCharacter::SetupWeaponValuesForBeginPlay()
{
	/* Spawning on a weapon can cause the game to crash */

	// This for loop prevents crashes at startup. i < x * 4, where x is the number of weapons
	// that exist in the game
	for (unsigned int i = 0; i < 6 * 4; i++)
	{
		WeaponAmmoValues.Insert(0, i);
		WeaponAmmoValues.Insert(0, i);
		WeaponAmmoValues.Insert(0, i);
		WeaponAmmoValues.Insert(0, i);
	}

	/* Input all the max ammo and inital values for all the weapons */

	// laser rifle
	AdjustCurrentClipAmmo(0, 8);
	AdjustCurrentNonClipAmmo(0, 32);
	AdjustMaxClipAmmo(0, 8);
	AdjustMaxNonClipAmmo(0, 32);

	// Auto rifle
	AdjustCurrentClipAmmo(1, 0);
	AdjustCurrentNonClipAmmo(1, 0);
	AdjustMaxClipAmmo(1, 30);
	AdjustMaxNonClipAmmo(1, 120);

	// Grenade launcher
	AdjustCurrentClipAmmo(2, 0);
	AdjustCurrentNonClipAmmo(2, 0);
	AdjustMaxClipAmmo(2, 2);
	AdjustMaxNonClipAmmo(2, 12);

	// Sniper rifle
	AdjustCurrentClipAmmo(3, 0);
	AdjustCurrentNonClipAmmo(3, 0);
	AdjustMaxClipAmmo(3, 5);
	AdjustMaxNonClipAmmo(3, 20);


	// Setup rates of fire and automatic status
	RatesOfFire.Insert(0.8f, 0);
	RatesOfFire.Insert(0.167f, 1);
	RatesOfFire.Insert(1.2f, 2);
	RatesOfFire.Insert(0.6f, 3);
	
	AutomaticStatus.Insert(false, 0);
	AutomaticStatus.Insert(true, 1);
	AutomaticStatus.Insert(false, 2);
	AutomaticStatus.Insert(false, 3);

	// Setup camera shakes. I think if one of these is nullptr then when trying to insert one after
	// it will cause an error because the array knows the index before is null and would like the 
	// shake to be inserted there instead, so this has been commened out and the shakes can
	// be added manually in the editor
	//if (BP_LaserRifleCameraShake)
	//{
	//	WeaponCameraShakes.Insert(BP_LaserRifleCameraShake, 0);
	//}
	//if (BP_AutoRifleCameraShake)
	//{
	//	WeaponCameraShakes.Insert(BP_AutoRifleCameraShake, 1);
	//}
	//if (BP_GrenadeLauncherCameraShake)
	//{
	//	WeaponCameraShakes.Insert(BP_GrenadeLauncherCameraShake, 2);
	//}
	//if (BP_SniperRifleCameraShake)
	//{
	//	WeaponCameraShakes.Insert(BP_SniperRifleCameraShake, 3);
	//}

	// Set up the weapon values for the weapon with WeaponID of 0.
	// This is the weapon the player starts with
	EquippedWeapon = 0;
	WeaponRateOfFire = RatesOfFire[EquippedWeapon];
	bEquippedWeaponIsAutomatic = AutomaticStatus[EquippedWeapon];
	ChangeWeaponMesh(EquippedWeapon);

}

void AFPS_ProjectCharacter::AttachWeaponMeshesForBeginPlay()
{
	Mesh1POriginalLocation = Mesh1P->RelativeLocation;
	Mesh1POriginalRotation = Mesh1P->RelativeRotation;
	
	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	FP_Gun->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));	// This is only for the first person shooter SK
	FP_Gun->SetRelativeLocation(FVector(-1.5f, 3.f, 0.f));	// This is only for the first person shooter SK

	// This was the only way I could get weapon switching to work: by attaching all the skeletal meshs to the 
	// character in BeginPlay and changing their visibility when they are picked up. Trying to attach a 
	// skeletal mesh while playing would not work.
	FP_GunAutoRifle->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	FP_GunAutoRifle->SetHiddenInGame(true);
	FP_GunAutoRifle->SetVisibility(false);
	FP_GunAutoRifle->SetRelativeScale3D(FVector(1.25f, 1.25f, 1.25f));
	FP_GunAutoRifle->SetRelativeLocation(FVector(0.f, 0.f, -3.5f));
	FP_GunAutoRifle->SetRelativeRotation(FRotator(0.f, 0.f, -85.f));

	FP_GunGrenadeLauncher->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	FP_GunGrenadeLauncher->SetHiddenInGame(true);
	FP_GunGrenadeLauncher->SetVisibility(false);
	FP_GunGrenadeLauncher->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
	FP_GunGrenadeLauncher->SetRelativeLocation(FVector(0.f, 5.f, 0.f));
	FP_GunGrenadeLauncher->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));

	FP_GunSniperRifle->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	FP_GunSniperRifle->SetHiddenInGame(true);
	FP_GunSniperRifle->SetVisibility(false);
	FP_GunSniperRifle->SetRelativeScale3D(FVector(1.2f, 1.2f, 1.2f));
	FP_GunSniperRifle->SetRelativeLocation(FVector(0.f, -5.f, 27.f));
	FP_GunSniperRifle->SetRelativeRotation(FRotator(180.f, 0.f, -90.f));

}

void AFPS_ProjectCharacter::ChangeWeaponMesh(uint8 Type)
{
	ToggleSniperScope(false);
	
	// Make old weapon invisible
	switch (OtherWeapon)
	{
	case 0:
		FP_Gun->SetHiddenInGame(true);
		FP_Gun->SetVisibility(false);
		break;

	case 1:
		FP_GunAutoRifle->SetHiddenInGame(true);
		FP_GunAutoRifle->SetVisibility(false);
		break;

	case 2:
		FP_GunGrenadeLauncher->SetHiddenInGame(true);
		FP_GunGrenadeLauncher->SetVisibility(false);
		break;

	case 3:
		FP_GunSniperRifle->SetHiddenInGame(true);
		FP_GunSniperRifle->SetVisibility(false);
		break;

	default:
		break;
	}
	
	// Make new weapon visible
	switch (Type)
	{
	case 0:
		FP_Gun->SetHiddenInGame(false);
		FP_Gun->SetVisibility(true);
		break;

	case 1:
		FP_GunAutoRifle->SetHiddenInGame(false);
		FP_GunAutoRifle->SetVisibility(true);
		break;

	case 2:
		FP_GunGrenadeLauncher->SetHiddenInGame(false);
		FP_GunGrenadeLauncher->SetVisibility(true);
		break;

	case 3:
		FP_GunSniperRifle->SetHiddenInGame(false);
		FP_GunSniperRifle->SetVisibility(true);
		break;

	default:
		break;
	}

	// Also change muzzle location
	ChangeMuzzleLocation(Type);

}

void AFPS_ProjectCharacter::PlaySoundAtRandom(TArray<class USoundBase*> & Sounds)
{
	UMyStatics::PlaySound(this, Sounds, GetActorLocation(), 0.30f);

}

void AFPS_ProjectCharacter::SwitchWeapons()
{
	if (!bOkToSwitchWeapons)
	{
		return;
	}

	// If no other weapon
	if (OtherWeapon == 255)
	{
		if (HUDText != FString("No other weapon in inventory"))
		{
			FString Message = FString("No other weapon in inventory");

			CreateHUDMessage(Message, true, true);

			FTimerHandle TextTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TextTimerHandle, this, &AFPS_ProjectCharacter::ClearHUDText, 3.f, false);
		}

		return;
	}

	// If here then there is another weapon and a switch will take place
	bOkToSwitchWeapons = false;
	
	if (bIsReloading)
	{
		bHasRecentlySwitchedWeapons = true;

		// Switching weapons causes a reload to not finish
		bIsReloading = false;
	}

	//GetWorldTimerManager().ClearTimer(ReloadDelayTimerHandle);

	// Stop reload animation from playing
	// param1: Blend out time
	if (ReloadAnimInstance)
	{
		ReloadAnimInstance->Montage_Stop(0.15f);
	}

	PlayWeaponSwitchAnimation();

	ClearHUDText();

	bCanFireAfterWeaponSwitch = false;

	uint8 Temp = OtherWeapon;
	OtherWeapon = EquippedWeapon;
	EquippedWeapon = Temp;

	// Update the aim animation info
	AssignAimAnimsOnWeaponChange(EquippedWeapon);

	// Change rate of fire, automatic status and weapon mesh
	WeaponRateOfFire = RatesOfFire[EquippedWeapon];
	bEquippedWeaponIsAutomatic = AutomaticStatus[EquippedWeapon];
	ChangeWeaponMesh(EquippedWeapon);

	// Make sure delay between switching weapons is enforced
	GetWorld()->GetTimerManager().SetTimer(SwitchWeaponTimerHandle, this, &AFPS_ProjectCharacter::MakeOkToSwitchWeaponsTrue, WeaponSwitchCooldown, false);

	// Allow firing after weapon switch
	GetWorld()->GetTimerManager().SetTimer(FireAfterWeaponSwitchTimerHandle, this, &AFPS_ProjectCharacter::MakeFiringAfterSwitchWeaponsOk, WeaponSwitchFiringTime, false);

}

void AFPS_ProjectCharacter::PlayWeaponSwitchAnimation(float Playrate)
{
	// Play weapon switch animation
	if (Mesh1P)
	{
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (OtherWeapon == 2 && GrenadeEquipAnimation && AnimInstance)
		{
			AnimInstance->Montage_Play(GrenadeEquipAnimation, 0.5f * Playrate);
		}
		else if ((OtherWeapon == 0 || OtherWeapon == 1 || OtherWeapon == 3) && RifleEquipAnimation && AnimInstance)
		{
			AnimInstance->Montage_Play(RifleEquipAnimation, 0.5f * Playrate);
		}
	}

}

int AFPS_ProjectCharacter::GetCurrentClipAmmo(uint8 WeaponID)
{
	if (WeaponAmmoValues.IsValidIndex(WeaponID * 4))
	{
		return WeaponAmmoValues[WeaponID * 4];
	}
	
	return -1;
	
}

int AFPS_ProjectCharacter::GetCurrentNonClipAmmo(uint8 WeaponID)
{
	if (WeaponAmmoValues.IsValidIndex(WeaponID * 4 + 1))
	{
		return WeaponAmmoValues[(WeaponID * 4) + 1];
	}

	return -1;
}

int AFPS_ProjectCharacter::GetMaxClipAmmo(uint8 WeaponID)
{
	if (WeaponAmmoValues.IsValidIndex(WeaponID * 4 + 2))
	{
		return WeaponAmmoValues[(WeaponID * 4) + 2];
	}

	return -1;
}

int AFPS_ProjectCharacter::GetMaxNonClipAmmo(uint8 WeaponID)
{
	if (WeaponAmmoValues.IsValidIndex(WeaponID * 4 + 3))
	{
		return WeaponAmmoValues[(WeaponID * 4) + 3];
	}

	return -1;
}


/* Not sure yet what happens with these 4 setter functions if there is
 * no entry with WeaponID already in the map 
*/
void AFPS_ProjectCharacter::AdjustCurrentClipAmmo(uint8 WeaponID, int Amount)
{
	WeaponAmmoValues[WeaponID * 4] += Amount;

}

void AFPS_ProjectCharacter::AdjustCurrentNonClipAmmo(uint8 WeaponID, int Amount)
{
	WeaponAmmoValues[WeaponID * 4 + 1] += Amount;

}

void AFPS_ProjectCharacter::AdjustMaxClipAmmo(uint8 WeaponID, int Amount)
{
	WeaponAmmoValues[WeaponID * 4 + 2] += Amount;

}

void AFPS_ProjectCharacter::AdjustMaxNonClipAmmo(uint8 WeaponID, int Amount)
{
	WeaponAmmoValues[WeaponID * 4 + 3] += Amount;

}

/* There is a bug where if the clip is completely used, then switch weapons
   the weapon will not fire. Not sure if the issue is in this function 
   or somewhere else */
void AFPS_ProjectCharacter::Reload()
{
	// Only dealing with EquippedWeapon in here so save some typing.
	uint8 e = EquippedWeapon;
	
	// If already have a full clip
	if (GetCurrentClipAmmo(e) == GetMaxClipAmmo(e))
	{
		return;
	}

	// If there's no non clip ammo
	if (GetCurrentNonClipAmmo(e) == 0)
	{
		return;
	}

	// If already reloading
	if (bIsReloading)
	{
		return;
	}
	
	// If here then a reload will happen

	if (EquippedWeapon == 3)
	{
		ToggleSniperScope(false);
	}

	// Make a 2.1 second reload delay, then update ammo info
	StartReloading();
	
	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		World->GetTimerManager().SetTimer(ReloadDelayTimerHandle, this, &AFPS_ProjectCharacter::StopReloading, 2.10f, false);
	}
}

void AFPS_ProjectCharacter::StartReloading()
{
	bIsReloading = true;

	// Play reload animation. Different animations for different weapons
	if (EquippedWeapon != 2)
	{
		if (AutoRifleReloadAnimation != nullptr)
		{
			// How does this know to get the right reload animation?
			ReloadAnimInstance = Mesh1P->GetAnimInstance();
			
			ReloadAnimInstance->Montage_Play(AutoRifleReloadAnimation, 0.5555f);
		}
	}
	else
	{
		if (GrenadeLauncherReloadAnimation != nullptr)
		{
			// How does this know to get the right reload animation?
			ReloadAnimInstance = Mesh1P->GetAnimInstance();
			
			ReloadAnimInstance->Montage_Play(GrenadeLauncherReloadAnimation, 1.f);
		}
	}

}

void AFPS_ProjectCharacter::StopReloading()
{
	//GetWorldTimerManager().ClearTimer(ReloadDelayTimerHandle);
	
	// If a weapon switch happened while reloading then cancel the reload
	if (bHasRecentlySwitchedWeapons)
	{
		bHasRecentlySwitchedWeapons = false;
		
		bCanFire = true;
		bIsReloading = false;

		return;
	}

	// Only dealing with EquippedWeapon in here so save some typing
	uint8 e = EquippedWeapon;
	
	int AmmoToReload = GetMaxClipAmmo(e) - GetCurrentClipAmmo(e);

	// If the reload will make the non clip ammo equal to 0
	if (AmmoToReload > GetCurrentNonClipAmmo(e))
	{
		
		AdjustCurrentClipAmmo(e, GetCurrentNonClipAmmo(e));
		AdjustCurrentNonClipAmmo(e, -GetCurrentNonClipAmmo(e));
	}
	// If the reload will still leave some non clip ammo remaining
	else
	{
		
		AdjustCurrentClipAmmo(e, AmmoToReload);
		AdjustCurrentNonClipAmmo(e, -AmmoToReload);
	}
	
	// need this here to continue firing after emptying clip.
	bCanFire = true;	
	
	bIsReloading = false;

	// If the left mouse button is pressed after reloading and the weapon
	// is automatic then keep firing
	if (bEquippedWeaponIsAutomatic && bHoldingTrigger)
	{
		OnFire();
	}

	// If RMB is pressed when reload has finished then aim
	if (bWantsToAim)
	{
		RMBPressed();
	}

}

void AFPS_ProjectCharacter::ReleasedTrigger()
{
	bHoldingTrigger = false;

}

void AFPS_ProjectCharacter::RaycastForMouseoverDisplaysOnTick(float DeltaTime)
{
	// This is semi straining on CPU. When playing in editor I can notice a difference in smoothness
	// when turning
	// Called each tick this will do
	// - Cast to an interface
	// - Iterate twice over an array of all enemy characters. Could possibly reduce that to only one
	//

	FHitResult* HitResult = new FHitResult();
	const FVector StartTrace = FirstPersonCameraComponent->GetComponentLocation();
	const FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	const FVector EndTrace = (ForwardVector * 6000.f) + StartTrace; 
	FCollisionQueryParams* CQP = new FCollisionQueryParams();
	CQP->AddIgnoredActor(this);

	UWorld* World = GetWorld();

	// Now declared as a field 
	//ACharacter * Bot = nullptr;
	Bot = nullptr;

	/* Check if ray cast was successful. The SingleByChannel means it will stop at the first thing it hits I think */
	if (!bIsUsingCameraRobot && World && World->LineTraceSingleByChannel(*HitResult, StartTrace, EndTrace, ECC_Visibility, *CQP))
	{
		// ray cast was successful

		if (HitResult->GetActor() != nullptr && HitResult->GetActor()->IsA(ACharacter::StaticClass()))
		{
			
			Bot = Cast<ACharacter>(HitResult->GetActor());

			if (Bot)
			{

				int32 Index = MouseoverBots.Find(Bot);

				if (Index == INDEX_NONE)
				{
					/* Bot not already in array */
					
					int32 Index = MouseoverBots.Add(Bot);
					MouseoverBotsMouseoverTime.Insert(DeltaTime, Index);
					MouseoverBotsFadeTime.Insert(MouseoverDisplayFadeOutTime, Index);

				}
				else
				{
					/* Bot already in array */
					
					float CurrentMouseoverTime = MouseoverBotsMouseoverTime[Index] + DeltaTime;

					MouseoverBotsFadeTime[Index] = MouseoverDisplayFadeOutTime;
					
					if (CurrentMouseoverTime >= MouseoverDisplayTime)
					{
						CurrentMouseoverTime = MouseoverDisplayTime;
						
						// Start fading in mouseover display
						IDamagableBot* AsDamagable = Cast<IDamagableBot>(Bot);
						if (AsDamagable)
						{
							AsDamagable->FadeMouseoverDisplay(DeltaTime);


						}
							
					}

					MouseoverBotsMouseoverTime.Insert(CurrentMouseoverTime, Index);

				}
				
			}

		}

	}

	// Fade out each character in mouseover bots array that isn't Bot
	for (int32 i = 0; i < MouseoverBots.Num(); i++)
	{
		if (MouseoverBots[i] != Bot)
		{
			// Reduce mouseover time
			MouseoverBotsFadeTime[i] -= DeltaTime;
			if (MouseoverBotsFadeTime[i] <= 0.f)
			{
				MouseoverBotsFadeTime[i] = 0.f;

				// Fade mouseover display
				IDamagableBot* AsDamagable = Cast<IDamagableBot>(MouseoverBots[i]);
				if (AsDamagable)
				{
					AsDamagable->FadeMouseoverDisplay(-DeltaTime);

				}

				// Start reducing mouseover time
				MouseoverBotsMouseoverTime[i] -= (DeltaTime / 2);
				if (MouseoverBotsMouseoverTime[i] < 0.f)
				{
					MouseoverBotsMouseoverTime[i] = 0.f;
				}

			}
		}
	}

}
