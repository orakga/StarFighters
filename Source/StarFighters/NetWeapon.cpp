// Fill out your copyright notice in the Description page of Project Settings.


#include "NetWeapon.h"
#include "NetProjectile.h"
#include "NetWeaponBarrel.h"


// Sets default values
ANetWeapon::ANetWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;

}

// Called when the game starts or when spawned
void ANetWeapon::BeginPlay()
{
	Super::BeginPlay();

	rootComp = (UPrimitiveComponent*) (this->GetRootComponent());
	theWorld = GetWorld();

	chargesReady = initialCharges;

	if (isAutomatic || hasCooldown || canStoreCharges)
	{
		if (HasAuthority())
		{
			SetActorTickEnabled(true);
		}
	}
	
}

// Called every frame
void ANetWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CooldownManagement(DeltaTime);

	if (isAutomatic && isTriggerDown)
	{
		Shoot();
	}

}

void ANetWeapon::EnableClientTick_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("ANetWeapon::EnableClientTick() TICKING NOW ===== | % s"), *GetDebugName(this));

	SetActorTickEnabled(true);
}

void ANetWeapon::SetBarrel_Left(ANetWeaponBarrel* inBarrel)
{
	if (!inBarrel)
	{
		UE_LOG(LogTemp, Error, TEXT("ANetWeapon::SetBarrel_Left() INVALID Barrel Pointer | % s"), *GetDebugName(this));
		return;
	}

	barrelLeft = inBarrel;
	muzzleLeft = inBarrel->GetMuzzleComponent();

}

void ANetWeapon::SetBarrel_Right(ANetWeaponBarrel* inBarrel)
{
	if (!inBarrel)
	{
		UE_LOG(LogTemp, Error, TEXT("ANetWeapon::SetBarrel_Right() INVALID Barrel Pointer | % s"), *GetDebugName(this));
		return;
	}

	barrelRight = inBarrel;
	muzzleRight = inBarrel->GetMuzzleComponent();

}


void ANetWeapon::SetWeaponParameters(int32 incomingID)
{
	if (!HasAuthority())
	{
		return;
	}

	shooterID = incomingID;
}

void ANetWeapon::Trigger(bool isActive)
{
	if (!HasAuthority())
	{
		return;
	}

	isTriggerDown = isActive;

	if (isActive)  // Shoot button was JUST PRESSED
	{
		// UE_LOG(LogTemp, Warning, TEXT("ANetWeapon::Trigger() Shooter ID: % i | PRESSED | % s"), shooterID, *GetDebugName(this));

		Shoot();
	}
	else
	{
		// UE_LOG(LogTemp, Display, TEXT("ANetWeapon::Trigger() Shooter ID: % i | RELEASED | % s"), shooterID, *GetDebugName(this));
	}

}


void ANetWeapon::Shoot()
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("ANetWeapon::Shoot() Has to run AT SERVER | ID: % i | % s"), shooterID, *GetDebugName(this));
		return;
	}

	if (!IsReadyToShoot())
	{
		return;
	}

	if (!projectileTemplate)
	{
		UE_LOG(LogTemp, Error, TEXT("ANetWeapon::Shoot() Projectile TEMPLATE is NOT VALID | ID: % i | % s"), shooterID, *GetDebugName(this));
		return;
	}

	// ========== DETERMINE Shot Location/Direction ======================

	FVector muzzleLocation;
	FRotator muzzleRotation;

	if (bShootLeftNext)
	{
		if (muzzleLeft)
		{
			muzzleLocation = muzzleLeft->GetComponentLocation();
			muzzleRotation = muzzleLeft->GetComponentRotation();
		}
	}
	else
	{
		if (muzzleRight)
		{
			muzzleLocation = muzzleRight->GetComponentLocation();
			muzzleRotation = muzzleRight->GetComponentRotation();
		}
	}

	muzzleLocation = FVector(muzzleLocation.X, muzzleLocation.Y, 0); // FLATTEN location to Z = 0 plane

	ANetProjectile* spawnedProjectile = theWorld->SpawnActor<ANetProjectile>(projectileTemplate, muzzleLocation, muzzleRotation, FActorSpawnParameters());
	spawnedProjectile->SetProjectileParams(shooterID);

	// ======================= POST-SPAWN clean-up ===========================

	if (isDualBarrel)
	{
		bShootLeftNext = !bShootLeftNext;
	}

	if (canStoreCharges)
	{
		chargesReady--;
	}

	if (hasCooldown && cooldownTime > 0.f)
	{
		timeSinceWeaponShot = 0.f;
	}

	ClientCooldownReset(chargesReady);
}


void ANetWeapon::ClientCooldownReset_Implementation(int32 currentCharges)
{
	chargesReady = currentCharges;

	if (hasCooldown && cooldownTime > 0.f)
	{
		timeSinceWeaponShot = 0.f;
	}
}


void ANetWeapon::CooldownManagement(float deltaTime)
{
	// Manage COOLDOWN TIME ================================================
	if (hasCooldown && cooldownTime > 0.f)
	{
		if (timeSinceWeaponShot < cooldownTime)
		{
			timeSinceWeaponShot += deltaTime;

			if (timeSinceWeaponShot >= cooldownTime)
			{
				timeSinceWeaponShot = cooldownTime;

				// UE_LOG(LogTemp, Display, TEXT("ANetWeapon::CooldownManagement() WEAPON READY | ID: % i | % s"), shooterID, *GetDebugName(this));
			}
		}
	}


	// Manage REFILLING CHARGES ============================================
	if (canStoreCharges && maxCharges > 0 && chargesReady < maxCharges)
	{
		timeSinceRecharge += deltaTime;

		if (timeSinceRecharge > rechargeTime)
		{
			chargesReady++;
			timeSinceRecharge = 0;

			// UE_LOG(LogTemp, Warning, TEXT("ANetWeapon::CooldownManagement() CHARGE ADDED | ID: % i | % s | Charges: %d / %d"), shooterID, *GetDebugName(this), chargesReady, maxCharges);
		}
	}
}

bool ANetWeapon::IsReadyToShoot()
{
	if (!isTriggerDown)
	{
		return false;
	}

	if (hasCooldown && cooldownTime > 0.f && timeSinceWeaponShot < cooldownTime)
	{
		return false;
	}

	if (canStoreCharges && chargesReady <= 0)
	{
		return false;
	}

	return true;
}