// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "Rotatable.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "FlamingRock.h"


// Sets default values
ARotatable::ARotatable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AnimTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("AnimationTrigger"));
	AnimTrigger->bGenerateOverlapEvents = true;
	AnimTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARotatable::OnAnimTriggered);

	RotatableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RotatableMesh"));
	RotatableMesh->SetAbsolute(true, true, true);
	RotatableMesh->AttachTo(RootComponent);
	RootComponent = RotatableMesh;

	AnimTrigger->SetupAttachment(RootComponent);

	bTriggerOnlyOnce = false;
	TriggerDelay = 0.0f;

	TimeDilationAmount = 1.f;

}

// Called when the game starts or when spawned
void ARotatable::BeginPlay()
{
	Super::BeginPlay();
	
	// Check if animation curve is set up in editor (I think)
	if (AnimCurve)
	{
		FOnTimelineFloat ProgressFunction;

		ProgressFunction.BindUFunction(this, FName("HandleProgress"));

		MyTimeline.AddInterpFloat(AnimCurve, ProgressFunction);
		MyTimeline.SetLooping(false);

		ActorInitialRotation = TargetRotation = GetActorRotation();

		
		if (RotationAxis == "Yaw")
		{
			TargetRotation.Yaw += MaxRotation;
		}
		else if (RotationAxis == "Pitch")
		{
			TargetRotation.Pitch += MaxRotation;
		}
		else if (RotationAxis == "Roll")
		{
			TargetRotation.Roll += MaxRotation;
		}

		// Set up time dilation curve too
		if (TimeDilationAmount != 1.f)
		{
			if (TimeDilationCurve)
			{
				FOnTimelineFloat ProgressFunction2;

				ProgressFunction2.BindUFunction(this, FName("HandleTimeDilationProgress"));

				TimeDilationTimeline.AddInterpFloat(TimeDilationCurve, ProgressFunction2);
				TimeDilationTimeline.SetLooping(false);

			}
		}

		// Set up change player facing curve too
		if (bChangesPlayerFacing)
		{
			if (ChangeFacingCurve)
			{
				FOnTimelineFloat ProgressFunction3;

				ProgressFunction3.BindUFunction(this, FName("HandleChangeFacingProgress"));

				ChangeFacingTimeline.AddInterpFloat(ChangeFacingCurve, ProgressFunction3);
				ChangeFacingTimeline.SetLooping(false);

			}

		}

	}

}

// Called every frame
void ARotatable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MyTimeline.TickTimeline(DeltaTime);

	TimeDilationTimeline.TickTimeline(DeltaTime);

	ChangeFacingTimeline.TickTimeline(DeltaTime);

	if (bChangesPlayerFacing && ChangeFacingCurve)
	{
		if (ChangeFacingTimeline.GetPlaybackPosition() == ChangeFacingTimeline.GetTimelineLength())
		{
			// re enable player input
			UWorld* World = GetWorld();
			if (Player && World)
			{
				Player->EnableInput(World->GetFirstPlayerController());
				
				// So input never keeps getting re enabled
				bChangesPlayerFacing = false;


			}

		}
	}

}

void ARotatable::PlayAnimation()
{
	if (!MyTimeline.IsPlaying())
	{
		MyTimeline.PlayFromStart();
	}

}

void ARotatable::SlowDownTime()
{
	if (TimeDilationCurve)
	{
		if (!TimeDilationTimeline.IsPlaying())
		{
			TimeDilationTimeline.PlayFromStart();

		}

	}

}

void ARotatable::HandleTimeDilationProgress(float Value)
{
	UWorld* World = GetWorld();

	if (World)
	{
		float NewTimeDilation = FMath::Lerp(1.f, TimeDilationAmount, Value);

		UGameplayStatics::SetGlobalTimeDilation(World, NewTimeDilation);

	}
	
}

