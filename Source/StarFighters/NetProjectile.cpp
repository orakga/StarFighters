// Fill out your copyright notice in the Description page of Project Settings.


#include "NetProjectile.h"
#include "NetPawn.h"
#include "SFGameplayAttributes.h"
#include "DrawDebugHelpers.h"


// Sets default values
ANetProjectile::ANetProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

}

// Called when the game starts or when spawned
void ANetProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	rootComp = (UPrimitiveComponent*)this->GetRootComponent();

	SetActorTickInterval(0.1);

	if (!rootComp)
	{
		UE_LOG(LogTemp, Error, TEXT("ANetProjectile::BeginPlay() rootComp is NOT VALID | %s"), *GetName());
		return;
	}

	FVector currentDirection = this->GetActorForwardVector();
	rootComp->SetAllPhysicsLinearVelocity(currentDirection * projectileSpeed);

	if (HasAuthority())
	{
		TArray<UActorComponent*> myComponents;
		this->GetComponents<UActorComponent>(myComponents);

		FName colliderTag = FName(*FString("Collider"));
		UActorComponent* componentToCheck = nullptr;
		colliderComp = nullptr;

		for (int i = 0; i < myComponents.Num(); i++)
		{
			componentToCheck = myComponents[i];

			if (componentToCheck->ComponentHasTag(colliderTag))
			{
				// UE_LOG(LogTemp, Warning, TEXT("ANetProjectile::BeginPlay() Found a Collider | %s"), *GetName());
				colliderComp = (UPrimitiveComponent*)componentToCheck;
			}

		}

		// After all tags have been checked
		if (colliderComp) // A VALID collider has been found
		{
			// BINDING the DELEGATE to the Collider
			colliderComp->OnComponentBeginOverlap.AddDynamic(this, &ANetProjectile::OverlapDetected);
			// UE_LOG(LogTemp, Warning, TEXT("ANetProjectile::BeginPlay() Binding to ANetProjectile::OverlapDetected() | %s"), *GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ANetProjectile::BeginPlay() NO COLLIDER FOUND! | %s"), *GetName());
		}

	}

}

// Called every frame
void ANetProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckForOutOfBounds();

}


