// Fill out your copyright notice in the Description page of Project Settings.

#include "FallingObstacle.h"
#include "UnrealNetwork.h"


// Sets default values
AFallingObstacle::AFallingObstacle(const FObjectInitializer& ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Replication
	bReplicates = true;
	bAlwaysRelevant = true;

	ObstacleMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("ObstacleMesh"));
	RootComponent = ObstacleMesh;
}

// Called when the game starts or when spawned
void AFallingObstacle::BeginPlay()
{
	Super::BeginPlay();

	ObstacleMesh->SetSimulatePhysics(true);
}

// Called every frame
void AFallingObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Location = GetActorLocation();
	Rotation = GetActorRotation();
	/*
	if(HasAuthority())
	{
		NetMultiCast_UpdateLocation(Location);
	}*/
}

void AFallingObstacle::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	DOREPLIFETIME(AFallingObstacle, Location);
}

///RPC's
void AFallingObstacle::NetMultiCast_UpdateLocation_Implementation(FVector CurLoc)
{
	SetActorLocation(CurLoc);
}

bool AFallingObstacle::NetMultiCast_UpdateLocation_Validate(FVector CurLoc)
{
	return true;
}
void AFallingObstacle::OnRep_Location()
{
	SetActorLocation(Location);
}
void AFallingObstacle::OnRep_Rotation()
{
	SetActorRotation(Rotation, ETeleportType::None);
}
