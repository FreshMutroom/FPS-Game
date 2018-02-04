// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Project.h"
#include "Sniper_Projectile.h"
#include "FPS_ProjectCharacter.h"
#include "Sniper_Bot.h"


// Sets default values
ASniper_Projectile::ASniper_Projectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileTrail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Trail PS"));

	// This may cause the contrail to just disappear instead of fading out even though this 
	// is more than enough time to let it fade out
	InitialLifeSpan = 20.f;

}

// Called when the game starts or when spawned
void ASniper_Projectile::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();

	if (World)
	{
		ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(World, 0);
		if (PlayerCharacter)
		{
			Player = Cast<AFPS_ProjectCharacter>(PlayerCharacter);
		}
		
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

	float XVar = FMath::RandRange(0.f, XVariance) * MakeNegativeX;
	float YVar = FMath::RandRange(0.f, YVariance) * MakeNegativeY;

	FVector EndTrace;
	

	if (Player)
	{
		FVector PlayerLocation = Player->GetActorLocation();
		
		// Vector from spawn to player
		FVector ToPlayer = PlayerLocation - StartTrace;

		FVector Up = Player->GetActorUpVector() * YVar;
		FVector UpVector = Up + Player->GetActorLocation();

		// Cross product is a vector at 90 degrees to the vector from projectile spawn to player
		FVector CrossProduct = FVector::CrossProduct(ToPlayer, Player->GetActorUpVector());
		CrossProduct = CrossProduct / CrossProduct.Size();
		CrossProduct *= XVar;

		EndTrace = PlayerLocation + CrossProduct + Up;
		FVector EndTraceNormalized = EndTrace / EndTrace.Size();

		if (ProjectileTrail)
		{

			ProjectileTrail->SetBeamSourcePoint(0, StartTrace, 0);
			ProjectileTrail->SetBeamTargetPoint(0, EndTrace, 0);
		}
		
		// Raycast from StartTrace towards EndTrace. Not needed
		FHitResult HitResult = FHitResult();
		FCollisionQueryParams CQP = FCollisionQueryParams();
		CQP.AddIgnoredActor(this);
		
		// Add sniper bot as ignored actor. Assumes only one sniper bot is in the world
		TArray<AActor*> SniperBot;
		UWorld* World = GetWorld();
		if (World)
		{
			UGameplayStatics::GetAllActorsOfClass(World, ASniper_Bot::StaticClass(), SniperBot);
		}
		for (AActor* Bot : SniperBot)
		{
			CQP.AddIgnoredActor(Bot);
		}

		if (World && World->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Visibility, CQP))	// may need to change ECC_Visibility since it may hit bushes etc
		{
			// Raycast hit something
			FVector HitLocation = HitResult.ImpactPoint;

		}

	}

}

// Called every frame
void ASniper_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

