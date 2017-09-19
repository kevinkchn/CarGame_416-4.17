// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CarGame_416GameMode.h"
#include "MyEnums.h"
#include "Lobby_GameMode.generated.h"

class ACarGame_PlayerController;
/**
 * 
 */
UCLASS()
class CARGAME_416_API ALobby_GameMode : public ACarGame_416GameMode
{
	GENERATED_BODY()
public:

	ALobby_GameMode();
	virtual void PostLogin(APlayerController* NewPLayer) override;
	UPROPERTY()
	TArray<ACarGame_PlayerController*> DriverTeam;
	UPROPERTY()
	TArray<ACarGame_PlayerController*> NonDriverTeam;

	UFUNCTION()
		void SwitchTeam(ACarGame_PlayerController* PC, PlayerRole prole);

	UFUNCTION()
		bool TryToSwitchTeams(ACarGame_PlayerController* PC, PlayerRole prole);



protected:
	UFUNCTION()
		virtual void GenericPlayerInitialization(AController * C) override;
};
