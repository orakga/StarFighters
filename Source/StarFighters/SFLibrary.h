// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SFLibrary.generated.h"

#define MouseAimTraceChannel ECC_GameTraceChannel1

#define StarfieldAreaLeft -10000
#define StarfieldAreaRight 10000
#define StarfieldAreaFront -10000
#define StarfieldAreaBack 10000
#define StarfieldAreaBottom -10000
#define StarfieldAreaTop -500
#define StarfieldAreaQuantity 500

#define PlayerNameMaxLength 12

#define UserInputUpdateFrequency 30
#define ShipStateUpdateFrequency 10

#define ShipLinearDamping 0.5
#define ShipAngularDamping 0.5

#define FixedCameraLocation FVector(-1000, 0, 1200)
#define FixedCameraRotation FRotator(-45, 0, 0)

#define ProjectileSpawnOffset 110
#define ProjectileAreaWidth 10000
#define ProjectileAreaHeight 10000

#define MoveInputDeadzone 0.5
#define AimInputDeadzone 0.5


// STRUCTS ==========================================================

USTRUCT(BlueprintType)
struct FPlayerInputState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float moveHeading = 0.f;

	UPROPERTY(BlueprintReadOnly)
	bool isMoveInputActive = false;

	UPROPERTY(BlueprintReadOnly)
	float aimHeading = 0.f;

	UPROPERTY(BlueprintReadOnly)
	bool isAimInputActive = false;
};


/**
 * 
 */
class STARFIGHTERS_API SFLibrary
{
public:
	SFLibrary();
	~SFLibrary();

	static float BoundHeadingAngle(float inHeading);
};
