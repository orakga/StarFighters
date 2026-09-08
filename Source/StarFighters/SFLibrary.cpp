// Fill out your copyright notice in the Description page of Project Settings.


#include "SFLibrary.h"


SFLibrary::SFLibrary()
{
}

SFLibrary::~SFLibrary()
{
}

float SFLibrary::BoundHeadingAngle(float inHeading)
{
	// Keep angle in the -180 ~ +180 range

	while (inHeading > 180) { inHeading -= 360; }

	while (inHeading < -180) { inHeading += 360; }

	return inHeading;

}


float SFLibrary::GetAngleBetweenVectors(FVector vector1, FVector vector2)
{
	vector1.Normalize();
	vector2.Normalize();

	float dotCalculated = FMath::Clamp( FVector::DotProduct(vector1, vector2), -1.f, 1.f);

	return FMath::RadiansToDegrees( FMath::Acos(dotCalculated) );
}


float SFLibrary::GetHeadingFromDirection(FVector direction)
{
	direction.Z = 0.f; // FLATTEN along Z-axis
	
	return BoundHeadingAngle( direction.Rotation().Yaw ); // ANGLE
}