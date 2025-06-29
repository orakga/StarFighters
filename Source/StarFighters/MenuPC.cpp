// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuPC.h"
#include "SFLibrary.h"

void AMenuPC::BeginPlay()
{
	Super::BeginPlay();

	// UE_LOG(LogTemp, Error, TEXT("AMenuPC::BeginPlay() message from SFLibrary: %s"), *SFLibrary::TestFunc());

	FString serverAddress = "192.168.1.9";
	FString serverURL = serverAddress + ":7777";

	UE_LOG(LogTemp, Error, TEXT("AMenuPC::BeginPlay() Traveling to : %s"), *serverURL);
	ClientTravel(serverURL, ETravelType::TRAVEL_Absolute);
}

