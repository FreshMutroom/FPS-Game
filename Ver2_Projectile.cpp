// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "Ver2_Projectile.h"
#include "FPS_ProjectCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
AVer2_Projectile::AVer2_Projectile()
{

	ProjectileSpeed = 2000.f;

	ProjectileCollision = CreateDefaultSubobject<USphereComponent>(FName("ProjectileCollision"));
	ProjectileCollision->InitSphereRadius(1.0f);
	ProjectileCollision->BodyInstance.SetCollisionProfileName("Projectile");
	ProjectileCollision->bGenerateOverlapEvents = true;
	ProjectileCollision->OnComponentBeginOverlap.AddDynamic(this, &AVer2_Projectile::OnOverlap);
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

	MinDamage = 3.0f;
	MaxDamage = 5.0f;

}

// Called when the game starts or when spawned
void AVer2_Projectile::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();

	if (World)
	{
		Player = Cast<AFPS_ProjectCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));

	}

	const FVector StartTrace = this->GetActorLocation();

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
	
	FVector EndTrace;
	FVector FinalVelocity;

	if (Player)
	{
		// Vector from spawn to player
		FVector ToPlayer = Player->GetActorLocation() - StartTrace;
		
		FVector Up = Player->GetActorUpVector() * YVar;
		FVector UpVector = Up + Player->GetActorLocation();

		// Cross product is a vector at 90 degrees to the vector from projectile spawn to player
		FVector CrossProduct = FVector::CrossProduct(ToPlayer, Player->GetActorUpVector());
		CrossProduct = CrossProduct / CrossProduct.Size();
		CrossProduct *= XVar;	// Don't think this line is needed - line below does it, or keep this and 
								// and the line above with * YVar and remove the XVar and YVar from the line below

		EndTrace =  ToPlayer + CrossProduct * XVar + UpVector * YVar;
		
		// Maybe need to set velocity here to make projectiles spread
		if (ProjectileMovement)
		{
			EndTrace = EndTrace / EndTrace.Size();

			FVector MovementFactor = FVector(0.f);

			if (Player->GetVelocity().Size() > 450)
			{
				// Make it a leading shot if player is moving
				MovementFactor = Player->GetVelocity() / 4;
			}

			FinalVelocity = (MovementFactor + Up + CrossProduct + Player->GetActorLocation()) - StartTrace;
			FinalVelocity /= FinalVelocity.Size();

			ProjectileMovement->Velocity = FinalVelocity * ProjectileMovement->InitialSpeed;

		}

	}

	if (RifleParticles)
	{
		RifleParticles->SetBeamSourcePoint(0, StartTrace, 0);
		RifleParticles->SetBeamTargetPoint(0, EndTrace, 0);
	}
	
}

// Called every frame
void AVer2_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVer2_Projectile::OnOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
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



