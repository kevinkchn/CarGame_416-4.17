// Fill out your copyright notice in the Description page of Project Settings.

#include "Obstacle.h"
#include "Components/StaticMeshComponent.h"
#include "UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Math/UnrealMathUtility.h"
#include "UObject/ConstructorHelpers.h"

AObstacle::AObstacle(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bReplicates = true;
	NetUpdateFrequency = 10;
	//bStaticMeshReplicateMovement = true;
	
	
	static ConstructorHelpers::FObjectFinder <UCurveFloat> Curve(TEXT("/Game/Curves/ObjectCorrectionCurve.ObjectCorrectionCurve"));
	if (Curve.Object)
	{
		FCurve = Curve.Object;
	}

	CorrectionTimelineLocation = ObjectInitializer.CreateDefaultSubobject<UTimelineComponent>(this, TEXT("Location Correction Timeline"));
	CorrectionTimelineRotation = ObjectInitializer.CreateDefaultSubobject<UTimelineComponent>(this, TEXT("Rotation Correction Timeline"));
	InterpFunctionLocation.BindUFunction(this, FName{ TEXT("CorrectionTimelineUpdateFloatLocation") });
	InterpFunctionRotation.BindUFunction(this, FName{ TEXT("CorrectionTimelineUpdateFloatRotation") });
}

void AObstacle::BeginPlay()
{
	Super::BeginPlay();

	CorrectionTimelineLocation->AddInterpFloat(FCurve, InterpFunctionLocation, FName(TEXT("Float")));
	CorrectionTimelineLocation->SetTimelineLength(.1f);
	CorrectionTimelineRotation->AddInterpFloat(FCurve, InterpFunctionRotation, FName(TEXT("Float")));
	CorrectionTimelineRotation->SetTimelineLength(.1f);

	if (GetWorld())
	{
		if (GetWorld()->IsServer())
		{
			GetWorld()->GetTimerManager().SetTimer(ClientStateTimerHandle, this, &AObstacle::SendClientStateUpdate, .05f, true);
		}
	}

	GetStaticMeshComponent()->SetSimulatePhysics(true);
	GetStaticMeshComponent()->BodyInstance.bUseCCD = true;
	//GetStaticMeshComponent()->SetAngularDamping(2);
	//GetStaticMeshComponent()->SetLinearDamping(3);

}

// Called every frame
void AObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FDelta = DeltaTime;
	FDeltaTotal += DeltaTime;
	if (GetWorld())
	{
		if (GetWorld()->IsServer() && FDeltaTotal > 1 && GetStaticMeshComponent()->GetPhysicsAngularVelocity().Size() < 10 && GetStaticMeshComponent()->GetPhysicsLinearVelocity().Size() < 10)
		{
			GetStaticMeshComponent()->SetSimulatePhysics(false);
			NetMulticast_UpdatePhysics(false);
		}
	
	}
	
}
void AObstacle::NetMulticast_UpdatePhysics_Implementation(bool PhysState)
{
	if (GetWorld())
	{
		if (!GetWorld()->IsServer())
		{
			GetStaticMeshComponent()->SetSimulatePhysics(false);
		}
	}
}
bool AObstacle::NetMulticast_UpdatePhysics_Validate(bool PhysState)
{
	return true;
}

void AObstacle::NetMulticast_UpdateClientState_Implementation(FVector Loc, FRotator Rot, FVector AngVel, FVector LinVel)
{
	if(!GetWorld())
	{
		return;
	}
	if (!GetWorld()->IsServer())
	{
		NextLocation = Loc;
		CorrectedLocation = Loc;
		NextRotation = Rot;
		NextAngVel = AngVel;
		NextLinVel = LinVel;
		FVector CurLocation = GetActorLocation();
		CurLoc = GetActorLocation();
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("AngVel %f, %f, %f LinVel %f, %f, %f"), AngVel.X, AngVel.Y, AngVel.Z, LinVel.X, LinVel.Y, LinVel.Z));
		float LocationDelta = (CurLocation - NextLocation).Size();

		

		if (LocationDelta >200.0f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Snap Correction >10 %f, %f, %f - %f, %f, %f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z, NextLocation.X, NextLocation.Y, NextLocation.Z));
			CorrectedLocation = NextLocation;
			CorrectLocation();
		}
		else if(LocationDelta>10.0f)
		{
			
			FVector DirectionVector = NextLocation - CurLocation;
			DirectionVector.Normalize();
			CorrectedLocation = (DirectionVector*(LocationDelta / 5.0f)) + CurLocation;
			//CorrectedLocation = NextLocation;
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Exp Correction %f, %f, %f -> %f, %f, %f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z, CorrectedLocation.X, CorrectedLocation.Y, CorrectedLocation.Z));
			//GetStaticMeshComponent()->SetAllPhysicsPosition(CorrectedLocation);
			CorrectLocation();
		}
		CorrectRotation();
		GetStaticMeshComponent()->SetPhysicsAngularVelocity(NextAngVel, false, NAME_None);
		GetStaticMeshComponent()->SetPhysicsLinearVelocity(NextLinVel, false, NAME_None);
		
	}
}
bool AObstacle::NetMulticast_UpdateClientState_Validate(FVector Loc, FRotator Rot, FVector AngVel, FVector LinVel)
{
	return true;
}
void AObstacle::CorrectLocation()
{
	CorrectionTimelineLocation->PlayFromStart();
}
void AObstacle::CorrectRotation()
{
	CorrectionTimelineRotation->PlayFromStart();
}
void AObstacle::CorrectionTimelineUpdateFloatLocation(float Val)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Lerp %f, %f, %f -> %f, %f, %f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z, CorrectedLocation.X, CorrectedLocation.Y, CorrectedLocation.Z));
	GetStaticMeshComponent()->SetAllPhysicsPosition(FMath::VInterpTo(GetActorLocation(), CorrectedLocation, FDelta, 5.0f));	
}
void AObstacle::CorrectionTimelineUpdateFloatRotation(float Val)
{
	GetStaticMeshComponent()->SetAllPhysicsRotation(FMath::RInterpTo(GetActorRotation(), NextRotation, FDelta, 5.0f));
}
void AObstacle::SendClientStateUpdate()
{
	if (!GetWorld())
	{
		return;
	}
	if (GetWorld()->IsServer())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("ServerLoc %f"), GetActorLocation().X));
		NetMulticast_UpdateClientState(FVector_NetQuantize100(GetActorLocation()), GetActorRotation(), FVector_NetQuantize100(GetStaticMeshComponent()->GetPhysicsAngularVelocity()), FVector_NetQuantize100(GetStaticMeshComponent()->GetPhysicsLinearVelocity()));
	}
}
