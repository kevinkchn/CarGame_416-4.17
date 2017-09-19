// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CarGame_416GameMode.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerStart.h"
#include "MyEnums.h"
#include "AsymmetricRace_GameModeBase.generated.h"

class ACarGame_PlayerController;
class ACheckpoint;
class AMyPlayerStart;
/**
 * 
 */
 /**
 *
 */
UCLASS()
class CARGAME_416_API AAsymmetricRace_GameModeBase : public ACarGame_416GameMode
{
	GENERATED_BODY()

public:
	AAsymmetricRace_GameModeBase();
	TArray<APlayerStart*> PlayerStarts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, noclear, Category = "Class Types")
		TSubclassOf<class APawn> DefaultDriverClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, noclear, Category = "Class Types")
		TSubclassOf<class APawn> DefaultNonDriverClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, noclear, Category = "Class Types")
		TSubclassOf<class APawn> DefaultSpectatorClass;
	UPROPERTY()
		TArray<ACheckpoint*> Checkpoints;
	
	/////////GameModeBase overrides///////////
	AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	void InitStartSpot_Implementation(AActor* StartSpot, AController* NewPlayer) override;
	UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	UPROPERTY()
		bool bAllPlayersStarted = false;
	UPROPERTY()
		int PlayerCount = 0;
private:
	
protected:
		virtual void GenericPlayerInitialization(AController * C) override;


};
