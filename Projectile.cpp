// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "StaticTarget.h"
#include "DamagingRock.h"
#include "Projectile.h"
#include "AIPatrol.h"
#include "Advanced_Bot.h"
#include "IdleGuard.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "DamagableBot.h"
#include "Ver2_LauncherProjectile.h"
#include "Sniper_Bot.h"


// Sets default values
AProjectile::AProjectile()
{
	ProjectileSpeed = 200000.0f;
	GravityDelay = 0.0f;
	GravityScale = 0.0f;

	ProjectileCollision = CreateDefaultSubobject<USphereComponent>(FName("ProjectileCollision"));
	ProjectileCollision->InitSphereRadius(1.0f);
	ProjectileCollision->BodyInstance.SetCollisionProfileName("Projectile");
	ProjectileCollision->OnComponentHit.AddDynamic(this, &AProjectile::OnProjectileHit);
	SetRootComponent(ProjectileCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));

	// Tell ProjectileMovement what to attach to
	ProjectileMovement->UpdatedComponent = ProjectileCollision;
	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = GravityScale;
	ProjectileMovement->bRotationFollowsVelocity = true;

	bIsOriginalAmmo = false;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);

	ExplosionForce = CreateDefaultSubobject<URadialForceComponent>(FName("Explosion Force"));
	ExplosionForce->ImpulseStrength = 13000.f;
	ExplosionForce->DestructibleDamage = 1.01f;
	ExplosionForce->ForceStrength = 10.f;
	ExplosionForce->Radius = 800.f;
	//ExplosionForce->RemoveObjectTypeToAffect(EObjectTypeQuery::);
	ExplosionForce->SetupAttachment(RootComponent);
	
	BlastSphere = CreateDefaultSubobject<USphereComponent>(FName("Blast mesh"));
	BlastSphere->SetupAttachment(RootComponent);
	BlastSphere->OnComponentHit.AddDynamic(this, &AProjectile::InsideBlast);
	BlastSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Skeletal mesh for sniper"));
	SkeletalMesh->SetupAttachment(RootComponent);

	ProjectileParticles = CreateDefaultSubobject<UParticleSystemComponent>(FName("ProjectileParticles"));
	ProjectileParticles->SetupAttachment(RootComponent);

	BulletMeshDamage = CreateDefaultSubobject<URadialForceComponent>(FName("Force that damages DMs"));
	BulletMeshDamage->SetupAttachment(RootComponent);


	/* If bullet doesn't hit anything after 3 seconds, destroy it*/
	InitialLifeSpan = 3.0f;

	// Added by me for grenade and sniper ammo
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// This is relevant for laser rifle ammo only
	ProjectileDamage = 80.0f;	

	MaxRange = 30000.f;

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (!MyPlayer)
	{
		MyPlayer = Cast<AFPS_ProjectCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
		if (!MyPlayer)
		{
			return;
		}
	}

	switch (MyPlayer->EquippedWeapon)
	{
	// Auto rifle ammo
	case 1:
		// Spawn a projectile from blueprints then destroy this projectile
		if (GetWorld())
		{
			if (bIsOriginalAmmo)	// So an infinite amount of ammo aren't spawned
			{
				GetWorld()->SpawnActor<AProjectile>(BP_AutoRifle, GetActorLocation(), GetActorRotation());
				Destroy();
			}
		}

		break;
		
	// grenade launcher ammo
	case 2:
			
		// Spawn a projectile from blueprints then destroy this projectile
		if (GetWorld())
		{
			if (bIsOriginalAmmo)	// So an infinite amount of ammo aren't spawned
			{
				// Make grenade spawn facing up a bit more
				FRotator GrenadeOffset = FRotator(7.5f, 0.f, 0.f);
				
				AProjectile* Temp = GetWorld()->SpawnActor<AProjectile>(BP_GrenadeLauncher, GetActorLocation(), GetActorRotation() + GrenadeOffset);
				Destroy();
			}
		}

		break;
	
	// Sniper rifle ammo
	case 3:
		
		if (GetWorld())
		{
			if (bIsOriginalAmmo)	// So an infinite amount of ammo aren't spawned
			{
				AProjectile* Temp = GetWorld()->SpawnActor<AProjectile>(BP_SniperRifle, GetActorLocation(), GetActorRotation());
				Destroy();
			}
		}

		break;
		
		// laser ammo
	default:
		break;
	}

	// After GravityDelay amount of time, make the bullet start to drop
	FTimerHandle DummyHandle;
	GetWorldTimerManager().SetTimer(DummyHandle, this, &AProjectile::ApplyGravity, GravityDelay, false);

	// Stop if a grenade launcher
	if (MyPlayer->EquippedWeapon == 2)
	{
		return;
	}

	if (ProjectileParticles)
	{
		ProjectileParticles->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Store where the projectile spawns which is at the muzzle for creating the particle system later
	const FVector StartTrace = this->GetActorLocation();
	const FVector ForwardVector = MyPlayer->FirstPersonCameraComponent->GetForwardVector();
	const FVector EndTrace = (ForwardVector * MaxRange) + StartTrace;

	FVector NewSpawnLocation;
	FRotator NewSpawnRotation;

	if (MyPlayer)
	{
		// The projectile spawns at the gun muzzle. This moves it to the centre of the screen so what is under the 
		// crosshair is always hit. Particle effects will still spawn from the gun muzzle.
		NewSpawnLocation = MyPlayer->GetFirstPersonCameraComponent()->GetComponentLocation();	
		NewSpawnRotation = MyPlayer->GetFirstPersonCameraComponent()->GetComponentRotation();
		SetActorLocation(NewSpawnLocation + FVector(101.f, 0.f, 0.f));	// Start a little forward since the projectile collides with something otherwise
		SetActorRotation(NewSpawnRotation);

	}

	// Do a ray trace to find the first thing the projectile hits so the particle system knows where to stop
	const FVector RayTraceEnd = NewSpawnLocation + UKismetMathLibrary::GetForwardVector(NewSpawnRotation) * MaxRange;
	FHitResult HitResult = FHitResult();
	FCollisionQueryParams CQP = FCollisionQueryParams();
	CQP.AddIgnoredActor(this);
	CQP.AddIgnoredActor(MyPlayer);
	CQP.bReturnPhysicalMaterial = true;
	FVector NewEndTrace;
	FVector ImpactPoint;

	if (GetWorld()->LineTraceSingleByChannel(HitResult, NewSpawnLocation, RayTraceEnd, ECC_Visibility, CQP))
	{
		// ray cast was successful

		if (HitResult.GetActor() != NULL)
		{
			// The minus value is because the beam gets drawn past the object it hits a little bit
			NewEndTrace = (ForwardVector * (HitResult.Distance - 160.f)) + StartTrace;
			ImpactPoint = HitResult.ImpactPoint;

			// Work out hit damage here, at least for bots. 	
			// Changing the velocity just doesn't make the projectile always hit under the crosshair
			AActor* HitTarget = HitResult.GetActor();

			if (HitTarget->IsA(AIdleGuard::StaticClass()))
			{
				AIdleGuard* Bot = Cast<AIdleGuard>(HitTarget);

				Bot->HasBeenShot();
			}
			else if (Type == 3 && HitTarget->IsA(ASniper_Bot::StaticClass()))
			{
				ASniper_Bot* Hit = Cast<ASniper_Bot>(HitTarget);
				if (Hit)
				{
					Hit->OnHit();
				}
			}
			else
			{
				IDamagableBot* DamagableActor = Cast<IDamagableBot>(HitTarget);

				if (DamagableActor)
				{
					float Damage = FMath::RandRange(0.9f, 1.1f) * ProjectileDamage;

					// Check which part we hit.
					if (HitResult.BoneName == "head" || HitResult.BoneName == "neck_01" ||
						HitResult.BoneName == "b_head" || HitResult.BoneName == "b_Neck" ||
						HitResult.BoneName == "b_MF_Head" || HitResult.BoneName == "b_MF_Neck"
						)
					{
						// Headshot
						DamagableActor->ReceiveDamage(Damage * 2);
						
					}
					else
					{
						// Limbs or torso. Don't distinguish between them right now - too many bone names
						DamagableActor->ReceiveDamage(Damage);

						

					}

				}

			}
			
		}
		else
		{
			NewEndTrace = (ForwardVector * MaxRange) + StartTrace;
			ImpactPoint = NewEndTrace;

		}

		// If here I believe the raycast was still successful

		// Get physical material of hit
		UPhysicalMaterial* HitPhysicalMaterial = HitResult.PhysMaterial.Get();
		EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysicalMaterial);
		int32 PhysMaterialIndex;
		float Scale = 30.f;
		float Duration = 30.f;
		switch (SurfaceType)
		{
		case EPhysicalSurface::SurfaceType_Default:
			PhysMaterialIndex = 0;
			break;

		case EPhysicalSurface::SurfaceType1:
			PhysMaterialIndex = 1;
			break;

		case EPhysicalSurface::SurfaceType2:
			PhysMaterialIndex = 2;
			break;

		case EPhysicalSurface::SurfaceType3:
			PhysMaterialIndex = 3;
			break;

		case EPhysicalSurface::SurfaceType4:
			PhysMaterialIndex = 4;
			break;

		case EPhysicalSurface::SurfaceType5:
			PhysMaterialIndex = 5;
			break;

		case EPhysicalSurface::SurfaceType6:
			PhysMaterialIndex = 6;
			break;

		case EPhysicalSurface::SurfaceType7:
			PhysMaterialIndex = 7;
			break;

		case EPhysicalSurface::SurfaceType8:
			PhysMaterialIndex = 8;
			break;
		
		default:
			PhysMaterialIndex = -1;
			break;
		}

		if (PhysMaterialIndex >= 0)
		{
			SpawnHitDecal(PhysMaterialIndex, Scale, Duration, HitResult);
		}

	}
	else
	{
		NewEndTrace = (ForwardVector * MaxRange) + StartTrace;
		ImpactPoint = NewEndTrace;
	}

	if (ProjectileParticles)
	{
		ProjectileParticles->SetBeamSourcePoint(0, StartTrace, 0);
		ProjectileParticles->SetBeamTargetPoint(0, ImpactPoint, 0);
	}
	
	// New velocity for projectile firing from camera
	FVector AdjustedVelocity = (NewSpawnLocation + GetActorForwardVector() * 130.f) - ImpactPoint;
	FRotator AdjustedRotation = UKismetMathLibrary::MakeRotFromX(AdjustedVelocity);
	FTransform AdjustedTransform = FTransform(AdjustedRotation, (NewSpawnLocation + GetActorForwardVector() * 130.f), FVector(1.f));
	SetActorTransform(AdjustedTransform);

	// Calculate correct velocity vector so projectile will hit what is under the crosshair
	//FVector AdjustedVelocity = StartTrace - ImpactPoint;
	//FRotator AdjustedRotation = UKismetMathLibrary::MakeRotFromX(AdjustedVelocity);
	//FTransform AdjustedTransform = FTransform(AdjustedRotation, StartTrace, FVector(1.f));
	//SetActorTransform(AdjustedTransform);

	// Control how long the beam stays seen for. Can remove this for non beam weapons.
	// Setting InitalLifeSpawn is just as easy
	FTimerHandle BeamDurationTimerHandle;
	
	// If auto rifle ammo
	if (Type == 1)
	{
		GetWorldTimerManager().SetTimer(BeamDurationTimerHandle, this, &AProjectile::Deestroy, 0.4f, false);
	}
	
	// Laser rifle ammo
	else if (Type == 0)
	{
		GetWorldTimerManager().SetTimer(BeamDurationTimerHandle, this, &AProjectile::Deestroy, 0.6f, false);
	}

}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// If sniper rifle ammo
	if (Type == 3)
	{
	}
		// Spawn a smoke contrail
		/*UWorld* World = GetWorld();
		if (World)
		{
			
			UGameplayStatics::SpawnEmitterAtLocation(World, ProjectileParticles->Template, GetTransform(), true);
		}
	}*/
	
	// If  grenade launcher ammo
	else if (Type == 2)
	{
		// If the grenade projectile has hit something then the sphere collision will be 
		// enabled. Disable it after one tick since the projectile actor isn't destroyed
		// until after 2 seconds.
		//if (BlastSphere->IsCollisionEnabled())
		//{
		//	BlastSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//}
		//
		//if (ProjectileMovement->MaxSpeed > 1200.f)
		//{
		//	// Apply deceleration
		//	ProjectileMovement->MaxSpeed -= (3.f * DeltaTime);
		//}
	}
	
}

