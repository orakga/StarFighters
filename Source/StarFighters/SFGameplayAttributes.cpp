// Fill out your copyright notice in the Description page of Project Settings.


#include "SFGameplayAttributes.h"
#include "NetPawn.h"


// Sets default values for this component's properties
USFGameplayAttributes::USFGameplayAttributes()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void USFGameplayAttributes::BeginPlay()
{
	Super::BeginPlay();

	myPawnPtr = Cast<ANetPawn>( GetOwner() );
	
}


// Called every frame
/*
void USFGameplayAttributes::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
*/


void USFGameplayAttributes::InitializeAttributes(int32 newPlayerID, int32 initialHealth, int32 initialMaxHealth)
{
	playerID = newPlayerID;
	health = initialHealth;
	maxHealth = initialMaxHealth;
}


void USFGameplayAttributes::ProcessDamage(int32 damage, int32 shooterID, AActor* damageSourceActor)
{
	UE_LOG(LogTemp, Error, TEXT("USFGameplayAttributes::ProcessDamage() %s (%i) | Shooter: %s (%i) | Damage: %i | Health: %i "), *GetReadableName(), playerID, *damageSourceActor->GetHumanReadableName(), shooterID, damage, health);
}