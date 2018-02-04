// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "AIPatrolController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/PawnSensingComponent.h"
#include "AIPatrol.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "Ver2_MouseoverDisplay.h"


// Sets default values
AAIPatrol::AAIPatrol()
{
	// Initialize senses
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	PawnSensingComp->SetPeripheralVisionAngle(90.f);

	PS_Bot = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Explosion Particles"));
	PS_Bot->AttachTo(RootComponent);
	PS_Bot->SetVisibility(false);
	PS_Bot->bGenerateOverlapEvents = false;

	PS_Bot_Fuse = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Fuse Particles"));
	PS_Bot_Fuse->AttachTo(RootComponent);
	PS_Bot_Fuse->SetVisibility(false);
	PS_Bot_Fuse->bGenerateOverlapEvents = false;

	RF_Bot = CreateDefaultSubobject<URadialForceComponent>(TEXT("Explosion force"));
	RF_Bot->Radius = 1000.0f;
	RF_Bot->DestructibleDamage = 1.0f;
	RF_Bot->bIgnoreOwningActor = true;
	RF_Bot->ImpulseStrength = 40000.f;
	RF_Bot->AttachTo(RootComponent);

	Sphere_Bot = CreateDefaultSubobject<USphereComponent>(TEXT("Explosion hit sphere"));
	Sphere_Bot->bGenerateOverlapEvents = false;
	Sphere_Bot->SetSphereRadius(RF_Bot->Radius);
	Sphere_Bot->AttachTo(RootComponent);

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

	RunSpeed = 400.f;
	DetonationDistance = 250.f;
	MaxHealth = 200.f;

	OverheadIconText = FString("");

}

void AAIPatrol::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!MyPlayer)
	{
		
		ACharacter * PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
		if (PlayerCharacter)
		{
			MyPlayer = Cast<AFPS_ProjectCharacter>(PlayerCharacter);
		}
		
	}
	
	if (MyPlayer)
	{	
		if (bIsChasingPlayer && (GetActorLocation() - MyPlayer->GetActorLocation()).Size() < DetonationDistance)
		{
			Detonate();
		}
	}

	UpdateMouseoverDisplayPosition();

	UpdateOverheadIconLocation();

	UpdateOverheadIconRotation();

}

// Called when the game starts or when spawned
void AAIPatrol::BeginPlay()
{
	Super::BeginPlay();
	
	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &AAIPatrol::OnPlayerCaught);
	}

	MyPlayer = Cast<AFPS_ProjectCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	// Attach fuse PS to bomb socket
	if (PS_Bot_Fuse)
	{
		PS_Bot_Fuse->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("b_MF_BombSocket"));
		PS_Bot_Fuse->SetRelativeLocation(FVector(-13.f, 22.f, 45.f));
	}

	GetMesh()->bGenerateOverlapEvents = true;

	SetupMouseoverInfoBoxForBeginPlay();

	SetupOverheadIconForBeginPlay();

	Health = MaxHealth;

}

bool AAIPatrol::ReceiveDamage(float Amount)
{
	Health -= Amount;

	if (Health <= 0.f)
	{
		if (bZeroHealth)
		{
			return false;
		}
		
		bZeroHealth = true;

		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Reduce bot's speed to 0
		AController* Controller = GetController();
		if (Controller)
		{
			Controller->StopMovement();
		}
		
		if (bIsChasingPlayer)
		{
			// Make the bomb detonate 3 seconds after the bot has lost all its health
			FTimerHandle BombDetonationTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(BombDetonationTimerHandle, this, &AAIPatrol::Deestroy, 3.0f, false);

		}

		// Hide overhead icon
		if (OverheadIconUserWidget)
		{
			OverheadIconUserWidget->SetVisibility(ESlateVisibility::Hidden);
		}

		// For bunker event
		if (MyPlayer)
		{
			MyPlayer->IncrementNumBotsDefeated();

			UWorld * World = GetWorld();
			if (World)
			{
				FTimerHandle DummyTimerHandle;
				World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AAIPatrol::RemoveFromMouseoverArrays, 3.f, false);

			}

		}

	}
	else
	{
		// Bot still has health

		UWorld * World = GetWorld();
		if (World)
		{
			FTimerHandle DummyTimerHandle;
			World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AAIPatrol::OnHit, FMath::RandRange(0.2f, 0.4f), false);

		}

	}

	return false;
}

