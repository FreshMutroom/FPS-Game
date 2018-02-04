// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "FlamingRock.h"
#include "FPS_ProjectCharacter.h"
#include "MyStatics.h"


// Sets default values
AFlamingRock::AFlamingRock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rock Mesh"));
	StaticMesh->OnComponentHit.AddDynamic(this, &AFlamingRock::OnHit);
	RootComponent = StaticMesh;

	PS_Rock = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Rock Particles"));
	PS_Rock->AttachTo(RootComponent);
	PS_Rock->SetActive(false);
	PS_Rock->SetVisibility(false);

	ParticleDuration = 30.f;

	InitialLifeSpan = 60.f;

	Damage = 101.f;

}

// Called when the game starts or when spawned
void AFlamingRock::BeginPlay()
{
	Super::BeginPlay();

	DamagingSpeedSquared = DamagingSpeed * DamagingSpeed;
	

}

// Called every frame
void AFlamingRock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFlamingRock::SetFire()
{
	if (!bFireAlreadyLit)
	{
		if (PS_Rock)
		{
			PS_Rock->SetVisibility(true);
			PS_Rock->SetActive(true);
			bFireAlreadyLit = true;
			GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &AFlamingRock::PutOutFire, ParticleDuration, false);
		}

		UMyStatics::PlayASoundAttached(StaticMesh, FireSound, FVector(0.f), 0.5f);
		
	}

}

void AFlamingRock::FallSoundOffCooldown()
{
}

void AFlamingRock::OnHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	if (OtherActor->IsA(AFPS_ProjectCharacter::StaticClass()))
	{
		AFPS_ProjectCharacter* Player = Cast<AFPS_ProjectCharacter>(OtherActor);

		if (Player)
		{
			if (GetVelocity().SizeSquared() > DamagingSpeedSquared)
			{
				Player->ReceiveDamage(Damage);
			}
			
		}

	}

	if (GetVelocity().Z > 400.f)
	{
		UWorld * World = GetWorld();
		if (World)
		{
			if (World->GetTimerManager().IsTimerActive(FallSoundTimerHandle))
			{
				// Do nothing, timer is active
			}
			else
			{
				World->GetTimerManager().SetTimer(FallSoundTimerHandle, this, &AFlamingRock::FallSoundOffCooldown, 1.0f, false);

				UMyStatics::PlayASound(this, FallSound, GetActorLocation(), 0.75f);

			}
			
		}
		
	}

}

void AFlamingRock::PutOutFire()
{
	PS_Rock->SetActive(false);
	GetWorldTimerManager().ClearTimer(CountdownTimerHandle);

}

