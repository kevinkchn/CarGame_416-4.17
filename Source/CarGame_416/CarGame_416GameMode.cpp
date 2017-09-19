// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "CarGame_416GameMode.h"
#include "CarGame_416Pawn.h"
#include "CarGame_416Hud.h"
#include "MyGameStateBase.h"
#include "MyPlayerState.h"
#include "CarGame_PlayerController.h"

ACarGame_416GameMode::ACarGame_416GameMode()
{
	DefaultPawnClass = ACarGame_416Pawn::StaticClass();
	HUDClass = ACarGame_416Hud::StaticClass();
	GameStateClass = AMyGameStateBase::StaticClass();
	PlayerStateClass = AMyPlayerState::StaticClass();
	bUseSeamlessTravel = true;
}

void ACarGame_416GameMode::RemoveWidgetsBeforeTravel()
{
	for (ACarGame_PlayerController* PC : Clients)
	{
		PC->Client_RemoveWidgets();
	}
}