void ANetProjectile::OverlapDetected(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Display, TEXT("ANetProjectile::OverlapDetected() %s | Actor: %s | Comp: %s "), *GetName(), *OtherActor->GetName(), *OtherComp->GetName());

	if (!IsInitalized())
	{
		UE_LOG(LogTemp, Error, TEXT("ANetProjectile::OverlapDetected() This Projectile is NOT INITALIZED | %s | Actor: %s | Comp: %s "), *GetName(), *OtherActor->GetName(), *OtherComp->GetName());
		return;
	}

	bool isValidHit = false;

	// Test if the other Actor was a SHIP (Pawn)
	ANetPawn* otherPawn = Cast<ANetPawn>(OtherActor);
	if (otherPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("ANetProjectile::OverlapDetected() It's a SHIP | %s | Actor: %s"), *GetName(), *OtherActor->GetName());

		if (myShooterID != otherPawn->GetMyID())  // Is this MY OWN ship or not?
		{
			UE_LOG(LogTemp, Error, TEXT("ANetProjectile::OverlapDetected() It's an ENEMY SHIP! | %s (%i) | Actor: %s (%i)"), *GetName(), myShooterID, *OtherActor->GetName(), otherPawn->GetMyID());
			isValidHit = true;
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("ANetProjectile::OverlapDetected() It's MY ship | %s (%i) | Actor: %s (%i)"), *GetName(), myShooterID, *OtherActor->GetName(), otherPawn->GetMyID());
			isValidHit = canSelfDamage;
		}
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("ANetProjectile::OverlapDetected() It's NOT A SHIP | %s | Actor: %s"), *GetName(), *OtherActor->GetName());
	}

	// Test if the other Actor was a PROJECTILE
	ANetProjectile* otherProjectile = Cast<ANetProjectile>(OtherActor);
	if (otherProjectile)
	{
		UE_LOG(LogTemp, Warning, TEXT("ANetProjectile::OverlapDetected() It's a PROJECTILE | %s | Actor: %s"), *GetName(), *OtherActor->GetName());

		if (otherProjectile->IsInitalized())
		{
			if (myShooterID != otherProjectile->GetMyShooterID()) // Is this MY OWN projectile or not?
			{
				UE_LOG(LogTemp, Error, TEXT("ANetProjectile::OverlapDetected() It's an ENEMY PROJECTILE! | %s (%i) | Actor: %s (%i)"), *GetName(), myShooterID, *OtherActor->GetName(), otherProjectile->GetMyShooterID());
				isValidHit = true;
			}
			else
			{
				UE_LOG(LogTemp, Display, TEXT("ANetProjectile::OverlapDetected() It's MY PROJECTILE | %s (%i) | Actor: %s (%i)"), *GetName(), myShooterID, *OtherActor->GetName(), otherProjectile->GetMyShooterID());
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ANetProjectile::OverlapDetected() The other Projectile is NOT INITALIZED | %s | Actor: %s | Comp: %s "), *GetName(), *OtherActor->GetName(), *OtherComp->GetName());
		}


	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("ANetProjectile::OverlapDetected() It's NOT A PROJECTILE | %s | Actor: %s"), *GetName(), *OtherActor->GetName());
	}


	// Check if there was a Valid Hit found
	if (isValidHit)
	{
		UE_LOG(LogTemp, Error, TEXT("ANetProjectile::OverlapDetected() VALID HIT | %s | Actor: %s | Comp: %s "), *GetName(), *OtherActor->GetName(), *OtherComp->GetName());
		
		USFGameplayAttributes* victimGameplayAttributes = OtherActor->FindComponentByClass<USFGameplayAttributes>();
		if (victimGameplayAttributes)
		{
			// OtherActor can receive damage
			victimGameplayAttributes->ProcessDamage(projectileDamage, myShooterID, this);
		}
		else
		{
			// OtherActor DOES NOT have a GameplayAttributes component
			UE_LOG(LogTemp, Error, TEXT("ANetProjectile::OverlapDetected() NO GameplayAttributes on the Victim | %s | Actor: %s"), *GetName(), *OtherActor->GetName());
		}
		
		BroadcastHit();
		this->Destroy();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ANetProjectile::OverlapDetected() NOT A HIT | %s | Actor: %s | Comp: %s "), *GetName(), *OtherActor->GetName(), *OtherComp->GetName());
	}
}


void ANetProjectile::SetProjectileParams(int32 shooterID)
{
	myShooterID = shooterID;
	projectileInitialized = true;
}


void ANetProjectile::BroadcastHit_Implementation()
{
	UE_LOG(LogTemp, Display, TEXT("ANetProjectile::BroadcastDamage() HIT! | %s (%i)"), *GetName(), myShooterID);

	if (HasAuthority())
	{
		return;
	}

	if(!rootComp)
	{
		UE_LOG(LogTemp, Error, TEXT("ANetProjectile::BroadcastDamage() ROOTCOMP NOT VALID! | %s (%i)"), *GetName(), myShooterID);
		return;
	}

	this->GetWorld()->SpawnActor<AActor>(hitFX_template, rootComp->GetComponentLocation(), FRotator(), FActorSpawnParameters());

	// === DEBUG LINE to show LOCATION + DIRECTION of HIT =================
	UWorld* theWorld = GetWorld();
	DrawDebugLine(theWorld, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 15.f, FColor::Red, false, 5.f, 0, 8.f);
	DrawDebugLine(theWorld, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 40.f, FColor::Red, false, 5.f, 0, 3.f);

}


void ANetProjectile::CheckForOutOfBounds()
{
	if (HasAuthority())
	{
		if (rootComp)
		{
			FVector currentLocation = rootComp->GetComponentLocation();

			if (
				currentLocation.X < -ProjectileAreaWidth / 2
				|| currentLocation.X > ProjectileAreaWidth / 2
				|| currentLocation.Y < -ProjectileAreaHeight / 2
				|| currentLocation.Y > ProjectileAreaHeight / 2
				)
			{
				// Destroy the Projectile
				UE_LOG(LogTemp, Display, TEXT("ANetProjectile::CheckForOutOfBounds() OUT OF BOUNDS | %s | %i / %i"), *GetName(), (int32) currentLocation.X, (int32) currentLocation.Y);
				this->Destroy();
			}
		}
	}

}