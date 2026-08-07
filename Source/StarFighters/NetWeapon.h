// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFLibrary.h"
#include "NetWeapon.generated.h"

UCLASS()
class STARFIGHTERS_API ANetWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANetWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetWeaponParameters(int32 incomingID);

	void Trigger(bool isActive);

	UPROPERTY(EditAnywhere)
		TSubclassOf<class ANetProjectile> projectileTemplate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		bool isAutomatic = false;

	UPROPERTY(EditAnywhere)
		bool hasCooldown = false;

	UPROPERTY(EditAnywhere)
		float cooldownTime = 0.25f;

	UPROPERTY(EditAnywhere)
		bool canStoreCharges = false;

	UPROPERTY(EditAnywhere)
		float rechargeTime = 2.0f;

	UPROPERTY(EditAnywhere)
		int32 initialCharges = 0;

	UPROPERTY(EditAnywhere)
		int32 maxCharges = 5;

private:

	void CooldownManagement(float deltaTime);
	bool IsReadyToShoot();
	void Shoot();

	UPrimitiveComponent* rootComp;
	UWorld* theWorld;

	int32 shooterID = -1;

	bool isTriggerDown = false;

	float timeSinceWeaponShot = 0.f;
	float timeSinceRecharge = 0.f;

	int32 chargesReady = 0;

};
