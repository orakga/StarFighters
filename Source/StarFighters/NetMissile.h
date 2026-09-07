// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetProjectile.h"
#include "Engine/OverlapResult.h"
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
	virtual void DestroyProjectile(bool reachedTarget) override;
	void SetSyncTimer(float nextSyncTime) { timeLeftToSync = nextSyncTime; }

	UFUNCTION(NetMulticast, Reliable)
		void BroadcastDamage(int32 newHealth, int32 damage);

protected:

	virtual void Move(float DeltaTime) override;
	virtual void ManageNetSync(float DeltaTime); // New, MISSILE-specific net-sync function
	
	UFUNCTION(NetMulticast, Unreliable)
		void Multicast_BroadcastState(FVector Location, FRotator Rotation, FVector Velocity);

	// DEBUGGING ONLY ============================
	UFUNCTION(NetMulticast, Unreliable)
		void Multicast_BroadcastScanResult(FVector Location, float size, FColor color);

	void ScanForTarget();
	bool IsValidHomingTarget(AActor* potentialTarget);
	bool IsInsideSensorArea(AActor* potentialTarget);

	void DisplayHealth();

	UPROPERTY(EditAnywhere)
		bool isHoming = false;

	UPROPERTY(EditAnywhere)
		bool tracksMissile = false;

	UPROPERTY(EditAnywhere)
		float thrust = 0;

	UPROPERTY(EditAnywhere)
		float maxSpeed = 200;
	
	UPROPERTY(EditAnywhere)
		float sensorRange = 1000;

	UPROPERTY(EditAnywhere)
		float sensorConeAngle = 60.f;

	UPROPERTY(EditAnywhere)
		float coreRadius = 200;
	
	UPROPERTY(EditAnywhere)
		float outerRadius = 400;
	
	UPROPERTY(EditAnywhere)
		float baseExplosionDamage = 50;
	
	UPROPERTY(EditAnywhere)
		float explosionImpactForce = 500;
	
	UPROPERTY(EditAnywhere)
		float coreRadius_small = 100;
	
	UPROPERTY(EditAnywhere)
		float outerRadius_small = 200;
	
	UPROPERTY(EditAnywhere)
		float baseExplosionDamage_small = 20;
	
	UPROPERTY(EditAnywhere)
		float explosionImpactForce_small = 200;
			
	UPROPERTY(EditAnywhere)
		float missileLifespan = 30.0f;

	UPROPERTY(Replicated, EditAnywhere)
		int32 health = 10;

	UPROPERTY(Replicated)
		int32 maxHealth;

	class USFGameplayAttributes* myGameplayAttributes;
	UPrimitiveComponent* triggerComp = nullptr;
	FCollisionObjectQueryParams scanObjectParams;
	FComponentQueryParams scanComponentParams;
	TArray<FOverlapResult> scanResults;

	bool targetAcquired = false;

	float timeLeftToSync = 0.1f;

};
