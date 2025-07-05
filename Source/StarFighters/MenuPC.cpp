// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuPC.h"
#include "SFLibrary.h"

void AMenuPC::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;

	// Capture PLAYER NAME, then send to Blueprint
	if (FParse::Value(FCommandLine::Get(), TEXT("playername="), initialPlayerName))
	{
		UE_LOG(LogTemp, Error, TEXT("AMenuPC::BeginPlay() Player Name received: %s"), *initialPlayerName);
		RegisterPlayerName(initialPlayerName);
	}

	// Capture SERVER IP, then send to Blueprint
	if (FParse::Value(FCommandLine::Get(), TEXT("serverip="), initialServerIP))
	{
		UE_LOG(LogTemp, Error, TEXT("AMenuPC::BeginPlay() Server IP received: %s"), *initialServerIP);
		RegisterServerIP(initialServerIP);
	}

}


void AMenuPC::JoinGame(FString serverIP, FString myPlayerName)
{
	FString serverURL = serverIP + ":7777" + "?PlayerName=" + myPlayerName;

	UE_LOG(LogTemp, Error, TEXT("AMenuPC::BeginPlay() Traveling to : %s"), *serverURL);
	ClientTravel(serverURL, ETravelType::TRAVEL_Absolute);

}