// Fill out your copyright notice in the Description page of Project Settings.


#include "NetMissile.h"
#include "SFLibrary.h"
#include "Explosion.h"


void ANetMissile::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickInterval( (float) 1 / (float) MissileTickFrequency );
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