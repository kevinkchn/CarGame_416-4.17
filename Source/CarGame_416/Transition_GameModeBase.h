// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CarGame_416GameMode.h"
#include "Transition_GameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class CARGAME_416_API ATransition_GameModeBase : public ACarGame_416GameMode
{
	GENERATED_BODY()
	
public:
	ATransition_GameModeBase();

	void PostSeamlessTravel() override;
	
	
	
	
};
