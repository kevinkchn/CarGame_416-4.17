// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameState.h"
#include "UnrealNetwork.h"
#include "Engine/Engine.h"

AMyGameState::AMyGameState()
{
	bReplicates = true;
}

void AMyGameState::SendChatMessage(const FText& Content)
{
	Server_SendChatMessage(Content);
}
void AMyGameState::Server_SendChatMessage_Implementation(const FText& Content)
{
	NetMulticast_SendChatMessage(Content);
}
bool AMyGameState::Server_SendChatMessage_Validate(const FText& Content)
{
	return true;
}
void AMyGameState::NetMulticast_SendChatMessage_Implementation(const FText& Content)
{
	UpdateChat(Content);
}
bool AMyGameState::NetMulticast_SendChatMessage_Validate(const FText& Content)
{
	return true;
}

