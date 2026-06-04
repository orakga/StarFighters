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