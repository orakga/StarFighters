// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameMode.h"


void ANetGameMode::BeginPlay()
{
	Super::BeginPlay();
	

	if (FParse::Value(FCommandLine::Get(), TEXT("servername="), serverName))
	{
		UE_LOG(LogTemp, Warning, TEXT("ANetGameMode::BeginPlay() Server Name is: %s"), *serverName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ANetGameMode::BeginPlay() ERROR | Server Name NOT PROVIDED: %s"), *serverName);
	}
	
}

