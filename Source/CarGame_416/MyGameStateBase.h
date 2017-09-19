// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MyEnums.h"
#include "MyGameStateBase.generated.h"

class ACarGame_416Pawn;
class ACheckpoint;

/**
 * 
 */

UCLASS()
class CARGAME_416_API AMyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
public:
	AMyGameStateBase();

	UFUNCTION()
		void SendChatMessage(const FText& Content);
	UFUNCTION(BlueprintImplementableEvent, Category = Chat)
		void BP_UpdateChat(const FText& Content, ChatColor Color);
	UFUNCTION(BlueprintCallable, Category = Lobby)
		void FromBP_AdminChatMessage(const FText& Content, ChatColor Color);

	/* Start Game From Lobby */
	UFUNCTION(BlueprintCallable, Category = Lobby)
		void FromBP_StartGame();
	UFUNCTION(BlueprintCallable, Category = Lobby)
		void FromBP_AttemptStartGame(FString MapName, EGameMode Mode);
	UFUNCTION(BlueprintCallable, Category = Race)
		void FromBP_StartRace();
	UFUNCTION(BlueprintImplementableEvent, Category = Lobby)
		void BP_StartGameFailed(FailMessage Failure);
	UFUNCTION(BlueprintImplementableEvent, Category = Lobby)
		void BP_BeginStartGameCountdown();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void NetMulticast_SetGameMode(EGameMode GM);

	UPROPERTY(Replicated, BlueprintReadOnly)
		EGameMode CurGameMode;
	
	
	/* Race Gameplay */
	UFUNCTION(BlueprintImplementableEvent, Category = Race)
		void BP_StartRaceCountdown(float ServerTimeDelta);
	UFUNCTION()
		void StartRaceCountdown();
	UFUNCTION()
		void PassCheckpoint(AController* PC, int CPNum);

	UPROPERTY()
		FString Map;
	UPROPERTY()
		FString GameModeURL;
	UPROPERTY()
		EGameMode NextGameMode;
	UPROPERTY(BlueprintReadOnly)
		TArray<FString> MapNames;


	////// Game Stats
	UFUNCTION(Client, Reliable, WithValidation)
		void Client_UpdatePlace(int place);
	UFUNCTION(Client, Reliable, WithValidation)
		void Client_UpdateLap(int lap);


	UPROPERTY()
		int Place = 1;
	UPROPERTY()
		int Lap = 1;
	UPROPERTY()
		TArray<ACheckpoint*> Checkpoints;
	UPROPERTY(Replicated)
		int NumCheckpoints = 0;

	//////TAG Gameplay
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tag)
		TSubclassOf<class ACarGame_416Pawn> ItClass;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tag)
		TSubclassOf<class ACarGame_416Pawn> NotItClass;
	UPROPERTY()
		bool bRaceEnabled = false;


private:
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void NetMulticast_SendChatMessage(const FText& Content, ChatColor Color);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void NetMulticast_StartRaceCountdown();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void NetMulticast_StartRace();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void NetMulticast_FinishGame();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void NetMulticast_SetMapAndMode();
	UFUNCTION()
		void FinishGameTimerEvent();
	FTimerHandle FinishGameTimerHandle;

public:
	/////////TAG Gameplay funcs
	
	
	
};
