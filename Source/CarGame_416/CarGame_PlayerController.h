// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UnrealNetwork.h"
#include "MyEnums.h"
#include "CarGame_PlayerController.generated.h"

class ACarGame_416Pawn;
/**
 * 
 */

UCLASS()
class CARGAME_416_API ACarGame_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACarGame_PlayerController();



	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Ref)
		ACarGame_416Pawn* CarPawn;
	

	//////////////////chat/////////////////////
	UFUNCTION(BlueprintCallable, Category = Chat)
		void SendChatMessage(const FText& Content);

	UFUNCTION(BlueprintCallable, Category = Roles)
		void FromBP_SwitchTeam(PlayerRole prole);
	UFUNCTION(BlueprintCallable, Category = Lobby)
		void FromBP_ReadyUp();
	UFUNCTION(BlueprintImplementableEvent, Category = Roles)
		void BP_SwitchTeamFailed(FailMessage Fail);
	UFUNCTION(BlueprintImplementableEvent, Category = Roles)
		void BP_SwitchTeam(PlayerRole prole, const FString& Name);
	UFUNCTION(BlueprintImplementableEvent, Category = Roles)
		void BP_AddSelfToLobby(PlayerRole prole, const FString& Name);
	UFUNCTION(BlueprintImplementableEvent, Category = Roles)
		void BP_AddPlayerToLobby(PlayerRole prole, const FString& Name);
		
	UFUNCTION()
		void JoinLobby(PlayerRole prole);
	UFUNCTION()
		void AddToLobby(PlayerRole prole, const FString& Name);
	UFUNCTION(Client, Reliable, WithValidation)
		void Client_RemoveWidgets();
	UFUNCTION(BlueprintImplementableEvent, Category = UI)
		void HideWidgets();

	UFUNCTION()
		void EnterPressed();
	UFUNCTION(BlueprintImplementableEvent)
		void BP_EnterPressed();
	UFUNCTION()
		void EscapePressed();
	UFUNCTION(BlueprintImplementableEvent)
		void BP_EscapePressed();

	UFUNCTION()
		void StartRacing();
	UFUNCTION(Client, Reliable, WithValidation)
		void Client_EndRacing(bool Victory);
	UFUNCTION(Client, Reliable, WithValidation)
		void Client_FinishGame();
	UFUNCTION(BlueprintImplementableEvent)
		void BP_EndRacing(bool Victory);
	UFUNCTION(BlueprintImplementableEvent)
		void BP_FinishGame();

	/////Tag Gameplay
	UFUNCTION()
		void TagSwitch(TSubclassOf<ACarGame_416Pawn> SwitchClass, FTransform SwitchTransform, AActor* DestroyActor, bool IsIt, FVector AngVel, FVector LinVel);
	UFUNCTION(BlueprintImplementableEvent, Category = Tag)
		void BP_Tag(bool IsIt);
	UFUNCTION(Client, Reliable, WithValidation)
		void Client_Tag(bool IsIt);
	UPROPERTY()
		bool bCanRace = false;
	UPROPERTY()
		bool bIsIt = false;
	UPROPERTY()
		bool bIsFinished = false;

private:
	
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_SendChatMessage(const FText& Content);
	UFUNCTION(Client, Reliable, WithValidation)
		void Client_AddPlayerToLobby(PlayerRole prole, const FString& Name);
	UFUNCTION(Client, Reliable, WithValidation)
		void Client_JoinLobby(PlayerRole prole, const FString& Name);
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_SwitchTeam(PlayerRole prole);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void NetMulticast_SwitchTeam(PlayerRole prole, const FString& Name);
	UFUNCTION(Client, Reliable, WithValidation)
		void Client_SwitchTeamFailed(FailMessage Fail);
	
	
	



protected:
	
	//virtual void SetPawn(APawn* InPawn) override;
	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaSeconds) override;

};
