// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameMode.h"


void ANetGameMode::BeginPlay()
{
	Super::BeginPlay();
	

	UE_LOG(LogTemp, Warning, TEXT("HELLO WORLD!!"));

	if (FParse::Value(FCommandLine::Get(), TEXT("servername="), serverName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Server Name is: %s"), *serverName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Server Name NOT PROVIDED: %s"), *serverName);
	}
	
}

