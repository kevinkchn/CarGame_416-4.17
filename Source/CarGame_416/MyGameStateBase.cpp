// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameStateBase.h"
#include "UnrealNetwork.h"
#include "MyPlayerState.h"
#include "Engine/Engine.h"
#include "Lobby_GameMode.h"
#include "AsymmetricRace_GameModeBase.h"
#include "Classes/Kismet/GameplayStatics.h"
#include "CarGame_416Pawn.h"
#include "CarGame_PlayerController.h"
#include "NetWorkedGameInstance.h"
#include "CarGame_416GameMode.h"
#include "Checkpoint.h"
#include "CarGame_416GameMode.h"


AMyGameStateBase::AMyGameStateBase()
{
	bReplicates = true;
	
	if (GetWorld())
	{
		if (GetWorld()->IsServer() && GetWorld()->GetAuthGameMode())
		{
			ACarGame_416GameMode* GM = (ACarGame_416GameMode*)GetWorld()->GetAuthGameMode();
			CurGameMode = GM->ThisGameMode;

			//// Initialize Properties for each GameMode
			switch(CurGameMode)
			{
				case EGameMode::RACETAG:
					Checkpoints = ((AAsymmetricRace_GameModeBase*)GM)->Checkpoints;
					NumCheckpoints = Checkpoints.Num();
				case EGameMode::HIDEANDSEEK:
					Checkpoints = ((AAsymmetricRace_GameModeBase*)GM)->Checkpoints;
					NumCheckpoints = Checkpoints.Num();
				case EGameMode::FREEZETAG:
					Checkpoints = ((AAsymmetricRace_GameModeBase*)GM)->Checkpoints;
					NumCheckpoints = Checkpoints.Num();
				default: break;
			}
			
		}
	}

	//// Add Map names to list
	MapNames.Add("TestRaceTagMap");
}
void AMyGameStateBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	DOREPLIFETIME(AMyGameStateBase, CurGameMode);
	DOREPLIFETIME(AMyGameStateBase, NumCheckpoints);

}
void AMyGameStateBase::SendChatMessage(const FText& Content)
{

	NetMulticast_SendChatMessage(Content, ChatColor::PLAYER);
}
void AMyGameStateBase::NetMulticast_SendChatMessage_Implementation(const FText& Content, ChatColor Color)
{
	BP_UpdateChat(Content, Color);
}
bool AMyGameStateBase::NetMulticast_SendChatMessage_Validate(const FText& Content, ChatColor Color)
{
	return true;
}
void AMyGameStateBase::StartRaceCountdown()
{
	NetMulticast_StartRaceCountdown();
}
void AMyGameStateBase::NetMulticast_StartRaceCountdown_Implementation()
{

	BP_StartRaceCountdown(11-ServerWorldTimeSecondsDelta);
}
bool AMyGameStateBase::NetMulticast_StartRaceCountdown_Validate()
{
	return true;
}

void AMyGameStateBase::NetMulticast_StartRace_Implementation()
{
	if (GetWorld())
	{
		bRaceEnabled = true;
		((ACarGame_PlayerController*)UGameplayStatics::GetPlayerController(GetWorld(), 0))->StartRacing();
	}
}
bool AMyGameStateBase::NetMulticast_StartRace_Validate()
{
	return true;
}

void AMyGameStateBase::FromBP_StartRace()
{

	if(GetWorld()->IsServer())
		NetMulticast_StartRace();
}

void AMyGameStateBase::FromBP_StartGame()
{

	if (GetWorld() && GetWorld()->IsServer())
	{	
		
		TArray<APlayerState*> PlayerStates = PlayerArray;
		for (APlayerState* State : PlayerStates)
		{

			if (!((AMyPlayerState*)State)->IsReady)
			{
				//BP_StartGameFailed(FailMessage::PLAYERS_NOT_READY);
				return;
			}
		}
		AMyPlayerState* ItState = (AMyPlayerState*)PlayerArray[FMath::RandRange(0, PlayerArray.Num()-1)];
		ItState->bIsIt = true;
		ItState->MyRole = PlayerRole::NonDriver;
		((ACarGame_416GameMode*)GetWorld()->GetAuthGameMode())->RemoveWidgetsBeforeTravel();
		
		((UNetWorkedGameInstance*)GetWorld()->GetGameInstance())->TravelToTransitionLevel();

	}
}
void AMyGameStateBase::FromBP_AttemptStartGame(FString MapName, EGameMode Mode)
{
	if (GetWorld() && GetWorld()->IsServer())
	{
		TArray<APlayerState*> PlayerStates = PlayerArray;
		for (APlayerState* State : PlayerStates)
		{
			if (!((AMyPlayerState*)State)->IsReady)
			{
				BP_StartGameFailed(FailMessage::PLAYERS_NOT_READY);
				return;
			}
		}
		((UNetWorkedGameInstance*)GetWorld()->GetGameInstance())->NextMapName = MapName;
		((UNetWorkedGameInstance*)GetWorld()->GetGameInstance())->NextGameMode = Mode;
		/*Map = MapName;
		NextGameMode = Mode;

		switch (NextGameMode)
		{
		case EGameMode::RACETAG:
			GameModeURL= "AsymmetricRace_GameModeBaseBP.AsymmetricRace_GameModeBaseBP";
		case EGameMode::HIDEANDSEEK:
			GameModeURL = "AsymmetricRace_GameModeBaseBP.AsymmetricRace_GameModeBaseBP";
		case EGameMode::FREEZETAG:
			GameModeURL = "AsymmetricRace_GameModeBaseBP.AsymmetricRace_GameModeBaseBP";
		}*/

		BP_BeginStartGameCountdown();
	}
}

