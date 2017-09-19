// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DestructibleComponent.h"
#include "RoadSection.generated.h"

UCLASS()
class CARGAME_416_API ARoadSection : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoadSection(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Category = Default, BlueprintReadWrite)
		class UStaticMeshComponent* RoadMesh;
	UPROPERTY(Category = Default, BlueprintReadWrite)
		UDestructibleComponent* DestructibleRoadComponent;

	UFUNCTION(Category = Road, BluePrintCallable)
		void ActivatePhysics();
	UFUNCTION(Category = Destructible, BluePrintCallable)
		void SetDestructibleComponentMesh(UDestructibleMesh* DMesh);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
