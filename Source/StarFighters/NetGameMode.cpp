// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameMode.h"
#include "SFLibrary.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "NetPC.h"
#include "GameFramework/GameStateBase.h"


ANetGameMode::ANetGameMode()
{
	bStartPlayersAsSpectators = true; // Prevents spawning Pawns immediately upon login
}

void ANetGameMode::BeginPlay()
{
	Super::BeginPlay();

	// UE_LOG(LogTemp, Error, TEXT("ANetGameMode::BeginPlay() message from SFLibrary: %s"), *SFLibrary::TestFunc());

	if (FParse::Value(FCommandLine::Get(), TEXT("servername="), serverName))
	{
		UE_LOG(LogTemp, Warning, TEXT("ANetGameMode::BeginPlay() Server Name is: %s"), *serverName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ANetGameMode::BeginPlay() ERROR | Server Name NOT PROVIDED: %s"), *serverName);
	}
	
}

void ANetGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	UE_LOG(LogTemp, Warning, TEXT("1) ANetGameMode::PreLogin() | Addr: %s | Options: %s"), *Address, *Options);

	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}


APlayerController* ANetGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	UE_LOG(LogTemp, Warning, TEXT("2-A) ANetGameMode::Login() | Options: %s"), *Options);

	APlayerController* newController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);

	UE_LOG(LogTemp, Warning, TEXT("2-B) ANetGameMode::Login() | PC Name: %s | PS Name: %s | ID: %i | Options: %s"), *newController->GetName(), *newController->PlayerState->GetPlayerName(), newController->PlayerState->GetPlayerId(),  *Options);

	return newController;
}


void ANetGameMode::PostLogin(APlayerController* NewPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("5-A) ANetGameMode::PostLogin() | PC Name: %s | PS Name: %s | ID: %i"), *NewPlayer->GetName(), *NewPlayer->PlayerState->GetPlayerName(), NewPlayer->PlayerState->GetPlayerId());

	Super::PostLogin(NewPlayer);

	UE_LOG(LogTemp, Warning, TEXT("5-B) ANetGameMode::PostLogin() | PC Name: %s | PS Name: %s | ID: %i"), *NewPlayer->GetName(), *NewPlayer->PlayerState->GetPlayerName(), NewPlayer->PlayerState->GetPlayerId());

	// Send the Server Name to the new Client
	ANetPC* newPC = Cast<ANetPC>(NewPlayer);
	newPC->MessageToClient("ANetGameMode::PostLogin() | Welcome to: " + serverName);
}


APlayerController* ANetGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
	UE_LOG(LogTemp, Warning, TEXT("3) ANetGameMode::SpawnPlayerController() | Options: %s"), *Options);

	APlayerController* newController = Super::SpawnPlayerController(InRemoteRole, Options);

	return newController;
}


FString ANetGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	UE_LOG(LogTemp, Warning, TEXT("4-A) ANetGameMode::InitNewPlayer() | PC Name: %s | PS Name: %s | ID: %i | Options: %s"), *NewPlayerController->GetName(), *NewPlayerController->PlayerState->GetPlayerName(), NewPlayerController->PlayerState->GetPlayerId(), *Options);

	FString ErrorMessage = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	UE_LOG(LogTemp, Warning, TEXT("4-B) ANetGameMode::InitNewPlayer() | PC Name: %s | PS Name: %s | ID: %i | Options: %s"), *NewPlayerController->GetName(), *NewPlayerController->PlayerState->GetPlayerName(), NewPlayerController->PlayerState->GetPlayerId(), *Options);

	// Capture Player Name from URL ======
	FString RequestedName = UGameplayStatics::ParseOption(Options, TEXT("PlayerName")).Left(PlayerNameMaxLength);
	ChangeName(NewPlayerController, RequestedName, true);

	return ErrorMessage;
}


void ANetGameMode::Logout(AController* Exiting)
{
	APlayerController* exitingPlayer = Cast<APlayerController>(Exiting);
	UE_LOG(LogTemp, Warning, TEXT("X) ANetGameMode::Logout() | PC Name: %s | PS Name: %s | ID: %i"), *exitingPlayer->GetName(), *exitingPlayer->PlayerState->GetPlayerName(), exitingPlayer->PlayerState->GetPlayerId());

	Super::Logout(Exiting);
}
