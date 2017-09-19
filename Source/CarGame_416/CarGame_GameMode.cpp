// Fill out your copyright notice in the Description page of Project Settings.

#include "CarGame_GameMode.h"
#include "MyGameState.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"


ACarGame_GameMode::ACarGame_GameMode()
{
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Path/To/Your/Blueprint_Class"));
	//if (PlayerPawnBPClass.Succeeded()) DefaultPawnClass = PlayerPawnBPClass.Class;
	GameStateClass = AMyGameState::StaticClass();
}

AActor* ACarGame_GameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	UWorld* World = GetWorld();
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		APlayerStart* Start = *It;
		if (Start && Start->PlayerStartTag == FName(TEXT("Driver")))
		{
			return Start;
		}
	}
	return nullptr;
}

