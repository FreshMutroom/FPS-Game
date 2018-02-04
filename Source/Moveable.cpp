// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "Moveable.h"
#include "TutorialMessage.h"


// Sets default values
AMoveable::AMoveable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SM_Moveable = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Moveable Mesh"));
	RootComponent = SM_Moveable;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	CollisionBox->bGenerateOverlapEvents = true;
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AMoveable::TriggerEnter);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &AMoveable::TriggerExit);
	CollisionBox->SetAbsolute(true, true, true);
	CollisionBox->SetupAttachment(RootComponent);

	bPlayerIsWithinRange = false;
	bIsAlreadyMoving = false;
	bAlwaysTriggers = false;

}

// Called when the game starts or when spawned
void AMoveable::BeginPlay()
{
	Super::BeginPlay();
	
	// Get the current player
	CurrentPlayerController = Cast<AFPS_ProjectCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	// Check if animation curve is set up in editor (I think)
	if (AnimCurve)
	{
		FOnTimelineFloat ProgressFunction;

		ProgressFunction.BindUFunction(this, FName("HandleProgress"));

		MyTimeline.AddInterpFloat(AnimCurve, ProgressFunction);
		MyTimeline.SetLooping(false);


		if (SM_Moveable)
		{
			ActorInitialLocation = TargetLocation = GetActorLocation();
		}

		if (MovementAxis == "X")
		{
			TargetLocation.X += MaxMovement;
		}
		else if (MovementAxis == "Y")
		{
			TargetLocation.Y += MaxMovement;
		}
		else if (MovementAxis == "Z")
		{
			TargetLocation.Z += MaxMovement;
		}

	}

	if (bMovesWhenSpawned)
	{
		MyTimeline.PlayFromStart();
	}

}

// Called every frame
void AMoveable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CurrentPlayerController)
	{
		UWorld * World = GetWorld();
		if (World)
		{
			ACharacter * PlayerCharacter = UGameplayStatics::GetPlayerCharacter(World, 0);
			if (PlayerCharacter)
			{
				CurrentPlayerController = Cast<AFPS_ProjectCharacter>(PlayerCharacter);
			}
		}
	}
	
	if (bAlwaysTriggers && bPlayerIsWithinRange && !bIsAlreadyMoving)
	{
		Open();
	}

	else if (CurrentPlayerController && CurrentPlayerController->bIsInteracting && !bIsAlreadyMoving && CurrentPlayerController->bHasKeyCard && bPlayerIsWithinRange)
	{
		Open();
	}
	

	MyTimeline.TickTimeline(DeltaTime);

	if (!MyTimeline.IsPlaying())
	{
		bIsAlreadyMoving = false;
	}
}

void AMoveable::RemoveOnScreenText()
{
	if (CurrentPlayerController)
	{
		if (CurrentPlayerController->HUDText == OnScreenMessage || CurrentPlayerController->HUDText == OnScreenErrorMessage)
		{
			CurrentPlayerController->ClearHUDText();
		}
	}

}

void AMoveable::CreateTypewriterMessage()
{
	if (BP_TypewriterMessage)
	{
		UWorld * World = GetWorld();
		if (World)
		{
			ATutorialMessage * Message = World->SpawnActor<ATutorialMessage>(BP_TypewriterMessage, FVector(0.f, 0.f, 10000.f), FRotator(0.f));
			if (Message)
			{
				Message->CreateMessage();
			}
		}
	}

}

void AMoveable::TriggerEnter(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(AFPS_ProjectCharacter::StaticClass()))
	{
		if (bChecksForComputerEvent)
		{
			if (CurrentPlayerController && CurrentPlayerController->bHasAccessedComputer)
			{
				bPlayerIsWithinRange = true;
			}
		}
		else if (bChecksForBunkerEvent)
		{
			if (CurrentPlayerController && CurrentPlayerController->bHasDefendedBunker)
			{
				bPlayerIsWithinRange = true;
			}
		}
		else
		{
			bPlayerIsWithinRange = true;
		}

		

		if (CurrentPlayerController)
		{
			if (bShowsHUDMessage)
			{
				if (!CurrentPlayerController->bHasKeyCard)
				{
					// Player does not have keycard

					CurrentPlayerController->HUDText = OnScreenErrorMessage;

					if (MessageDisplayTime != 0.0f)
					{
						FTimerHandle DummyHandle;
						GetWorld()->GetTimerManager().SetTimer(DummyHandle, this, &AMoveable::RemoveOnScreenText, MessageDisplayTime, false);
					}

				}
				else
				{
					// Player has keycard

					CurrentPlayerController->HUDText = OnScreenMessage;

				}
			}

		}

	}

}

void AMoveable::TriggerExit(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(AFPS_ProjectCharacter::StaticClass()))
	{
		bPlayerIsWithinRange = false;
	}
	
	if (MessageDisplayTime == 0.0f && bShowsHUDMessage)
	{
		RemoveOnScreenText();
	}

}

void AMoveable::Open()
{
	if (!bCannotMoveAgain)
	{
		bIsAlreadyMoving = true;

		MyTimeline.PlayFromStart();

		CreateTypewriterMessage();

		if (bTriggersOnce)
		{
			// Don't really need this variable because of the line after
			bCannotMoveAgain = true;
			
			if (CollisionBox)
			{
				CollisionBox->bGenerateOverlapEvents = false;
			}
			
		}
	}

}

void AMoveable::HandleProgress(float Value)
{
	FVector NewLocation = FMath::Lerp(ActorInitialLocation, TargetLocation, Value);
	
	SetActorLocation(NewLocation);

}

