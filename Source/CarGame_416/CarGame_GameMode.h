// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerStart.h"
#include "CarGame_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class CARGAME_416_API ACarGame_GameMode : public AGameMode
{
	GENERATED_BODY()

		
	
public:
	ACarGame_GameMode();

	AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;
	TArray<APlayerStart*> OccupiedPlayerStarts;
};
