// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "FallingObstacle.generated.h"

UCLASS()
class CARGAME_416_API AFallingObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFallingObstacle(const  FObjectInitializer& ObjectInitializer);

	UPROPERTY(Category = Default, BlueprintReadWrite, VisibleDefaultsOnly)
		class UStaticMeshComponent* ObstacleMesh;

	UPROPERTY(ReplicatedUsing = OnRep_Location)
		FVector Location;
	UPROPERTY(ReplicatedUsing = OnRep_Rotation)
		FRotator Rotation;
	UFUNCTION(NetMultiCast, Reliable, WithValidation)
		void NetMultiCast_UpdateLocation(FVector curLoc);

private:
	UFUNCTION()
		void OnRep_Location();
	UFUNCTION()
		void OnRep_Rotation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:	
	


	
};
