// Fill out your copyright notice in the Description page of Project Settings.

#include "Transition_GameModeBase.h"
#include "NetWorkedGameInstance.h"

ATransition_GameModeBase::ATransition_GameModeBase()
{
	bUseSeamlessTravel = true;

	
}

void ATransition_GameModeBase::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();

	if (GetWorld() && GetWorld()->IsServer())
	{
		((UNetWorkedGameInstance*)GetWorld()->GetGameInstance())->TravelToNextLevel();
	}
}