void ARotatable::ChangePlayerFacing()
{
	if (Player)
	{
		UWorld* World = GetWorld();

		if (World)
		{
			// Disable player input
			Player->DisableInput(World->GetFirstPlayerController());

			// Make player stop sprinting
			Player->StopSprinting();

			// Make player uncrouch
			Player->LCTRLReleased();

		}
		
		PlayerInitialRotation = Player->GetActorRotation();

	}
	
	if (ChangeFacingCurve)
	{
		if (!ChangeFacingTimeline.IsPlaying())
		{
			ChangeFacingTimeline.PlayFromStart();
		}
	}

}

void ARotatable::HandleChangeFacingProgress(float Value)
{
	if (Player)
	{
		FRotator NewRotation = FMath::Lerp(PlayerInitialRotation, UKismetMathLibrary::FindLookAtRotation( Player->GetActorLocation(), GetActorLocation() ), Value);

		Player->GetController()->SetControlRotation(NewRotation);

	}

}

void ARotatable::LightBoulders()
{
	TArray<AActor*> Boulders;
	UWorld* World = GetWorld();
	if (World)
	{
		UGameplayStatics::GetAllActorsOfClass(World, AFlamingRock::StaticClass(), Boulders);

		for (AActor* Actor : Boulders)
		{
			AFlamingRock* Boudler = Cast<AFlamingRock>(Actor);
			if (Boudler)
			{
				// Could add a distance check here to not light boulders in other traps
				
				Boudler->SetFire();
			}
		}

	}

}

void ARotatable::HandleProgress(float Value)
{
	// Make this a FVector to make the object a moveable instead of a rotatable
	FRotator NewRotation = FMath::Lerp(ActorInitialRotation, TargetRotation, Value);

	SetActorRotation(NewRotation);

}

void ARotatable::OnAnimTriggered(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(AFPS_ProjectCharacter::StaticClass()))
	{
		if (!OverlapLock.TryLock())
		{
			return;
		}
		
		// If animation isn't playing then start the animation
		if (!MyTimeline.IsPlaying())
		{
			// Check to see if the has already played at least once
			if (bTriggerOnlyOnce)
			{
				if (bHasBeenTriggered)
				{
					OverlapLock.Unlock();
					return;
				}
				else
				{
					bHasBeenTriggered = true;
				}
			}

			if (TriggerDelay > 0.0f)
			{
				FTimerHandle TriggerDelayTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TriggerDelayTimerHandle, this, &ARotatable::PlayAnimation, TriggerDelay, false);

			}
			else
			{
				MyTimeline.PlayFromStart();
			}

		}

		UWorld* World = GetWorld();

		if (World)
		{
			// Time dilation stuff starts here
			if (TimeDilationAmount != 1.f)
			{
				World->GetTimerManager().SetTimer(TimeDilationDelayTimerHandle, this, &ARotatable::SlowDownTime, TimeDilationDelay, false);

			}

			// Change facing stuff starts here
			if (bChangesPlayerFacing && bTriggerOnlyOnce)
			{
				Player = Cast<AFPS_ProjectCharacter>(OtherActor);
				
				if (Player)
				{
					if (ChangeFacingDelay > 0.0f)
					{
						FTimerHandle ChangeFacingTimerHandle;
						World->GetTimerManager().SetTimer(ChangeFacingTimerHandle, this, &ARotatable::ChangePlayerFacing, ChangeFacingDelay, false);
					}
					else
					{
						ChangePlayerFacing();

					}

				}
				
			}

			// Boudler lighting stuff starts here
			if (bIsFlamingBoulderTrap)
			{
				if (BoulderLightDelay <= 0.f)
				{
					LightBoulders();
				}
				else
				{
					World->GetTimerManager().SetTimer(LightBouldersTimerHandle, this, &ARotatable::LightBoulders, BoulderLightDelay, false);

				}
				
			}

		}

		OverlapLock.Unlock();

	}

}

