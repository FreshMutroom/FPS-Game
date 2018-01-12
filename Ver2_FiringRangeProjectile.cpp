// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "Ver2_FiringRangeProjectile.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"


// Sets default values
AVer2_FiringRangeProjectile::AVer2_FiringRangeProjectile()
{

	ProjectileSpeed = 2000.f;

	ProjectileCollision = CreateDefaultSubobject<USphereComponent>(FName("ProjectileCollision"));
	ProjectileCollision->InitSphereRadius(1.0f);
	ProjectileCollision->BodyInstance.SetCollisionProfileName("Projectile");
	ProjectileCollision->BodyInstance.SetResponseToAllChannels(ECR_Overlap);	// does nothing
	ProjectileCollision->bGenerateOverlapEvents = true;
	ProjectileCollision->OnComponentBeginOverlap.AddDynamic(this, &AVer2_FiringRangeProjectile::OnOverlap);
	RootComponent = ProjectileCollision;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = ProjectileCollision;
	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->bShouldBounce = false;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("ProjectileMesh"));
	ProjectileMesh->AttachTo(RootComponent);

	RifleAmmoParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Ammo particles"));
	RifleAmmoParticles->AttachTo(RootComponent);

	RifleParticles = CreateDefaultSubobject<UParticleSystemComponent>(FName("ProjectileParticles"));
	RifleParticles->SetupAttachment(RootComponent);

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	InitialLifeSpan = 5.f;

}

// Called when the game starts or when spawned
void AVer2_FiringRangeProjectile::BeginPlay()
{
	Super::BeginPlay();

	int32 MakeNegativeX, MakeNegativeY;

	if (FMath::RandRange(0, 1) == 0)
	{
		MakeNegativeX = 1;
	}
	else
	{
		MakeNegativeX = -1;
	}

	if (FMath::RandRange(0, 1) == 0)
	{
		MakeNegativeY = 1;
	}
	else
	{
		MakeNegativeY = -1;
	}

	float XVar = FMath::RandRange(XVarianceMin, XVarianceMax) * MakeNegativeX;
	float YVar = FMath::RandRange(YVarianceMin, YVarianceMax) * MakeNegativeY;

	FRotator Rot = GetActorRotation();
	FRotator NewRotation = FRotator(0.f, XVar, YVar  + 1.f);

	SetActorRotation(NewRotation);

	if (ProjectileMovement)
	{
		FVector NewVelocity = NewRotation.RotateVector(ProjectileMovement->Velocity);
		ProjectileMovement->Velocity = (NewVelocity / NewVelocity.Size()) * ProjectileMovement->InitialSpeed;
	}

	UWorld * World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(DurationTimerHandle, this, &AVer2_FiringRangeProjectile::OkToCollide, 0.05f, false);

	}

}

// Called every frame
void AVer2_FiringRangeProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVer2_FiringRangeProjectile::OkToCollide()
{
	bCanCollide = true;

}

void AVer2_FiringRangeProjectile::OnOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	
	if (bCanCollide)
	{
		if (OtherActor->IsA(AFPS_ProjectCharacter::StaticClass()))
		{
			if (!Player)
			{
				Player = Cast<AFPS_ProjectCharacter>(OtherActor);
			}

			if (Player)
			{
				float Damage = FMath::RandRange(MinDamage, MaxDamage);

				Player->ReceiveDamage(Damage);

			}

		}
		else
		{
			IDamagableBot* Target = Cast<IDamagableBot>(OtherActor);

			if (Target)
			{
				float Damage = FMath::RandRange(MinDamage, MaxDamage);

				Target->ReceiveDamage(Damage);
			}

		}

		Destroy();
	}
	

}