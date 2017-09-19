// Fill out your copyright notice in the Description page of Project Settings.

#include "Lobby_GameMode.h"
#include "CarGame_PlayerController.h"
#include "NetWorkedGameInstance.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.h"
#include "Engine/Engine.h"


ALobby_GameMode::ALobby_GameMode()
{
	bUseSeamlessTravel = true;
	ThisGameMode = EGameMode::LOBBY;
}
void ALobby_GameMode::GenericPlayerInitialization(AController * C)
{
	Super::GenericPlayerInitialization(C);

	ACarGame_PlayerController* Player = Cast<ACarGame_PlayerController>(C);
	PlayerRole NewRole;
	//
	Clients.Add(Player);

	//save PC's in gameinstance for use after switching maps
	((UNetWorkedGameInstance*)GetWorld()->GetGameInstance())->PlayerControllers.Add(Player);

	//assign at least 1 NonDriver, and the rest to Driver team.
	/*if (NonDriverTeam.Num() == 0)
	{
	NonDriverTeam.Add(Player);
	NewRole = PlayerRole::NonDriver;
	}
	else
	{*/
	DriverTeam.Add(Player);
	NewRole = PlayerRole::Driver;
	//}
	Player->JoinLobby(NewRole);
	AMyPlayerState* MPS = (AMyPlayerState*)Player->PlayerState;
	MPS->MyRole = NewRole;
	MPS->bIsIt = false;
	for (ACarGame_PlayerController* PC : DriverTeam)
	{
		if (PC != Player)
		{ ///change back to name
			PC->AddToLobby(NewRole, Player->PlayerState->PlayerName);
			Player->AddToLobby(PlayerRole::Driver, PC->PlayerState->PlayerName);
		}

	}
	/*for (ACarGame_PlayerController* PC : NonDriverTeam)
	{
	if (PC != Player)
	{
	PC->AddToLobby(NewRole, Player->PlayerState->PlayerName);
	Player->AddToLobby(PlayerRole::NonDriver, PC->PlayerState->PlayerName);
	}
	}*/
}
void ALobby_GameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}
void ALobby_GameMode::SwitchTeam(ACarGame_PlayerController* PC, PlayerRole prole)
{

}
bool ALobby_GameMode::TryToSwitchTeams(ACarGame_PlayerController* PC, PlayerRole prole)
{
	if ((prole == PlayerRole::NonDriver && DriverTeam.Num() <= NonDriverTeam.Num()))
	{
		return false;
	}
	for (ACarGame_PlayerController* Con : DriverTeam)
	{
		if (Con == PC && prole == PlayerRole::NonDriver)
		{
			DriverTeam.Remove(PC);
			NonDriverTeam.Add(PC);
			return true;
		}
	}
	for (ACarGame_PlayerController* Con : NonDriverTeam)
	{
		DriverTeam.Add(PC);
		NonDriverTeam.Remove(PC);
		return true;
	}
	return false;
}




