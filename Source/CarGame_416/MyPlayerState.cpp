// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerState.h"
#include "UnrealNetwork.h"

AMyPlayerState::AMyPlayerState()
{
	bReplicates = true;
	if (GetWorld() && GetWorld()->IsServer())
	{
		IsReady = true;
	}
}

void AMyPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	DOREPLIFETIME(AMyPlayerState, MyRole);
	DOREPLIFETIME(AMyPlayerState, IsReady);
	DOREPLIFETIME(AMyPlayerState, bIsFinished);
	DOREPLIFETIME(AMyPlayerState, bIsIt);
	DOREPLIFETIME(AMyPlayerState, LastCheckpoint);
}
void AMyPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	AMyPlayerState* MyState = Cast<AMyPlayerState>(PlayerState);

	if (MyState)
	{
		MyState->MyRole = MyRole;
		MyState->bIsIt = bIsIt;
	}
}
