// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "UnrealNetwork.h"
#include "MyGameState.generated.h"

/**
 * 
 */
UCLASS()
class CARGAME_416_API AMyGameState : public AGameState
{
	GENERATED_BODY()

public:
		AMyGameState();

		UFUNCTION(BlueprintCallable, Category = Chat)
			void SendChatMessage(const FText& Content);
		UFUNCTION(BlueprintImplementableEvent, Category = Chat)
			void UpdateChat(const FText& Content);
private:
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_SendChatMessage(const FText& Content);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void NetMulticast_SendChatMessage(const FText& Content);

	
	
};
