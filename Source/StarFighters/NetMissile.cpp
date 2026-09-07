// Fill out your copyright notice in the Description page of Project Settings.


#include "NetMissile.h"
#include "NetPawn.h"
#include "SFLibrary.h"
#include "Explosion.h"
#include "SFGameplayAttributes.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"


ANetMissile::ANetMissile()
{
	myGameplayAttributes = CreateDefaultSubobject<USFGameplayAttributes>(TEXT("MyGameplayAttributes"));

}


void ANetMissile::BeginPlay()
{
	Super::BeginPlay();

	// SetActorTickInterval( 0.1f );

	if (HasAuthority())
	{
		if (isHoming)
		{
			// Look for SCANNING VOLUME
			TArray<UActorComponent*> triggerComponents;
			GetComponents<UActorComponent>(triggerComponents); // Grab ALL actor components

			for (UActorComponent* comp : triggerComponents)
			{
				if (comp->ComponentHasTag(TEXT("Trigger")))
				{
					triggerComp = Cast<UPrimitiveComponent>(comp);
					break; // Accept the FIRST trigger we find with the correct tag
				}
			}

			if(!triggerComp) UE_LOG(LogTemp, Error, TEXT("ANetMissile::BeginPlay() no TRIGGER component found | %s"), *GetDebugName(this));

			// INIITALIZE parameters for scanning
			scanObjectParams.AddObjectTypesToQuery(ECC_Pawn);
			scanObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
			scanObjectParams.AddObjectTypesToQuery(ECC_PhysicsBody);
			scanComponentParams.AddIgnoredActor(this);
		}
	}

}

void ANetMissile::Tick(float DeltaTime)
{
	// ==== HOMING acquisition / streeting happens BEFORE base Tick(), so that Move() can use updated heading
	if (isHoming)
	{
		if (targetAcquired)
		{
			// Then CHASE the TARGET
		}
		else if (HasAuthority())
		{
			ScanForTarget();
		}

	}


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
			theWorld,
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

void ANetMissile::Multicast_BroadcastScanResult_Implementation(FVector Location, float size, FColor color)
{
	if (HasAuthority()) return;

	DrawDebugSphere(
		theWorld,
		Location,
		size,
		16,
		color,
		false,
		0.1f,
		0,
		1);

	if (rootComp)
	{
		DrawDebugLine(
			theWorld,
			Location,
			rootComp->GetComponentLocation(),
			color,
			false,
			0.1f,
			0,
			4);
	}

}


void ANetMissile::ScanForTarget()
{
	// Identify ALL actors in SCANNING VOLUME (Trigger)

	theWorld->ComponentOverlapMulti(
		scanResults,
		triggerComp,
		triggerComp->GetComponentLocation(),
		triggerComp->GetComponentRotation(),
		scanComponentParams,
		scanObjectParams);

	AActor* closestCandidate = nullptr;

	for (FOverlapResult overlapResult : scanResults)
	{
		// Check Each actor for:
		AActor* newCandidate = overlapResult.GetActor();

		if (IsValidHomingTarget(newCandidate))
		{
			// Multicast_BroadcastScanResult(newCandidate->GetActorLocation(), 120, FColor::Green); // DEBUG ONLY =========================

			if (!closestCandidate)
			{
				closestCandidate = newCandidate;
			}
			// Is it the CLOSEST valid target?
			else if ((newCandidate->GetActorLocation() - GetActorLocation()).Size()		// Distance to THIS candidate
				< (closestCandidate->GetActorLocation() - GetActorLocation()).Size()	// Distance to (previous) CLOSEST candidate
				)
			{
				// If so, CHOOSE that as your NEW TARGET
				closestCandidate = newCandidate;
			}
		}
		else
		{
			// Multicast_BroadcastScanResult(newCandidate->GetActorLocation(), 150, FColor::Purple); // DEBUG ONLY =========================
		}
	}

	if (closestCandidate)
	{
		// ACQUIRE this NEW TARGET
		UE_LOG(LogTemp, Error, TEXT("ANetMissile::ScanForTarget() TARGET FOUND | %s"), *GetDebugName(this));
		Multicast_BroadcastScanResult(closestCandidate->GetActorLocation(), 170, FColor::Red); // DEBUG ONLY =========================
	}
	else
	{
		// Multicast_BroadcastScanResult(GetActorLocation(), 100, FColor::Yellow); // DEBUG ONLY =========================
	}

}


bool ANetMissile::IsValidHomingTarget(AActor* potentialTarget)
{
	// Make sure the target is VALID, and not ourself
	if ( !IsValid(potentialTarget) || potentialTarget == this ) return false;

	// PAWNS ===========================
	ANetPawn* targetPawn = Cast<ANetPawn>(potentialTarget);
	if (targetPawn)
	{
		if (targetPawn->GetMyID() == myShooterID
			&& !canSelfDamage)
		{
			return false;
		}

		return IsInsideSensorArea(potentialTarget);

	}


	// MISSILES ========================
	if (tracksMissile)
	{

		ANetMissile* targetMissile = Cast<ANetMissile>(potentialTarget);
		if (targetMissile)
		{
			if(!targetMissile->IsInitalized()) return false;

			if(targetMissile->GetMyShooterID() == myShooterID) return false;

			return IsInsideSensorArea(potentialTarget);
		}

	}

	// =========== ADD NEW TYPES HERE ========================

	return false;

}


bool ANetMissile::IsInsideSensorArea(AActor* potentialTarget)
{
	FVector vector2Target = potentialTarget->GetActorLocation() - GetActorLocation();

	// Is the DISTANCE to TARGET less than our SENSOR RANGE?
	if( vector2Target.Size() > sensorRange ) return false;


	// Is the ANGLE to the TARGET within our SENSOR CONE ANGLE?
	if (SFLibrary::GetAngleBetweenVectors(vector2Target, GetActorForwardVector())
		> sensorConeAngle * 0.5f)
	{
		return false;
	}

	return true;

}



void ANetMissile::DestroyProjectile(bool reachedTarget)
{
	if (reachedTarget)
	{
		// Spawn FULL explosion
		AExplosion* spawnedExplosion = theWorld->SpawnActor<AExplosion>(GetActorLocation(), FRotator(), FActorSpawnParameters());
		if (spawnedExplosion) spawnedExplosion->Detonate(coreRadius, outerRadius, baseExplosionDamage, explosionImpactForce, myShooterID);
	}
	else
	{
		// Spawn SMALLER explosion
		AExplosion* spawnedExplosion = theWorld->SpawnActor<AExplosion>(GetActorLocation(), FRotator(), FActorSpawnParameters());
		if (spawnedExplosion) spawnedExplosion->Detonate(coreRadius_small, outerRadius_small, baseExplosionDamage_small, explosionImpactForce_small, myShooterID);

	}

	Super::DestroyProjectile(reachedTarget);
}


void ANetMissile::DisplayHealth()
{
	DrawDebugString(
		theWorld,
		GetActorLocation() + FVector(0.f, 0.f, -100.f),
		FString::FromInt(health),
		nullptr,
		FColor::Yellow,
		0.f,
		true,
		1.f);
}