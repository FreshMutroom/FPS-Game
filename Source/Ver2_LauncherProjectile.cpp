// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "Ver2_LauncherProjectile.h"
#include "DamagableBot.h"
#include "MyStatics.h"


// Sets default values
AVer2_LauncherProjectile::AVer2_LauncherProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// This is actually null inside the editor and may be the reason there is collision with an 
	// invisible object after this ammo has exploded
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->OnComponentHit.AddDynamic(this, &AVer2_LauncherProjectile::OnProjectileHit);
	//CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AVer2_LauncherProjectile::OnOverlap);
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	//CollisionComp->SetSimulatePhysics(true);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComp->bGenerateOverlapEvents = true;
	RootComponent = CollisionComp;

	//OnActorHit.AddDynamic(this, &AVer2_LauncherProjectile::OnProjectileHit);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 0.f;
	ProjectileMovement->MaxSpeed = 700.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->bInitialVelocityInLocalSpace = true;
	ProjectileMovement->bIsHomingProjectile = true;
	ProjectileMovement->HomingAccelerationMagnitude = 5000.f;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	Mesh->bGenerateOverlapEvents = false;
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	Mesh->SetRelativeScale3D(FVector(1.f, 1.2f, 1.f));
	//Mesh->OnComponentHit.AddDynamic(this, &AVer2_LauncherProjectile::OnProjectileHit);
	Mesh->AttachTo(RootComponent);


	TrailParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Trail particles"));
	TrailParticles->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TrailParticles->AttachTo(Mesh);

	ExplosionParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ExplosionParticles"));
	ExplosionParticles->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ExplosionParticles->AttachTo(RootComponent);

	ExplosionForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("Explosion force"));
	ExplosionForce->ImpulseStrength = 24614.441406f;
	ExplosionForce->Radius = 800.f;
	ExplosionForce->bIgnoreOwningActor;
	ExplosionForce->DestructibleDamage = 2.1f;
	ExplosionForce->AttachTo(RootComponent);

	MinDamage = 70.f;
	MaxDamage = 95.f;
	BlastRadius = 800.f;

	InitialLifeSpan = 60.f;

}

// Called when the game starts or when spawned
void AVer2_LauncherProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();

	if (World)
	{
		ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0);

		if (Character)
		{
			Player = Cast<AFPS_ProjectCharacter>(Character);

			if (Player)
			{
				UCapsuleComponent* PlayerCapsule = Player->GetCapsuleComponent();
				
				if (PlayerCapsule)
				{
					ProjectileMovement->HomingTargetComponent = PlayerCapsule;

				}
				
			}

		}

	}

	if (ExplosionParticles)
	{
		ExplosionParticles->SetVisibility(false);
	}

	if (TrailParticles)
	{
		TrailParticles->ActivateSystem(true);
		TrailParticles->SetVisibility(true);
	}
	
	if (ExplosionForce)
	{
		ExplosionForce->Radius = BlastRadius;
		ExplosionForce->SetVisibility(false);
	}

}

// Called every frame
void AVer2_LauncherProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVer2_LauncherProjectile::OnOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (bIsPendingDestroy)
	{
		return;
	}

	bIsPendingDestroy = true;

	Explode();

}

void AVer2_LauncherProjectile::OnProjectileHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	
	if (bIsPendingDestroy)
	{
		return;
	}
	
	bIsPendingDestroy = true;

	Explode();

}

void AVer2_LauncherProjectile::Deestroy()
{
	Destroy();
}

void AVer2_LauncherProjectile::Explode()
{
	if (CollisionComp)
	{
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CollisionComp->SetSimulatePhysics(false);
	}
	
	if (ExplosionParticles)
	{
		ExplosionParticles->SetVisibility(true);
		ExplosionParticles->ActivateSystem(true);
	}

	if (TrailParticles)
	{
		TrailParticles->SetVisibility(false);
		TrailParticles->ActivateSystem(false);
	}

	if (Mesh)
	{
		Mesh->SetVisibility(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (ExplosionForce)
	{
		// Fire impulse twice to DM break up instantly
		ExplosionForce->FireImpulse();
		ExplosionForce->FireImpulse();
	}

	UWorld* World = GetWorld();

	if (World)
	{
		FTimerHandle DestroyTimerHandle;
		World->GetTimerManager().SetTimer(DestroyTimerHandle, this, &AVer2_LauncherProjectile::Deestroy, 1.5f, false);

	}

	// Play explosion sound
	UMyStatics::PlaySound(this, ExplosionSounds, GetActorLocation());

	// Deal damage to player
	if (Player)
	{
		// Maybe also raycast to check if player is behind cover and nullify damage if they are
		
		float DistanceToPlayer = (GetActorLocation() - Player->GetActorLocation()).Size();
		if (DistanceToPlayer <= BlastRadius / 4)
		{
			// Full damage if inside first quarter of blast radius
			float Damage = FMath::RandRange(MinDamage, MaxDamage);
			Player->ReceiveDamage(Damage);
		}
		else if (DistanceToPlayer <= BlastRadius)
		{
			// Damage drops off linearly to a minimum of 0% at BlastRadius
			float Damage = FMath::RandRange(MinDamage, MaxDamage);
			float Multiplier = (DistanceToPlayer - (BlastRadius / 4)) / (BlastRadius - (BlastRadius / 4));
			Damage *= Multiplier;
			Player->ReceiveDamage(Damage);
		}
	}
	 
	// Deal damage to AI
	TArray<AActor*> HitTargets;
	
	if (World)
	{
		UGameplayStatics::GetAllActorsWithInterface(World, UDamagableBot::StaticClass(), HitTargets);
		{
			for (AActor* Hit : HitTargets)
			{
				float Distance = (GetActorLocation() - Hit->GetActorLocation()).Size();
				if (Distance <= BlastRadius)
				{
					IDamagableBot* AsDamagable = Cast<IDamagableBot>(Hit);
					if (AsDamagable)
					{
						if (Distance <= BlastRadius / 4)
						{
							// Full damage if inside first quarter of blast radius
							float Damage = FMath::RandRange(MinDamage, MaxDamage);
							AsDamagable->ReceiveDamage(Damage);
						}
						else if (Distance <= BlastRadius)
						{
							// Damage drops off linearly to a minimum of 0% at BlastRadius
							float Damage = FMath::RandRange(MinDamage, MaxDamage);
							float Multiplier = (Distance - (BlastRadius / 4)) / (BlastRadius - (BlastRadius / 4));
							Damage *= Multiplier;
							AsDamagable->ReceiveDamage(Damage);
						}
					}
				}
			}
		}

	}
	
}

