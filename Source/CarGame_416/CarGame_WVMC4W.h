// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "CarGame_WVMC4W.generated.h"

/**
 * 
 */
UCLASS()
class CARGAME_416_API UCarGame_WVMC4W : public UWheeledVehicleMovementComponent4W
{
	GENERATED_BODY()
	
public:
	void UpdateEngine(FVehicleEngineData newEngine);
	
	
};
