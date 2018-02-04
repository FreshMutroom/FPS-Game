// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "TutorialMessage.h"


// Sets default values
ATutorialMessage::ATutorialMessage()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OverlapShape = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger shape"));
	OverlapShape->bGenerateOverlapEvents = true;
	OverlapShape->OnComponentBeginOverlap.AddDynamic(this, &ATutorialMessage::TriggerEnter);
	OverlapShape->OnComponentEndOverlap.AddDynamic(this, &ATutorialMessage::TriggerExit);
	RootComponent = OverlapShape;

	RepeatCooldown = 5.f;
	EraseTime = 5.0f;

	Sentence1Delay = 0.05f;
	Sentence2Delay = 0.05f;
	Sentence3Delay = 0.05f;

}

// Called when the game starts or when spawned
void ATutorialMessage::BeginPlay()
{
	Super::BeginPlay();
	
	Message = Sentence1 + Sentence2 + Sentence3 + Sentence4;

}

// Called every frame
void ATutorialMessage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Player)
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
		
	}

	if (Player && bIsPlayerInRange && !bIsCoolingDown)
	{
		if (StartDelay == 0.f)
		{
			CreateMessage();
		}
		else
		{
			// Added this so multiple messages aren't made
			bIsPlayerInRange = false;
			
			UWorld * World = GetWorld();
			if (World)
			{
				FTimerHandle DummyTimerHandle;
				World->GetTimerManager().SetTimer(DummyTimerHandle, this, &ATutorialMessage::CreateMessage, StartDelay, false);

			}
			
		}
		
	}

}

void ATutorialMessage::CreateMessage()
{
	if (!Player)
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

	}

	if (Player)
	{
		TArray<float> Delays;

		// can probably do this at begin play
		for (int i = 0; i < Message.Len(); i++)
		{
			Delays.Insert(Player->TypewriterDefaultDelay, i);
		}

		if (Sentence1.Len() > 0)
		{
			Delays.Insert(Sentence1Delay, Sentence1.Len());
		}
		if (Sentence2.Len() > 0)
		{
			Delays.Insert(Sentence2Delay, Sentence1.Len() + Sentence2.Len());
		}
		if (Sentence3.Len() > 0)
		{
			Delays.Insert(Sentence3Delay, Sentence1.Len() + Sentence2.Len() + Sentence3.Len());
		}

		// can probably do this at begin play
		TArray<int32> WordLengths = Player->FindWordLengths(Message);

		Player->CreateTypewriterMessage(Message, Delays, WordLengths, EraseTime);

		if (NextMessage)
		{
			UWorld * World = GetWorld();
			if (World)
			{
				
				ATutorialMessage * AnotherMessage = World->SpawnActor<ATutorialMessage>(NextMessage, FVector(0.f, 0.f, 4000.f), FRotator(0.f));
				if (AnotherMessage)
				{
					// Set up other message to be added to queue and eventually play
					AnotherMessage->Player = Player;
					AnotherMessage->bIsPlayerInRange = true;
				}
				
				
			}
		}

		if (!bCanRepeat)
		{
			Destroy();
		}
		else
		{
			bIsCoolingDown = true;

			UWorld* World = GetWorld();
			if (World)
			{
				if (!World->GetTimerManager().IsTimerActive(RepeatTimerHandle))
				{
					World->GetTimerManager().SetTimer(RepeatTimerHandle, this, &ATutorialMessage::MakeIsCoolingDownFalse, RepeatCooldown, false);

				}

			}
		}
	}

}

void ATutorialMessage::MakeIsCoolingDownFalse()
{
	bIsCoolingDown = false;

}

void ATutorialMessage::TriggerEnter(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!Player && OtherActor->IsA(AFPS_ProjectCharacter::StaticClass()))
	{
		Player = Cast<AFPS_ProjectCharacter>(OtherActor);
		bIsPlayerInRange = true;

	}
	else if (OtherActor->IsA(AFPS_ProjectCharacter::StaticClass()))
	{
		bIsPlayerInRange = true;
	}
	
}

void ATutorialMessage::TriggerExit(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(AFPS_ProjectCharacter::StaticClass()))
	{
		bIsPlayerInRange = false;
	}

}

