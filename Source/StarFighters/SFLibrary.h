// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

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

/**
 * 
 */
class STARFIGHTERS_API SFLibrary
{
public:
	SFLibrary();
	~SFLibrary();

	// static FString TestFunc() { return FString("SF Library is WORKING!!"); }
	// Called as SFLibrary::TestFunc()r
};
