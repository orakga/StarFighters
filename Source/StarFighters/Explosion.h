// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Explosion.generated.h"

UCLASS()
class STARFIGHTERS_API AExplosion : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosion();

	void Detonate(float coreRadius, float outerRadius, int32 baseDamage, float impactForce, int32 shooterID);

protected:

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnExplosionVFX(float coreRadius, float outerRadius);


private:

	bool IsDamageTarget(AActor* actorToCheck);
	float ComputeFalloff(float distance, float coreRadius, float outerRadius);
	
};
