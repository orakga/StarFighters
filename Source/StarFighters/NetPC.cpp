// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPC.h"
#include "GameFramework/PlayerState.h"


void ANetPC::BeginPlay()
{

	bShowMouseCursor = true;

	if (PlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("ANetPC::BeginPlay() | PC Name: %s | PS Name: %s | PID: %i"), *GetName(), *PlayerState->GetPlayerName(), PlayerState->GetPlayerId());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ANetPC::BeginPlay() | PC Name: %s | NO PlayerState"), *GetName());
	}
}


void ANetPC::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (PlayerState)
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPC::OnRep_PlayerState() | PC Name: %s | PS Name: %s | PID: %i"), *GetName(), *PlayerState->GetPlayerName(), PlayerState->GetPlayerId());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPC::OnRep_PlayerState() | PC Name: %s | NO PlayerState"), *GetName());
	}

}

void ANetPC::MessageToClient_Implementation(const FString& message)
{
	UE_LOG(LogTemp, Error, TEXT("ANetPC::MessageToClient() | %s"), *message);
}