bool AAIPatrol::FadeMouseoverDisplay(float DeltaTime)
{
	if (MouseoverInfoBox)
	{

		// Change alpha value
		UUserWidget* Widget = MouseoverInfoBox->GetUserWidgetObject();

		if (Widget)
		{
			if (bZeroHealth)
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

// Called to bind functionality to input
void AAIPatrol::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAIPatrol::StartRunning()
{
	bIsChasingPlayer = true;

	// Light fuse on bomb
	if (PS_Bot_Fuse)
	{
		PS_Bot_Fuse->ActivateSystem(true);
		PS_Bot_Fuse->SetVisibility(true);
	}
	
	// Change bot's movement speed to a running speed
	UCharacterMovementComponent* Movement = Cast<UCharacterMovementComponent>(GetMovementComponent());
	
	if (Movement)
	{
		Movement->MaxWalkSpeed = RunSpeed;
	}
	
}

void AAIPatrol::Detonate()
{
	bIsDetonating = true;

	// Reduce bot's speed to 0
	UCharacterMovementComponent* Movement = Cast<UCharacterMovementComponent>(GetMovementComponent());
	Movement->MaxWalkSpeed = 0.f;

	FTimerHandle DestroyTimerHandle;
	// Currently set at 1.3 seconds
	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &AAIPatrol::Deestroy, 1.3f, false);

}

void AAIPatrol::RemoveFromMouseoverArrays()
{
	// Remove from players mouseover arrays
	if (MyPlayer)
	{
		int32 Index = MyPlayer->MouseoverBots.Find(this);
		MyPlayer->MouseoverBots.RemoveAt(Index);
		MyPlayer->MouseoverBotsMouseoverTime.RemoveAt(Index);
		MyPlayer->MouseoverBotsFadeTime.RemoveAt(Index);
	}

}

void AAIPatrol::OnPlayerCaught(APawn * Pawn)
{
	if (Pawn->IsA(AFPS_ProjectCharacter::StaticClass()))
	{
		// Get a reference to the player controller
		AAIPatrolController* AIController = Cast<AAIPatrolController>(GetController());

		if (!bHasSpottedPlayer)
		{
			bHasSpottedPlayer = true;

			// Reduce bot's speed to 0
			UCharacterMovementComponent* Movement = Cast<UCharacterMovementComponent>(GetMovementComponent());
			if (Movement)
			{
				Movement->MaxWalkSpeed = 0.f;
			}

			FTimerHandle StartRunningTimerHandle;
			// Currently set at 3 seconds
			GetWorld()->GetTimerManager().SetTimer(StartRunningTimerHandle, this, &AAIPatrol::StartRunning, 1.8f, false);

			ShowExclamationPointIcon();

		}



		if (AIController)
		{
			AIController->SetPlayerCaught(Pawn);
		}
	}
	
}

bool AAIPatrol::GoAlert()
{
	if (!bHasSpottedPlayer)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0);
			if (Character)
			{
				OnPlayerCaught(Character);
				
			}
		}
		
	}
	
	return false;
}

void AAIPatrol::OnHit()
{
	if (MyPlayer && !bHasSpottedPlayer)
	{
		float Distance = (GetActorLocation() - MyPlayer->GetActorLocation()).SizeSquared();
		if (Distance < 5000.f * 5000.f)
		{
			GoAlert();
		}
	}
	
}

