// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPC.h"
#include "GameFramework/PlayerState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


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

	if (!HasAuthority())
	{
		UEnhancedInputLocalPlayerSubsystem* SFInputSystem = Cast<ULocalPlayer>(Player)->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

		if (!IMC_Spectating)
		{
			UE_LOG(LogTemp, Error, TEXT("ANetPC::BeginPlay() IMC_Spectating in NULL | %s"), *GetName());
			return;
		}

		SFInputSystem->AddMappingContext(IMC_Spectating, 1);
		UE_LOG(LogTemp, Error, TEXT("ANetPC::BeginPlay() IMC Set to Spectating | %s"), *GetName());
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

void ANetPC::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* SFInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	if (!SFInputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPC::SetupInputComponent() Cannot GET EnhancedInputComponent | %s"), *GetName());
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("ANetPC::SetupInputComponent() EnhancedInputComponent SECURED | %s"), *GetName());

	SFInputComponent->BindAction(IA_SpawnShip, ETriggerEvent::Triggered, this, &ANetPC::SpawnShip);
	SFInputComponent->BindAction(IA_ReturnToMenu, ETriggerEvent::Triggered, this, &ANetPC::ReturnToMenu);
}

void ANetPC::SpawnShip()
{
	UE_LOG(LogTemp, Error, TEXT("ANetPC::SpawnShip() | %s"), *GetName());
}

void ANetPC::ReturnToMenu()
{
	UE_LOG(LogTemp, Error, TEXT("ANetPC::ReturnToMenu() | %s"), *GetName());

	ClientTravel("MainMenu", ETravelType::TRAVEL_Absolute);
}