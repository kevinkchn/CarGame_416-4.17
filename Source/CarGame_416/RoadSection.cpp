// Fill out your copyright notice in the Description page of Project Settings.

#include "RoadSection.h"


// Sets default values
ARoadSection::ARoadSection(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARoadSection::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARoadSection::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARoadSection::ActivatePhysics()
{
	if (RoadMesh)
	{
		RoadMesh->SetSimulatePhysics(true);
	}
}

void ARoadSection::SetDestructibleComponentMesh(UDestructibleMesh* DMesh)
{

}
