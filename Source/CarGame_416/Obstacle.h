// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "Components/TimelineComponent.h"
#include "Obstacle.generated.h"

/**
 * 
 */
UCLASS()
class CARGAME_416_API AObstacle : public AStaticMeshActor
{
	GENERATED_BODY()

		

public:
	AObstacle(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Category = Default, BlueprintReadWrite, VisibleDefaultsOnly)
		UStaticMeshComponent* ObstacleMesh;

	UPROPERTY()
		float FDelta;
	UPROPERTY()
		float FDeltaTotal = 0;
	//UFUNCTION(NetMultiCast, Reliable, WithValidation)
	//	void NetMultiCast_UpdateLocation(FVector curLoc);
	UPROPERTY(BlueprintReadOnly, Category = Default)
		FVector NextLocation;
private:

	UFUNCTION(NetMulticast, Unreliable, WithValidation)
		void NetMulticast_UpdateClientState(FVector Loc, FRotator Rot, FVector AngVel, FVector LinVel);
	UFUNCTION(NetMulticast, Unreliable, WithValidation)
		void NetMulticast_UpdatePhysics(bool PhysState);
	UFUNCTION()
		void SendClientStateUpdate();
	UFUNCTION()
		void CorrectLocation();
	UFUNCTION()
		void CorrectRotation();

	UPROPERTY()
		FVector CurLoc;

	UPROPERTY()
		FVector CorrectedLocation;
	UPROPERTY()
		FRotator CorrectedRotation;
	
	UPROPERTY()
		FRotator NextRotation;
	UPROPERTY()
		FVector NextAngVel;
	UPROPERTY()
		FVector NextLinVel;

	UFUNCTION()
		void CorrectionTimelineUpdateFloatLocation(float Val);
	UFUNCTION()
		void CorrectionTimelineUpdateFloatRotation(float Val);
	UPROPERTY()
		UCurveFloat* FCurve;
	UPROPERTY()
		UTimelineComponent* CorrectionTimelineLocation;
	UPROPERTY()
		UTimelineComponent* CorrectionTimelineRotation;

	FTimerHandle ClientStateTimerHandle;

	FOnTimelineFloat InterpFunctionLocation{};
	FOnTimelineFloat InterpFunctionRotation{};
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
};
