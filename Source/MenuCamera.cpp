// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "MenuCamera.h"


// Sets default values
AMenuCamera::AMenuCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Camera mesh"));
	CameraMesh->bGenerateOverlapEvents = false;
	CameraMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CameraMesh->SetHiddenInGame(true);
	RootComponent = CameraMesh;

	StartLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Starting location"));
	StartLocation->SetAbsolute(true, true, true);
	StartLocation->SetHiddenInGame(true);
	StartLocation->SetupAttachment(RootComponent);

	EndLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Target location"));
	EndLocation->SetAbsolute(true, true, true);
	EndLocation->SetHiddenInGame(true);
	EndLocation->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AMenuCamera::BeginPlay()
{
	Super::BeginPlay();

	// Check if animation curve is set up in editor (I think)
	if (AnimCurve && StartLocation && EndLocation)
	{
		FOnTimelineFloat ProgressFunction;

		ProgressFunction.BindUFunction(this, FName("HandleProgress"));

		MyTimeline.AddInterpFloat(AnimCurve, ProgressFunction);
		MyTimeline.SetLooping(false);

		ActorInitialLocation = StartLocation->GetComponentLocation();
		TargetLocation = EndLocation->GetComponentLocation();

		float StartTime;
		float EndTime;

		AnimCurve->GetTimeRange(StartTime, EndTime);

		Duration = (EndTime - StartTime) * 0.98f;

	}

	UWorld * World = GetWorld();
	if (World)
	{
		PlayerController = UGameplayStatics::GetPlayerController(World, 0);
	}

}

// Called every frame
void AMenuCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MyTimeline.TickTimeline(DeltaTime);

	/* Does this need to be called each tick? I think it does, should test it though */
	if (bIsActive && PlayerController)
	{
		PlayerController->SetViewTarget(this);
	}

	/* When returning from pause menu to main menu these cameras stop ticking. I have no idea why. */

	
}

void AMenuCamera::HandleProgress(float Value)
{
	FVector NewLocation = FMath::Lerp(ActorInitialLocation, TargetLocation, Value);

	SetActorLocation(NewLocation);

}

void AMenuCamera::OnSwitchTo()
{
	MyTimeline.PlayFromStart();

	bIsActive = true;

}

void AMenuCamera::OnSwitchFrom()
{
	bIsActive = false;

}