void AAIPatrol::Deestroy()
{
	
	if (bHasCalledDeestroy)
	{
		return;
	}

	bHasCalledDeestroy = true;

	// Would really just like to delete the mesh entirly
	USkeletalMeshComponent* CharacterMesh = GetMesh();
	if (CharacterMesh)
	{
		CharacterMesh->SetVisibility(false);
		CharacterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Make fuse particle system invisible
	PS_Bot_Fuse->SetVisibility(false);

	// Create explosion particle system
	if (PS_Bot)
	{
		PS_Bot->ActivateSystem(true);
		PS_Bot->SetVisibility(true);
	}

	// Create explosion force
	if (RF_Bot)
	{
		// Fire impulse twice to cause destructible meshes to explode
		RF_Bot->FireImpulse();
		RF_Bot->FireImpulse();
	}

	TArray<AActor*> ActorsHitByBlast;
	UWorld* World = GetWorld();
	if (World)
	{
		ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0);
		if (Character)
		{
			AFPS_ProjectCharacter* Player = Cast < AFPS_ProjectCharacter>(Character);
			if (Player)
			{
				// Deal damage to player. Max damage at < 200 units away
				float Distance = (Player->GetActorLocation() - GetActorLocation()).Size();
				if (Distance <= 1000.f)
				{
					if (Distance <= 200.f)
					{
						Distance = 0.f;
					}
					
					float Damage = 99.f - (Distance * 0.08f);

					Player->ReceiveDamage(Damage);

					// For bunker event
					if (Player)
					{
						Player->IncrementNumBotsDefeated();
					}

				}
			}
		}

	}
	
	// Add actors to array to be delt damage during tick
	//if (Sphere_Bot)
	//{
	//	Sphere_Bot->bGenerateOverlapEvents = true;
	//	Sphere_Bot->GetOverlappingActors(HitTargets);
	//}
	//



	// Deal damage to actors
	//for (AActor* HitTarget : HitTargets)
	//{
	//	// Deal damage to player
	//	IDamagable* DamagableTarget = Cast<IDamagable>(HitTarget);
	//
	//	if (DamagableTarget)
	//	{
	//		// Deal damage to every damagable actor close to the blast.
	//		float Distance = (GetActorLocation() - HitTarget->GetActorLocation()).Size();
	//
	//		
	//
	//		if (Distance < RF_Bot->Radius / 4)
	//		{
	//			DamagableTarget->ReceiveDamage(104.f);
	//		}
	//		else if (Distance < RF_Bot->Radius / 2)
	//		{
	//			DamagableTarget->ReceiveDamage(72.f);
	//		}
	//		else if (Distance < RF_Bot->Radius / 1.3333f)
	//		{
	//			DamagableTarget->ReceiveDamage(45.f);
	//		}
	//		else if (Distance < RF_Bot->Radius)
	//		{
	//			DamagableTarget->ReceiveDamage(20.f);
	//		}
	//
	//		continue;
	//
	//	}
	//
	//	// Deal damage to bots
	//	IDamagableBot* DamagableBot = Cast<IDamagableBot>(HitTarget);
	//
	//	if (DamagableBot)
	//	{
	//		// Deal damage to every damagable actor close to the blast.
	//		float Distance = (GetActorLocation() - HitTarget->GetActorLocation()).Size();
	//
	//		
	//
	//		if (Distance < RF_Bot->Radius / 4)
	//		{
	//			DamagableBot->ReceiveDamage(104.f);
	//		}
	//		else if (Distance < RF_Bot->Radius / 2)
	//		{
	//			DamagableBot->ReceiveDamage(72.f);
	//		}
	//		else if (Distance < RF_Bot->Radius / 1.3333f)
	//		{
	//			DamagableBot->ReceiveDamage(45.f);
	//		}
	//		else if (Distance < RF_Bot->Radius)
	//		{
	//			DamagableBot->ReceiveDamage(20.f);
	//		}
	//	}
	//
	//}
	//
	//if (Sphere_Bot)
	//{
	//	Sphere_Bot->bGenerateOverlapEvents = false;
	//}
	
	// Destroy actor after 5 seconds to allow particle system to still display
	FTimerHandle DestroyTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &AAIPatrol::DestroyActor, 5.f, false);
	

}

void AAIPatrol::DestroyActor()
{
	Destroy();

}

void AAIPatrol::SetupMouseoverInfoBoxForBeginPlay()
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
				MouseoverDisplay->OwningAIPatrolBot = this;
			}


			// Change widget opacity
			FLinearColor Color = Widget->ColorAndOpacity;

			// This line may not be needed
			Widget->SetVisibility(ESlateVisibility::Visible);

			Widget->SetColorAndOpacity(FLinearColor(Color.R, Color.G, Color.B, 0.f));

		}

	}

}

void AAIPatrol::UpdateMouseoverDisplayPosition()
{
	if (MouseoverInfoBox)
	{
		MouseoverInfoBox->SetWorldLocation(GetActorLocation() + MouseoverBoxOffset);
	}

}

void AAIPatrol::MakeMouseoverDisplayInvisible()
{
}

void AAIPatrol::ShowExclamationPointIcon()
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
			World->GetTimerManager().SetTimer(DummyTimerHandle, this, &AAIPatrol::FadeOutOverheadExclamationPoint, 1.0f, false);

		}

	}

}

void AAIPatrol::FadeOutOverheadExclamationPoint()
{
	// Create a animation in editor, make sure widgets parent class is the widget class I created
	// and play that animation. 

	if (OverheadIconUserWidget && OverheadIconUserWidget->Animation)
	{
		OverheadIconUserWidget->PlayAnimation(OverheadIconUserWidget->Animation, 0.f, 1);
	}

}

void AAIPatrol::UpdateOverheadIconLocation()
{
	if (OverheadIconWidget)
	{
		OverheadIconWidget->SetWorldLocation(GetActorLocation() + OverheadIconOffset);
	}

}

void AAIPatrol::SetupOverheadIconForBeginPlay()
{
	if (OverheadIconWidget)
	{
		UUserWidget* Widget = OverheadIconWidget->GetUserWidgetObject();

		if (Widget)
		{

			OverheadIconUserWidget = Cast<UVer2_MouseoverDisplay>(Widget);

			if (OverheadIconUserWidget)
			{
				OverheadIconUserWidget->OwningAIPatrolBot = this;

			}

		}

	}

}

void AAIPatrol::UpdateOverheadIconRotation()
{
	if (MyPlayer)
	{
		FVector BotToPlayer = MyPlayer->GetActorLocation() - GetActorLocation();

		FRotator NewRotation = FRotationMatrix::MakeFromX(BotToPlayer).Rotator();

		if (OverheadIconWidget)
		{
			OverheadIconWidget->SetWorldRotation(NewRotation);
		}

	}

}

