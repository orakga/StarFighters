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

	// SetActorTickInterval((float)1 / (float)MissileTickFrequency);
}

void ANetMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		ManageNetSync(DeltaTime);
	}
	else
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

	SetActorRotation(currentVelocity.Rotation());


	if (currentVelocity.Size() < maxSpeed)
	{
		rootComp->AddForce( GetActorForwardVector() * thrust, NAME_None, true );
	}
	else if (currentVelocity.Size() > maxSpeed * 1.2) // if the missile is MORE THAN 20% FASTER than the MAX SPEED
	{
		// UE_LOG(LogTemp, Warning, TEXT("ANetMissile::Move() SLOW DOWN =====  %f / %f | %s"), currentVelocity.Size(), maxSpeed, *GetDebugName(this));
		rootComp->SetPhysicsLinearVelocity(currentVelocity * ( 1 - DeltaTime * 0.2f));
	}

	// UE_LOG(LogTemp, Display, TEXT("ANetMissile::Move() %f / %f | %s"), currentVelocity.Size(), maxSpeed, *GetDebugName(this));
}


void ANetMissile::ManageNetSync(float DeltaTime)
{
	timeLeftToSync -= DeltaTime;
	if (timeLeftToSync <= 0)
	{
		Multicast_BroadcastState(GetActorLocation(), GetActorRotation(), rootComp->GetPhysicsLinearVelocity());

		timeLeftToSync += (float)1 / (float) MissileSyncFrequency;

		UE_LOG(LogTemp, Display, TEXT("ANetMissile::ManageNetSync() SYNCED | %s"), *GetDebugName(this));
	}

	missileLifespan -= DeltaTime;
	if (missileLifespan <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("ANetMissile::ManageNetSync() Missile EXPIRED | %s"), *GetDebugName(this));

		DestroyProjectile(false);
	}
}


void ANetMissile::Multicast_BroadcastState_Implementation(FVector Location, FRotator Rotation, FVector Velocity)
{
	// TECH DEBT: ROTATION can be compressed to HEADING instead ======================

	if(HasAuthority()) return;

	if (rootComp)
	{
		SetActorLocationAndRotation(Location, Rotation);
		rootComp->SetAllPhysicsLinearVelocity(Velocity);
	}

}


void ANetMissile::DestroyProjectile(bool reachedTarget)
{
	if (reachedTarget)
	{
		// Spawn FULL explosion
		AExplosion* spawnedExplosion = GetWorld()->SpawnActor<AExplosion>(GetActorLocation(), FRotator(), FActorSpawnParameters());
		if (spawnedExplosion) spawnedExplosion->Detonate(coreRadius, outerRadius, baseExplosionDamage, explosionImpactForce, myShooterID);
	}
	else
	{
		// Spawn SMALLER explosion
		AExplosion* spawnedExplosion = GetWorld()->SpawnActor<AExplosion>(GetActorLocation(), FRotator(), FActorSpawnParameters());
		if (spawnedExplosion) spawnedExplosion->Detonate(coreRadius_small, outerRadius_small, baseExplosionDamage_small, explosionImpactForce_small, myShooterID);

	}

	Super::DestroyProjectile(reachedTarget);
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