void AProjectile::OnProjectileHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	// If grenade launcher ammo skip damage done from a direct hit. All damage will be done
	// by the explosion
	if (Type == 2) 
	{
		DealExplosionDamage();
	}
	
	else if (OtherActor->IsA(AStaticTarget::StaticClass()))
	{
		if (Hit.BoneName != NAME_None)
		{
			OtherActor->Destroy();
		}
	}
	else if (OtherActor->IsA(ADamagingRock::StaticClass()))
	{
		ADamagingRock* Rock = Cast<ADamagingRock>(OtherActor);

		// This may be causing rocks to explode instead of the code in DamagingRock.cpp
		Rock->Explode(NormalImpulse);

	}
	
	//else if (OtherActor->IsA(AIdleGuard::StaticClass()))
	//{
	//	AIdleGuard* Bot = Cast<AIdleGuard>(OtherActor);
	//
	//	Bot->HasBeenShot();
	//
	//}
	//		
	//else
	//{
	//	// Even though AIPatrol implements Damagable it does not get cast here
	//	IDamagableBot* DamagableActor = Cast<IDamagableBot>(OtherActor);
	//
	//	if (DamagableActor)
	//	{
	//		
	//		if (Hit.BoneName != NAME_None)
	//		{
	//			
	//		}
	//
	//		// Check which part we hit.
	//		if (Hit.BoneName == "head" || Hit.BoneName == "neck_01" ||
	//			Hit.BoneName == "b_head" || Hit.BoneName == "b_Neck" ||
	//			Hit.BoneName == "b_MF_Head" || Hit.BoneName == "b_MF_Neck"
	//			)
	//		{
	//			// Headshot
	//			DamagableActor->ReceiveDamage(ProjectileDamage * 2);
	//			
	//		}
	//		else
	//		{
	//			// Limbs or torso. Don't distinguish between them right now - too many bone names
	//			DamagableActor->ReceiveDamage(ProjectileDamage);
	//
	//		
	//
	//		}
	//
	//	}
	//
	//}
	
	// If a grenade launcher ammo
	if (Type == 2)
	{
		if (ProjectileParticles)
		{
			ProjectileParticles->SetVisibility(true);
		}

		if (ProjectileMesh)
		{
			// Make mesh invisible
			ProjectileMesh->SetVisibility(false);
			ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			ProjectileMesh->bGenerateOverlapEvents = false;
		}

		// Create an explosion
		if (ExplosionForce)
		{
			// If bots get blasted outside map and fall below minimum Z depth then the 
			// player's mouseover arrays will cause the game to crash
			
			// Fire impulse twice to instantly make destructible meshes fully fracture
			ExplosionForce->FireImpulse();
			ExplosionForce->FireImpulse();
		}

		// Make sure it fractures my rocks I created, damages enemies and damages the player
		if (BlastSphere)
		{
			BlastSphere->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		}
		
		// Destroy actor after 2 seconds when explosion particle effect has finished
		FTimerHandle DestroyTimerHandle;
		GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AProjectile::Deestroy, 2.f, false);

	}
	else
	{
		// Disable collision on mesh and collision shape
		if (ProjectileCollision)
		{
			ProjectileCollision->bGenerateOverlapEvents = false;
			ProjectileCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		
		if (ProjectileMesh)
		{
			ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		// Create mini radial force to destroy destructible meshes
		if (Type == 1)
		{
			if (BulletMeshDamage)
			{
				BulletMeshDamage->FireImpulse();
			}
		}
		else if (Type == 3)
		{
			if (BulletMeshDamage)
			{
				BulletMeshDamage->FireImpulse();
				BulletMeshDamage->FireImpulse();
			}
		}

	}

}

void AProjectile::ApplyGravity()
{
	ProjectileMovement->ProjectileGravityScale = GravityScale;

}

void AProjectile::Deestroy()
{
	Destroy();

}

void AProjectile::InsideBlast(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	// This never fires. The radial force component must be what causes other rocks to explode
	
	if (Type != 2)
	{
		return;
	}

	if (OtherActor->IsA(AFPS_ProjectCharacter::StaticClass()))
	{
		// damage player
	}
	
	if (OtherActor->IsA(ADamagingRock::StaticClass()))
	{
		ADamagingRock* Rock = Cast<ADamagingRock>(OtherActor);

		Rock->Explode(NormalImpulse);
	}
	
	if (OtherActor->IsA(AAIPatrol::StaticClass()))
	{
		// damage enemy
		AAIPatrol* Bot = Cast<AAIPatrol>(OtherActor);

		float Distance = (GetActorLocation() - Bot->GetActorLocation()).Size();

		if (Distance < 100.f)
		{
			Bot->ReceiveDamage(301.f);
		}
		else if (Distance < 250.f)
		{
			Bot->ReceiveDamage(200.f);
		}
		else if (Distance < 400.f)
		{
			Bot->ReceiveDamage(150.f);
		}
		else if (Distance < 550.f)
		{
			Bot->ReceiveDamage(80.f);
		}
	}

	if (OtherActor->IsA(AVer2_LauncherProjectile::StaticClass()))
	{
		AVer2_LauncherProjectile* Projectile = Cast<AVer2_LauncherProjectile>(OtherActor);

		if (Projectile && !Projectile->bIsPendingDestroy)
		{
			Projectile->Explode();
		}
	}

	if (OtherActor->IsA(AAdvanced_Bot::StaticClass()))
	{
		// damage enemy
		AAdvanced_Bot* Bot = Cast<AAdvanced_Bot>(OtherActor);

		float Distance = (GetActorLocation() - Bot->GetActorLocation()).Size();

		if (Distance < 100.f)
		{
			Bot->ReceiveDamage(301.f);
		}
		else if (Distance < 250.f)
		{
			Bot->ReceiveDamage(200.f);
		}
		else if (Distance < 400.f)
		{
			Bot->ReceiveDamage(150.f);
		}
		else if (Distance < 550.f)
		{
			Bot->ReceiveDamage(80.f);
		}
	}

}

void AProjectile::DealExplosionDamage()
{
	if (ExplosionForce)
	{
		float Radius = ExplosionForce->Radius;

		// Check if need to damage player
		if (MyPlayer)
		{
			float Distance = (MyPlayer->GetActorLocation() - GetActorLocation()).Size();
			if (Distance < Radius)
			{
				// damage player
				float Damage = CalculateExplosionDamage(Distance, Radius);
				if (Damage > 0.f)
				{
					MyPlayer->ReceiveDamage(Damage);
				}

			}

		}

		// Damage AI
		UWorld* World = GetWorld();
		if (World)
		{
			TArray<AActor*> HitTargets;
			UGameplayStatics::GetAllActorsWithInterface(World, UDamagableBot::StaticClass(), HitTargets);

			for (AActor* Hit : HitTargets)
			{
				float Distance = (Hit->GetActorLocation() - GetActorLocation()).Size();
				
				if (Distance < Radius)
				{
					IDamagableBot* AsDamagable = Cast<IDamagableBot>(Hit);
					if (AsDamagable)
					{
						float Damage = CalculateExplosionDamage(Distance, Radius);
						
						AsDamagable->ReceiveDamage(Damage);

					}

				}

			}

		}

	}

}

float AProjectile::CalculateExplosionDamage(float Distance, float Radius)
{
	float Multiplier = FMath::RandRange(0.9f, 1.1f);
	
	float Damage = (1.f - (Distance / Radius)) * ProjectileDamage * Multiplier;
	
	return Damage;
}

void AProjectile::SpawnHitDecal(int32 PhysMaterialType, float Scale, float Duration, const FHitResult & Hit)
{
	
	if (ImpactDecals.IsValidIndex(PhysMaterialType) && Hit.GetActor() != nullptr)
	{
		UGameplayStatics::SpawnDecalAtLocation(Hit.GetActor(), ImpactDecals[PhysMaterialType], FVector(Scale), Hit.ImpactPoint, FRotator(0.f), Duration);
		

	}

}



