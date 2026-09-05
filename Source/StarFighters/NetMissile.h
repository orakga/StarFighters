// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetProjectile.h"
#include "NetMissile.generated.h"

/**
 * 
 */
UCLASS()
class STARFIGHTERS_API ANetMissile : public ANetProjectile
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

protected:

	virtual void Move(float DeltaTime) override;

	virtual void DestroyProjectile() override;
	
	UPROPERTY(EditAnywhere)
		float thrust = 0;

	UPROPERTY(EditAnywhere)
		float maxSpeed = 200;
	
	UPROPERTY(EditAnywhere)
		float coreRadius = 200;
	
	UPROPERTY(EditAnywhere)
		float outerRadius = 400;
	
	UPROPERTY(EditAnywhere)
		float baseExplosionDamage = 50;
	
	UPROPERTY(EditAnywhere)
		float explosionImpactForce = 500;

	
};