void AMyGameStateBase::NetMulticast_SetMapAndMode_Implementation()
{
	if (GetWorld())
	{
		((UNetWorkedGameInstance*)GetWorld()->GetGameInstance())->NextGameMode;
	}
}
bool AMyGameStateBase::NetMulticast_SetMapAndMode_Validate()
{
	return true;
}

void AMyGameStateBase::NetMulticast_FinishGame_Implementation()
{
	((ACarGame_PlayerController*)UGameplayStatics::GetPlayerController(GetWorld(), 0))->Client_FinishGame();
}
bool AMyGameStateBase::NetMulticast_FinishGame_Validate()
{
	return true;
}
void AMyGameStateBase::FromBP_AdminChatMessage(const FText& Content, ChatColor Color)
{
	NetMulticast_SendChatMessage(Content, Color);
}


///gameplay stats
void AMyGameStateBase::Client_UpdatePlace_Implementation(int place)
{

}
bool AMyGameStateBase::Client_UpdatePlace_Validate(int place)
{
	return true;
}
void AMyGameStateBase::Client_UpdateLap_Implementation(int lap)
{

}
bool AMyGameStateBase::Client_UpdateLap_Validate(int lap)
{
	return true;
}

void AMyGameStateBase::NetMulticast_SetGameMode_Implementation(EGameMode GM)
{
	CurGameMode = GM;
}
bool AMyGameStateBase::NetMulticast_SetGameMode_Validate(EGameMode GM)
{
	return true;
}

void AMyGameStateBase::PassCheckpoint(AController* PC, int CPNum)
{
	ACarGame_PlayerController* CGPC = (ACarGame_PlayerController*)PC;
	AMyPlayerState* MPS = ((AMyPlayerState*)PC->PlayerState);
	AMyPlayerState* ItState;
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Num Checkpoints: %d"), Checkpoints.Num()));
	///// FINISH CONDITION
	if (CPNum == 1 && MPS->LastCheckpoint == Checkpoints.Num())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Checking Finish Condition")));
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Last Checkpoint: %d"), MPS->LastCheckpoint));
		MPS->bIsFinished = true;
		CGPC->Client_EndRacing(true);
		int i = 0;
		for (APlayerState* It : PlayerArray)
		{
			AMyPlayerState* TheState = (AMyPlayerState*)It;
			if(TheState->bIsFinished) i++;
			if (TheState->bIsIt)
			{
				ItState = TheState;
			}
		}
		if ((PlayerArray.Num() - 1) == i)
		{
			NetMulticast_FinishGame();
			GetWorld()->GetTimerManager().SetTimer(FinishGameTimerHandle, this, &AMyGameStateBase::FinishGameTimerEvent, 5.0f, false);
		}
		

	}

	///// OTHERWISE ADVANCE CHECKPOINT
	if (MPS->LastCheckpoint == CPNum - 1)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Last Checkpoint: %d"), MPS->LastCheckpoint));
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Crossed Checkpoint: %d"), CPNum));
		MPS->LastCheckpoint = CPNum;
	}
}

void AMyGameStateBase::FinishGameTimerEvent()
{
	if (GetWorld())
	{
		((ACarGame_416GameMode*)GetWorld()->GetAuthGameMode())->RemoveWidgetsBeforeTravel();
		UNetWorkedGameInstance* NWGI = (UNetWorkedGameInstance*)GetWorld()->GetGameInstance();
		NWGI->NextMapName = "Lobby";
		NWGI->NextGameMode = EGameMode::LOBBY;
		NWGI->TravelToTransitionLevel();
	}
}

