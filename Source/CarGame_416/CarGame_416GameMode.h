// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MyEnums.h"
#include "GameFramework/GameModeBase.h"
#include "CarGame_416GameMode.generated.h"

class ACarGame_PlayerController;

UCLASS(MinimalAPI)
class ACarGame_416GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACarGame_416GameMode();
	UPROPERTY()
		EGameMode ThisGameMode = EGameMode::MAINMENU;
	UPROPERTY()
		TArray<ACarGame_PlayerController*> Clients;

	UFUNCTION()
		void RemoveWidgetsBeforeTravel();
};



