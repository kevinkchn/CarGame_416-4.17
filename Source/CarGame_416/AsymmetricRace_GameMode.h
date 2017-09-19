// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CarGame_GameMode.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerStart.h"
#include "MyPlayerStart.h"
#include "AsymmetricRace_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class CARGAME_416_API AAsymmetricRace_GameMode : public ACarGame_GameMode
{
	GENERATED_BODY()
	
public:
	AAsymmetricRace_GameMode();
	TArray<APlayerStart*> PlayerStarts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, noclear, Category = "Class Types")
		TSubclassOf<class APawn> DefaultDriverClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, noclear, Category = "Class Types")
		TSubclassOf<class APawn> DefaultNonDriverClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, noclear, Category = "Class Types")
		TSubclassOf<class APawn> DefaultSpectatorClass;

	/////////GameModeBase overrides//////////
	/*
	AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;
	void InitStartSpot_Implementation(AActor* StartSpot, AController* NewPlayer) override;
	UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;*/
	
};
