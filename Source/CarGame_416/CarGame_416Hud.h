// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CarGame_416Hud.generated.h"

UCLASS(config = Game)
class ACarGame_416Hud : public AHUD
{
	GENERATED_BODY()

public:
	ACarGame_416Hud();

	/** Font used to render the vehicle info */
	UPROPERTY()
	UFont* HUDFont;

	// Begin AHUD interface
	virtual void DrawHUD() override;
	// End AHUD interface

};
