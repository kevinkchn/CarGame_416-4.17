// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyEnums.h"
#include "MyPlayerState.generated.h"

/**
 * 
 */


UCLASS()
class CARGAME_416_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMyPlayerState();

	UPROPERTY(Replicated)
		PlayerRole MyRole = PlayerRole::Driver;
	UPROPERTY(Replicated)
		bool IsReady = false;
	UPROPERTY(Replicated)
		bool bIsFinished = false;
	UPROPERTY(Replicated, BlueprintReadOnly)
		bool bIsIt = false;
	UPROPERTY(Replicated)
		int LastCheckpoint = 1;

protected:
	virtual void CopyProperties(APlayerState* PlayerState) override;


	
};
