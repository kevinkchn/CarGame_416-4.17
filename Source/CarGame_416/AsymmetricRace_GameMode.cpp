// Fill out your copyright notice in the Description page of Project Settings.

#include "AsymmetricRace_GameMode.h"
#include "MyGameState.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "MyPlayerState.h"
#include "NetWorkedGameInstance.h"
#include "Engine/Engine.h"


AAsymmetricRace_GameMode::AAsymmetricRace_GameMode()
{
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Path/To/Your/Blueprint_Class"));
	//if (PlayerPawnBPClass.Succeeded()) DefaultPawnClass = PlayerPawnBPClass.Class;
	GameStateClass = AMyGameState::StaticClass();
	PlayerStateClass = AMyPlayerState::StaticClass();
	if (GetWorld())
	{
		UWorld* World = GetWorld();
		for (TActorIterator<APlayerStart> It(World); It; ++It)
		{
			APlayerStart* Start = *It;
			if (Start)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Debug Message")));
				PlayerStarts.Add(Start);
			}
		}
	}
}

