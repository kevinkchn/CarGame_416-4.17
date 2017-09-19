// Fill out your copyright notice in the Description page of Project Settings.

#include "Checkpoint.h"
#include "CarGame_416Pawn.h"
#include "MyGameStateBase.h"



// Sets default values
ACheckpoint::ACheckpoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CheckpointBox = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("Checkpoint Box Collision"));
	CheckpointBox->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld())
	{
		if (GetWorld()->IsServer())
		{
			GetWorld()->GetTimerManager().SetTimer(MyTimerHandle, this, &ACheckpoint::TagTimerEvent, .05f, true);
			MGS = GetWorld() != NULL ? GetWorld()->GetGameState<AMyGameStateBase>() : NULL;

		}
	}
	
}

// Called every frame
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACheckpoint::TagTimerEvent()
{
	CheckpointBox->GetOverlappingActors(OverlappingActors, ACarGame_416Pawn::StaticClass());
	for (AActor* It : OverlappingActors)
	{
		MGS->PassCheckpoint(((APawn*)It)->GetController(), CheckpointNumber);
	}
}

