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

	ANetMissile();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetProjectileParams(int32 shooterID) override;
	virtual void DestroyProjectile() override;

	UFUNCTION(NetMulticast, Reliable)
		void BroadcastDamage(int32 newHealth, int32 damage);

protected:

	virtual void Move(float DeltaTime) override;
	
	void DisplayHealth();

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

	UPROPERTY(Replicated, EditAnywhere)
		int32 health = 10;

	UPROPERTY(Replicated)
		int32 maxHealth;

	class USFGameplayAttributes* myGameplayAttributes;

	
};
