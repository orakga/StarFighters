// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosion.h"
#include "SFGameplayAttributes.h"
#include "NetPawn.h"
#include "NetMissile.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"


// Sets default values
AExplosion::AExplosion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	USceneComponent* root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(root);
}


void AExplosion::Detonate(float coreRadius, float outerRadius, int32 baseDamage, float impactForce, int32 shooterID)
{
	if (!HasAuthority()) return;

	coreRadius = FMath::Max(0.0f, coreRadius);
	outerRadius = FMath::Max(coreRadius, outerRadius);

	// Find ALL ACTORS within our OUTER Blast Radius

	TArray< TEnumAsByte< EObjectTypeQuery > > objectTypes;
	
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(this);

	TArray<AActor*> overlappedActors;

	UKismetSystemLibrary::SphereOverlapActors(
		this,
		GetActorLocation(),
		outerRadius,
		objectTypes,
		nullptr,
		actorsToIgnore,
		overlappedActors
		);

	// Look at EACH actor within Blast Radius, and DETERMINE:

	for (AActor* overlappedActor : overlappedActors)
	{
		if (!IsValid(overlappedActor)) continue; // SKIPS to next member in array

		FVector directionToVictim = overlappedActor->GetActorLocation() - GetActorLocation();
		float distanceToVictim = directionToVictim.Size();
		float falloff = ComputeFalloff(distanceToVictim, coreRadius, outerRadius);

		// APPLY DAMAGE ================================
		if (IsDamageTarget(overlappedActor))
		{
			USFGameplayAttributes* victimAttributes = overlappedActor->FindComponentByClass<USFGameplayAttributes>();
			if (victimAttributes)
			{
				// Calculate HOW MUCH DAMAGE it should receive
				int32 damageToApply = (int32) ((float) baseDamage * falloff);

				// If it's a MISSILE, reduce the damage
				if(Cast<ANetMissile>(overlappedActor)) damageToApply = (int32) ( (float) damageToApply * BlastDamageToMissileMultiplier  );

				// APPLY Damage
				victimAttributes->ProcessDamage(damageToApply, shooterID, this);

				UE_LOG(LogTemp, Error, TEXT("AExplosion::Detonate() dist: %.0f | fall: %.2f | dmg: %d | %s -> %s"), distanceToVictim, falloff, damageToApply, *GetDebugName(this), *GetDebugName(overlappedActor));
			}
		}

		// APPLY IMPACT ===============================
		UPrimitiveComponent* victimRoot = Cast<UPrimitiveComponent>(overlappedActor->GetRootComponent());

		if ( victimRoot && victimRoot->IsSimulatingPhysics())
		{
			// Calculate HOW MUCH IMPACT it should receive
			// APPLY Impact

			directionToVictim.Normalize();
			victimRoot->AddImpulse(directionToVictim * impactForce * falloff, NAME_None, true);

			// If it's a MISSILE, for a quick NetSync now
			ANetMissile* victimMissile = Cast<ANetMissile>(overlappedActor);
			if(victimMissile) victimMissile->SetSyncTimer(0.1);

			UE_LOG(LogTemp, Warning, TEXT("AExplosion::Detonate() impactFull: %.0f | fall: %.2f | impact: %.0f | %s -> %s"), impactForce, falloff, impactForce * falloff, *GetDebugName(this), *GetDebugName(overlappedActor));
		}

	}


	// SPAWN VISUAL EFFECT (at CLIENTS)
	Multicast_SpawnExplosionVFX(coreRadius, outerRadius);


	// DESTROY itself
	Destroy();

}


bool AExplosion::IsDamageTarget(AActor* actorToCheck)
{
	if (Cast<ANetPawn>(actorToCheck))
	{
		return true;
	}

	if (Cast<ANetMissile>(actorToCheck))
	{
		return true;
	}


	// == ADD NEW Actor Types Here ===========================

	return false;
}


float AExplosion::ComputeFalloff(float distance, float coreRadius, float outerRadius)
{
	if( distance <= coreRadius ) return 1.0f;

	if (distance >= outerRadius ) return 0.0f;

	if ( ( outerRadius - coreRadius ) <= 0 ) return 0.0f;

	return 1.0f - ( ( distance - coreRadius ) / ( outerRadius - coreRadius ) );


}


void AExplosion::Multicast_SpawnExplosionVFX_Implementation(float coreRadius, float outerRadius)
{
	UE_LOG(LogTemp, Warning, TEXT("AExplosion::Multicast_SpawnExplosionVFX() core: %.0f | outer: %.0f"), coreRadius, outerRadius);

	if(HasAuthority()) return;

	// Draw DEBUG SPHERE for CORE ================================
	DrawDebugSphere(
		GetWorld(),
		GetActorLocation(),
		coreRadius,
		16,
		FColor::Red,
		false,
		2.5f,
		0,
		2);


	// Draw DEBUG SPHERE for OUTER BLAST AREA ================================
	DrawDebugSphere(
		GetWorld(),
		GetActorLocation(),
		outerRadius,
		16,
		FColor::Yellow,
		false,
		4.f,
		0,
		1);

}