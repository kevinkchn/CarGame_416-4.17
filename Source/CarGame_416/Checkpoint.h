// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Classes/Components/BoxComponent.h"
#include "Checkpoint.generated.h"

class AMyGameStateBase;

UCLASS()
class CARGAME_416_API ACheckpoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckpoint(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite, Category = Settings, EditAnywhere)
		int CheckpointNumber = 0;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UBoxComponent* CheckpointBox;

	UPROPERTY()
		FTimerHandle MyTimerHandle;
	UPROPERTY()
		TArray<AActor*> OverlappingActors;
	UPROPERTY()
		AMyGameStateBase* MGS;
	UFUNCTION()
		void TagTimerEvent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
