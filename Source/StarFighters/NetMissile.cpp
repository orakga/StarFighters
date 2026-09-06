// Fill out your copyright notice in the Description page of Project Settings.


#include "NetMissile.h"
#include "SFLibrary.h"
#include "Explosion.h"
#include "SFGameplayAttributes.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"


ANetMissile::ANetMissile()
{
	myGameplayAttributes = CreateDefaultSubobject<USFGameplayAttributes>(TEXT("MyGameplayAttributes"));

}


void ANetMissile::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickInterval( (float) 1 / (float) MissileTickFrequency );
}

void ANetMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority())
	{
		DisplayHealth();
	}
}

void ANetMissile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ANetMissile, health, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ANetMissile, maxHealth, COND_InitialOnly);
}


void ANetMissile::SetProjectileParams(int32 shooterID)
{
	Super::SetProjectileParams(shooterID);

	maxHealth = health;
	myGameplayAttributes->InitializeAttributes(myShooterID, health, maxHealth);

}

void ANetMissile::BroadcastDamage_Implementation(int32 newHealth, int32 damage)
{
	health = newHealth;

	if (!HasAuthority())
	{
		DrawDebugString(
			GetWorld(),
			GetActorLocation() + FVector(150 + FMath::RandRange(-30, 30), FMath::RandRange(-30, 30), 0),
			FString::Printf(TEXT("%i"), damage),
			nullptr,
			FColor::Orange,
			5.0f,
			true,
			1.f
		);
	}
}


void ANetMissile::Move(float DeltaTime)
{
	Super::Move(DeltaTime);

	FVector currentVelocity = rootComp->GetPhysicsLinearVelocity();

	if (currentVelocity.Size() < maxSpeed)
	{
		rootComp->AddForce( GetActorForwardVector() * thrust, NAME_None, true );
	}

	// UE_LOG(LogTemp, Display, TEXT("ANetMissile::Move() %f / %f | %s"), currentVelocity.Size(), maxSpeed, *GetDebugName(this));
}


void ANetMissile::DestroyProjectile()
{
	AExplosion* spawnedExplosion = GetWorld()->SpawnActor<AExplosion>(GetActorLocation(), FRotator(), FActorSpawnParameters());
	if(spawnedExplosion) spawnedExplosion->Detonate(coreRadius, outerRadius, baseExplosionDamage, explosionImpactForce, myShooterID);


	Super::DestroyProjectile();
}


void ANetMissile::DisplayHealth()
{
	DrawDebugString(
		GetWorld(),
		GetActorLocation() + FVector(0.f, 0.f, -100.f),
		FString::FromInt(health),
		nullptr,
		FColor::Yellow,
		0.f,
		true,
		1.f);
}