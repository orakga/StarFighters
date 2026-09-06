// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetProjectile.generated.h"

UCLASS()
class STARFIGHTERS_API ANetProjectile : public AActor
{
	GENERATED_BODY()
	
private:	

	void CheckForOutOfBounds();

	UPrimitiveComponent* colliderComp;

	UPROPERTY(EditAnywhere)
		float projectileSpeed = 300;

	UPROPERTY(EditAnywhere)
		int32 projectileDamage = 10;

	UPROPERTY(EditAnywhere)
		bool canSelfDamage = false;

	UPROPERTY(EditAnywhere)
		bool canDmgProjectiles = false;

	UPROPERTY(EditAnywhere)
		bool canDmgMissiles = false;

	bool projectileInitialized = false;

protected:

	// Sets default values for this actor's properties
	ANetProjectile();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Move(float DeltaTime);

	virtual void DestroyProjectile(bool reachedTarget);

	UFUNCTION()
		void OverlapDetected(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(reliable, NetMulticast)
		void BroadcastHit();

	UPrimitiveComponent* rootComp;

	int32 myShooterID = -1;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	int32 GetMyShooterID() { return myShooterID; }
	virtual void SetProjectileParams(int32 shooterID);
	bool IsInitalized() { return projectileInitialized;}
	
	UPROPERTY(EditAnywhere)
		TSubclassOf<class AActor> hitFX_template;
};
