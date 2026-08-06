// Fill out your copyright notice in the Description page of Project Settings.


#include "NetWeapon.h"
#include "NetProjectile.h"


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
	
}

// Called every frame
void ANetWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ANetWeapon::SetWeaponParameters(int32 incomingID)
{
	if (!HasAuthority())
	{
		return;
	}

	shooterID = incomingID;
}


void ANetWeapon::Shoot()
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("ANetWeapon::Shoot() Has to run AT SERVER | ID: % i | % s"), shooterID, *GetDebugName(this));
		return;
	}

	if (!projectileTemplate)
	{
		UE_LOG(LogTemp, Error, TEXT("ANetWeapon::Shoot() Projectile TEMPLATE is NOT VALID | ID: % i | % s"), shooterID, *GetDebugName(this));
		return;
	}

	ANetProjectile* spawnedProjectile = theWorld->SpawnActor<ANetProjectile>(projectileTemplate, rootComp->GetComponentLocation() + this->GetActorForwardVector() * ProjectileSpawnOffset, rootComp->GetComponentRotation(), FActorSpawnParameters());
	spawnedProjectile->SetProjectileParams(shooterID);
}