// Fill out your copyright notice in the Description page of Project Settings.

#include "AsymmetricRace_GameModeBase.h"
#include "MyGameStateBase.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "MyPlayerState.h"
#include "CarGame_PlayerController.h"
#include "NetWorkedGameInstance.h"
#include "Engine/Engine.h"
#include "Checkpoint.h"
#include "MyPlayerStart.h"


AAsymmetricRace_GameModeBase::AAsymmetricRace_GameModeBase()
{
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Path/To/Your/Blueprint_Class"));
	//if (PlayerPawnBPClass.Succeeded()) DefaultPawnClass = PlayerPawnBPClass.Class;
	bUseSeamlessTravel = true;

	ThisGameMode = EGameMode::RACETAG;

	GameStateClass = AMyGameStateBase::StaticClass();
	PlayerStateClass = AMyPlayerState::StaticClass();
	if (GetWorld())
	{
		UWorld* World = GetWorld();

		//Get PlayerStates
		for (TActorIterator<APlayerStart> It(World); It; ++It)
		{
			APlayerStart* Start = *It;
			if (Start)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Debug Message")));
				PlayerStarts.Add(Start);
			}
		}

		//Get All Checkpoints
		for (TActorIterator<ACheckpoint> It(World); It; ++It)
		{
			ACheckpoint* Check = *It;
			if (Check)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Debug Message")));
				Checkpoints.Add(Check);
			}
		}
	}
		
}
void AAsymmetricRace_GameModeBase::GenericPlayerInitialization(AController * C)
{
	Super::GenericPlayerInitialization(C);
	///Save Ref to playercontrollers after travel

	ACarGame_PlayerController* Player = Cast<ACarGame_PlayerController>(C);
	Clients.Add(Player);
	if (Clients.Num() == GetNumPlayers())
	{
		((AMyGameStateBase*)(GetWorld()->GetGameState()))->StartRaceCountdown();
	}
	


}
void AAsymmetricRace_GameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);



}

void AAsymmetricRace_GameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	(ACarGame_PlayerController*)Exiting;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("PlayerExiting")));
}

AActor* AAsymmetricRace_GameModeBase::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	
	/*if (GetWorld())
	{
		PlayerCount++;

		if (((UNetWorkedGameInstance*)GetWorld()->GetGameInstance())->PlayerControllers.Num() == PlayerCount)
		{
			((AMyGameStateBase*)(GetWorld()->GetGameState()))->StartRaceCountdown();
		}
	}*/

	AMyPlayerState* MyState = Cast<AMyPlayerState>(Player->PlayerState);
	for (APlayerStart* It : PlayerStarts)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Debug Message")));
		AMyPlayerStart* Start = CastChecked<AMyPlayerStart>(It);
		if (Start)
		{
			
			if (MyState->MyRole == PlayerRole::Driver && !Start->bIsOccupied)
			{
				if (Start->PlayerStartTag == FName(TEXT("Driver")))
				{
					return Start;
				}
			}
			else if (MyState->MyRole == PlayerRole::NonDriver || MyState->MyRole == PlayerRole::Spectator)
			{
				if (Start->PlayerStartTag == FName(TEXT("NonDriver")))
				{
					return Start;
				}
			}
			else if (MyState->MyRole == PlayerRole::Lobby)
			{
				if (Start->PlayerStartTag == FName(TEXT("NonDriver")))
				{
					return Start;
				}
			}
		}
	}
	
	return GetWorld()->GetWorldSettings();
}

void AAsymmetricRace_GameModeBase::InitStartSpot_Implementation(AActor* StartSpot, AController* NewPlayer)
{
	AMyPlayerStart* Start = Cast<AMyPlayerStart>(StartSpot);
	Start->bIsOccupied = true;
}

UClass* AAsymmetricRace_GameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	AMyPlayerState* MyState = Cast<AMyPlayerState>(InController->PlayerState);
	if (MyState->MyRole == PlayerRole::Driver)
	{
		if (DefaultDriverClass)
			return DefaultDriverClass;
	}
	else if (MyState->MyRole == PlayerRole::NonDriver)
	{
		if (DefaultNonDriverClass)
			return DefaultNonDriverClass;
		((AMyPlayerState*)(((ACarGame_PlayerController*)InController)->PlayerState))->bIsIt = true;
		((ACarGame_PlayerController*)InController)->bIsIt = true;
	}
	else if (MyState->MyRole == PlayerRole::Spectator)
	{
		if (DefaultSpectatorClass)
			return DefaultSpectatorClass;
	}
	return DefaultPawnClass;
}



