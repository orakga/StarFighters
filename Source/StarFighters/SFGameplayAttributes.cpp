// Fill out your copyright notice in the Description page of Project Settings.


#include "SFGameplayAttributes.h"
#include "NetPawn.h"
#include "NetMissile.h"


// Sets default values for this component's properties
USFGameplayAttributes::USFGameplayAttributes()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void USFGameplayAttributes::BeginPlay()
{
	UE_LOG(LogTemp, Display, TEXT("USFGameplayAttributes::BeginPlay() %s"), *GetReadableName());

	Super::BeginPlay();

	myPawnPtr = Cast<ANetPawn>( GetOwner() );
	myMissilePtr = Cast<ANetMissile>( GetOwner() );

	if( !myPawnPtr.IsValid() && !myMissilePtr.IsValid() )
	{
		UE_LOG(LogTemp, Error, TEXT("USFGameplayAttributes::BeginPlay() OWNER NOT RECOGNIZED"), *GetReadableName());
	}

	
}


// Called every frame
/*
void USFGameplayAttributes::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
*/


void USFGameplayAttributes::InitializeAttributes(int32 newPlayerID, int32 initialHealth, int32 initialMaxHealth)
{
	playerID = newPlayerID;
	health = initialHealth;
	maxHealth = initialMaxHealth;

	isInitialized = true;
	isAlive = true;
}


void USFGameplayAttributes::ProcessDamage(int32 damage, int32 shooterID, AActor* damageSourceActor)
{
	// UE_LOG(LogTemp, Display, TEXT("USFGameplayAttributes::ProcessDamage() %s (%i) | Shooter: %s (%i) | Damage: %i | Health: %i "), *GetReadableName(), playerID, *damageSourceActor->GetHumanReadableName(), shooterID, damage, health);

	if( !isInitialized ) return; // SAFETY CHECK: Cannot receive damage if SFGA hasn't been initialized
	if( !isAlive ) return; // SAFETY CHECK

	int32 damageTaken = damage;
	float damageMultiplier = 1.0f;

	if (shooterID == playerID)
	{
		damageMultiplier = SelfDamageMultiplier;
		// damageTaken = 0;
	}

	damageTaken = FMath::RoundToInt( (float) damage * damageMultiplier );

	if (damageTaken > health)
	{
		damageTaken = health;
		health = 0;
	}
	else
	{
		health -= damageTaken;
	}

	health = FMath::Clamp(health, 0, maxHealth);

	// ========== IF DAMAGE was done, UPDATE at OWNER ===============
	if (damageTaken != 0)
	{
		// PAWN ==============================
		if (myPawnPtr.IsValid())
		{
			ANetPawn* myPawn = myPawnPtr.Get();
			if (myPawn)
			{
				myPawn->BroadcastDamage(health, damageTaken);
			}
		}

		// MISSILE ==============================
		if (myMissilePtr.IsValid())
		{
			ANetMissile* myMissile = myMissilePtr.Get();
			if (myMissile)
			{
				myMissile->BroadcastDamage(health, damageTaken);
			}
		}



		// ADD HERE for NEW TYPES ============================
		// ......

	}

	// ========== IF OWNER is DEAD, initiate DEATH processing ===============
	if (health <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("USFGameplayAttributes::ProcessDamage() %s (%i) | Shooter: %s (%i) | THIS ACTOR IS DEAD"), *GetReadableName(), playerID, *damageSourceActor->GetHumanReadableName(), shooterID);
		isAlive = false;

		// PAWN ==============================
		if (myPawnPtr.IsValid())
		{
			ANetPawn* myPawn = myPawnPtr.Get();
			if (myPawn)
			{
				myPawn->DestroyMe();
			}
		}

		// MISSILE ==============================
		if (myMissilePtr.IsValid())
		{
			ANetMissile* myMissile = myMissilePtr.Get();
			if (myMissile)
			{
				myMissile->DestroyProjectile();
			}
		}


		// ADD HERE for NEW TYPES ============================
		// ......

	}


}