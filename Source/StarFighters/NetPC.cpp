// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPC.h"
#include "NetGameMode.h"
#include "GameFramework/PlayerState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/World.h"


void ANetPC::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;

	theGameMode = Cast<ANetGameMode>(UGameplayStatics::GetGameMode(this->GetWorld()));

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
	Server_SpawnAndPossess();
}

void ANetPC::Server_SpawnAndPossess_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("ANetPC::Server_SpawnAndPossess() SPAWNING | %s (PID: %d)"), *GetName(), PlayerState->GetPlayerId());

	if (GetPawn())
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPC::Server_SpawnAndPossess() Pawn already exists | %s (PID: %d)"), *GetName(), PlayerState->GetPlayerId());
		return;
	}

	FVector spawnLocation = FVector(FMath::RandRange(-300, 500), FMath::RandRange(-1000,1000), 0);
	FTransform spawnTransform = FTransform(FRotator(), spawnLocation);
	APawn* newShip = theGameMode->SpawnDefaultPawnAtTransform(this, spawnTransform);

	if (!newShip)
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPC::Server_SpawnAndPossess() NO NEW SHIP! | %s (PID: %d)"), *GetName(), PlayerState->GetPlayerId());
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("ANetPC::Server_SpawnAndPossess() NEW SHIP SPAWNED! | %s (PID: %d)"), *GetName(), PlayerState->GetPlayerId());

	Possess(newShip);

	if (GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("ANetPC::Server_SpawnAndPossess() I HAVE A SHIP! OOOOOOOOOOOOO | %s (PID: %d)"), *GetName(), PlayerState->GetPlayerId());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ANetPC::Server_SpawnAndPossess() NO SHIP! XXXXXXXXXXXXX | %s (PID: %d)"), *GetName(), PlayerState->GetPlayerId());
	}
}

void ANetPC::ReturnToMenu()
{
	UE_LOG(LogTemp, Error, TEXT("ANetPC::ReturnToMenu() | %s"), *GetName());
	ClientTravel("MainMenu", ETravelType::TRAVEL_Absolute);
}
