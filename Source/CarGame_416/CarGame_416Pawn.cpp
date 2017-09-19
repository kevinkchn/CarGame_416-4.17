// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "CarGame_416Pawn.h"
#include "CarGame_416WheelFront.h"
#include "CarGame_416WheelRear.h"
#include "CarGame_416Hud.h"
#include "CarGame_WVMC4W.h"
#include "CarGame_PlayerController.h"
#include "MyGameStateBase.h"
#include "MyPlayerState.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/AudioComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Engine/SkeletalMesh.h"
#include "UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "Classes/Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Classes/Particles/ParticleSystemComponent.h"

// Needed for VR Headset
#if HMD_MODULE_INCLUDED
#include "IHeadMountedDisplay.h"
#endif // HMD_MODULE_INCLUDED

const FName ACarGame_416Pawn::LookUpBinding("LookUp");
const FName ACarGame_416Pawn::LookRightBinding("LookRight");
const FName ACarGame_416Pawn::EngineAudioRPM("RPM");

#define LOCTEXT_NAMESPACE "VehiclePawn"

ACarGame_416Pawn::ACarGame_416Pawn(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCarGame_WVMC4W>(AWheeledVehicle::VehicleMovementComponentName))
{
	
	AutoPossessPlayer = EAutoReceiveInput::Disabled;

	// Car mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(TEXT("/Game/Trueno/Trueno.Trueno"));
	GetMesh()->SetSkeletalMesh(CarMesh.Object);
	
	static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/MuscleCar/musclecar_AnimBlueprint"));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);

	// Setup friction materials
	static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> SlipperyMat(TEXT("/Game/VehicleAdv/PhysicsMaterials/Slippery.Slippery"));
	SlipperyMaterial = SlipperyMat.Object;
		
	static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> NonSlipperyMat(TEXT("/Game/VehicleAdv/PhysicsMaterials/NonSlippery.NonSlippery"));
	NonSlipperyMaterial = NonSlipperyMat.Object;

	UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	check(Vehicle4W->WheelSetups.Num() == 4);

	// Wheels/Tyres
	// Setup the wheels
	Vehicle4W->WheelSetups[0].WheelClass = UCarGame_416WheelFront::StaticClass();
	Vehicle4W->WheelSetups[0].BoneName = FName("WheelFL");
	Vehicle4W->WheelSetups[0].AdditionalOffset = FVector(0.f, -8.f, 0.f);

	Vehicle4W->WheelSetups[1].WheelClass = UCarGame_416WheelFront::StaticClass();
	Vehicle4W->WheelSetups[1].BoneName = FName("WheelFR");
	Vehicle4W->WheelSetups[1].AdditionalOffset = FVector(0.f, 8.f, 0.f);

	Vehicle4W->WheelSetups[2].WheelClass = UCarGame_416WheelRear::StaticClass();
	Vehicle4W->WheelSetups[2].BoneName = FName("WheelBL");
	Vehicle4W->WheelSetups[2].AdditionalOffset = FVector(0.f, -8.f, 0.f);

	Vehicle4W->WheelSetups[3].WheelClass = UCarGame_416WheelRear::StaticClass();
	Vehicle4W->WheelSetups[3].BoneName = FName("WheelBR");
	Vehicle4W->WheelSetups[3].AdditionalOffset = FVector(0.f, 8.f, 0.f);

	// Adjust the tire loading
	Vehicle4W->MinNormalizedTireLoad = 0.0f;
	Vehicle4W->MinNormalizedTireLoadFiltered = 0.2f;
	Vehicle4W->MaxNormalizedTireLoad = 2.0f;
	Vehicle4W->MaxNormalizedTireLoadFiltered = 2.0f;

	// Engine 
	// Torque setup
	Vehicle4W->MaxEngineRPM = 5700.0f;
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->Reset();
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(0.0f, 400.0f);
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(1890.0f, 500.0f);
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(5730.0f, 400.0f);

	StandardEngineSetup = Vehicle4W->EngineSetup;
	BoostEngineSetup = Vehicle4W->EngineSetup;

	
			
 	// Transmission	
	// We want 4wd
	Vehicle4W->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_RearDrive;
	// Drive the front wheels a little more than the rear
	Vehicle4W->DifferentialSetup.FrontRearSplit = 0.65;

	// Automatic gearbox
	Vehicle4W->TransmissionSetup.bUseGearAutoBox = true;
	Vehicle4W->TransmissionSetup.GearSwitchTime = 0.15f;
	Vehicle4W->TransmissionSetup.GearAutoBoxLatency = 1.0f;

	//Vehicle4W->bReverseAsBrake = false;

	// Physics settings
	// Adjust the center of mass - the buggy is quite low
	UPrimitiveComponent* UpdatedPrimitive = Cast<UPrimitiveComponent>(Vehicle4W->UpdatedComponent);
	if (UpdatedPrimitive)
	{
		UpdatedPrimitive->BodyInstance.COMNudge = FVector(0.0f, 0.0f, 0.0f);
	}

	// Set the inertia scale. This controls how the mass of the vehicle is distributed.
	Vehicle4W->InertiaTensorScale = FVector(1.0f, 1.0f, 1.0f);

	// Create a spring arm component for our chase camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 75.0f));
	SpringArm->SetWorldRotation(FRotator(-15.0f, 0.0f, 0.0f));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = false;

	// Create the chase camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ChaseCamera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->SetRelativeLocation(FVector(-125.0, 0.0f, 0.0f));
	Camera->SetRelativeRotation(FRotator(10.0f, 0.0f, 0.0f));
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 90.f;

	// Create In-Car camera component 
	InternalCameraOrigin = FVector(-34.0f, -10.0f, 50.0f);
	InternalCameraBase = CreateDefaultSubobject<USceneComponent>(TEXT("InternalCameraBase"));
	InternalCameraBase->SetRelativeLocation(InternalCameraOrigin);
	InternalCameraBase->SetupAttachment(GetMesh());

	InternalCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("InternalCamera"));
	InternalCamera->bUsePawnControlRotation = false;
	InternalCamera->FieldOfView = 90.f;
	InternalCamera->SetupAttachment(InternalCameraBase);

	// In car HUD
	// Create text render component for in car speed display
	InCarSpeed = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarSpeed"));
	InCarSpeed->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	InCarSpeed->SetRelativeLocation(FVector(35.0f, -6.0f, 20.0f));
	InCarSpeed->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	InCarSpeed->SetupAttachment(GetMesh());

	// Create text render component for in car gear display
	InCarGear = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarGear"));
	InCarGear->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	InCarGear->SetRelativeLocation(FVector(35.0f, 5.0f, 20.0f));
	InCarGear->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	InCarGear->SetupAttachment(GetMesh());
	
	// Setup the audio component and allocate it a sound cue
	//static ConstructorHelpers::FObjectFinder<USoundCue> SoundCue(TEXT("/Game/VehicleAdv/Sound/Engine_Loop_Cue.Engine_Loop_Cue"));
	EngineSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineSound"));
	EngineSoundComponent->SetupAttachment(GetMesh());

	// Colors for the in-car gear display. One for normal one for reverse
	GearDisplayReverseColor = FColor(255, 0, 0, 255);
	GearDisplayColor = FColor(255, 255, 255, 255);

	bIsLowFriction = false;
	bInReverseGear = false;

	/////Setup Correction timeline
	static ConstructorHelpers::FObjectFinder <UCurveFloat> Curve(TEXT("/Game/Curves/CarCorrectionCurve.CarCorrectionCurve"));
	if (Curve.Object)
	{
		FCurve = Curve.Object;
	}
	CorrectionTimelineLocation = ObjectInitializer.CreateDefaultSubobject<UTimelineComponent>(this, TEXT("Location Correction Timeline"));
	CorrectionTimelineRotation = ObjectInitializer.CreateDefaultSubobject<UTimelineComponent>(this, TEXT("Rotation Correction Timeline"));
	InterpFunctionLocation.BindUFunction(this, FName{ TEXT("CorrectionTimelineUpdateFloatLocation") });
	InterpFunctionRotation.BindUFunction(this, FName{ TEXT("CorrectionTimelineUpdateFloatRotation") });

	/////Replication Variables
	bReplicates = true;
	bReplicateMovement = true;
	NetUpdateFrequency = 10;

	//Tag Box
	TagBox = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("Tag Box"));
	TagBox->SetupAttachment(RootComponent);
}

////////////////////////////INPUT SETUP//////////////////////////////////
void ACarGame_416Pawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACarGame_416Pawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACarGame_416Pawn::MoveRight);
	PlayerInputComponent->BindAxis(LookUpBinding);
	PlayerInputComponent->BindAxis(LookRightBinding);

	PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &ACarGame_416Pawn::OnHandbrakePressed);
	PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &ACarGame_416Pawn::OnHandbrakeReleased);
	PlayerInputComponent->BindAction("SwitchCamera", IE_Pressed, this, &ACarGame_416Pawn::OnToggleCamera);

	PlayerInputComponent->BindAction("Boost", IE_Pressed, this, &ACarGame_416Pawn::OnBoostPressed);
	PlayerInputComponent->BindAction("Boost", IE_Released, this, &ACarGame_416Pawn::OnBoostReleased);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ACarGame_416Pawn::OnResetVR); 
}
void ACarGame_416Pawn::MoveForward(float Val)
{
	
	if (bCanRace)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Enabled")));
		if (Val == 0)
		{
			SteeringInput -= .01;
			FMath::Clamp(SteeringInput, 0, 1);
			GetVehicleMovementComponent()->SetThrottleInput(Val);
		}
		else if (Val == 1)
		{
			SteeringInput += .01;
			FMath::Clamp(SteeringInput, 0, 1);
			GetVehicleMovementComponent()->SetThrottleInput(Val);
		}
		
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("RaceDisabled")));
	}
	
		//Server_ClientMoveForward(Val);
}

void ACarGame_416Pawn::MoveRight(float Val)
{
	if(bCanRace)
	GetVehicleMovementComponent()->SetSteeringInput(Val);
}

void ACarGame_416Pawn::OnHandbrakePressed()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void ACarGame_416Pawn::OnHandbrakeReleased()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void ACarGame_416Pawn::OnToggleCamera()
{
	EnableIncarView(!bInCarCameraActive);
}
void ACarGame_416Pawn::OnBoostPressed()
{
	SwitchToBoostEngine();
}
void ACarGame_416Pawn::OnBoostReleased()
{
	SwitchToStandardEngine();
}



//////////////////////////////VEHICLE SETUP/////////////////////////////////////
void ACarGame_416Pawn::SetupVehicleParams(FVector CenterOfMass, USoundCue* SoundCue, UClass* FrontWheelClass, UClass* RearWheelClass, float MinNormalizedTireLoad, float MinNormalizedTireLoadFiltered, float MaxNormalizedTireLoad, float MaxNormalizedTireLoadFiltered, TArray<FVector2D> SteeringCurve, DiffType DiffTypeEnum, FVector InertiaTensorScale)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Testsss: %d"), CenterOfMass.X));
	UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());
	// Physics settings
	// Adjust the center of mass
	/*UPrimitiveComponent* UpdatedPrimitive = Cast<UPrimitiveComponent>(Vehicle4W->UpdatedComponent);
	if (UpdatedPrimitive)
	{
	UpdatedPrimitive->BodyInstance.COMNudge = CenterOfMass;
	}*/
	GetMesh()->SetCenterOfMass(CenterOfMass);
	// Start an engine sound playing
	if (SoundCue)
	{
		EngineSoundComponent->SetSound(SoundCue);
		EngineSoundComponent->Play();
	}

	// Adjust the steering 
	Vehicle4W->SteeringCurve.GetRichCurve()->Reset();
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(SteeringCurve[0].X, SteeringCurve[0].Y);
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(SteeringCurve[1].X, SteeringCurve[1].Y);
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(SteeringCurve[2].X, SteeringCurve[2].Y);

	switch (DiffTypeEnum)
	{
	case DiffType::LimitedSlip_4W:
		Vehicle4W->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_4W;
		break;
	case DiffType::LimitedSlip_FrontDrive:
		Vehicle4W->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_FrontDrive;
		break;
	case DiffType::LimitedSlip_RearDrive:
		Vehicle4W->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_RearDrive;
		break;
	case DiffType::Open_4W:
		Vehicle4W->DifferentialSetup.DifferentialType = EVehicleDifferential4W::Open_4W;
		break;
	case DiffType::Open_FrontDrive:
		Vehicle4W->DifferentialSetup.DifferentialType = EVehicleDifferential4W::Open_FrontDrive;
		break;
	case DiffType::Open_RearDrive:
		Vehicle4W->DifferentialSetup.DifferentialType = EVehicleDifferential4W::Open_RearDrive;
		break;
	default:
		break;
	}

	Vehicle4W->InertiaTensorScale = InertiaTensorScale;
}

void ACarGame_416Pawn::SwitchToBoostEngine()
{
	
	UCarGame_WVMC4W* Vehicle4W = CastChecked<UCarGame_WVMC4W>(GetVehicleMovement());

	check(Vehicle4W->WheelSetups.Num() == 4);

	Vehicle4W->UpdateEngine(BoostEngineSetup);
	//Vehicle4W->MaxEngineRPM = BoostEngineSetup.MaxRPM;


}
void ACarGame_416Pawn::SwitchToStandardEngine()
{
	 UCarGame_WVMC4W* Vehicle4W = CastChecked<UCarGame_WVMC4W>(GetVehicleMovement());

	check(Vehicle4W->WheelSetups.Num() == 4);
	
	Vehicle4W->UpdateEngine(StandardEngineSetup);
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("StandardEngine RPMS: %f"), StandardEngineSetup.MaxRPM));
	//Vehicle4W->MaxEngineRPM = StandardEngineSetup.MaxRPM;

	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Engine RPM: %f"), Vehicle4W->EngineSetup.MaxRPM));
	
}
void ACarGame_416Pawn::SetupStandardEngine(float MaxRPM, FVector2D TorqueKey1, FVector2D TorqueKey2, FVector2D TorqueKey3)
{
	// Engine 
	// Torque setup
	StandardEngineSetup.MaxRPM = MaxRPM;
	StandardEngineSetup.TorqueCurve.GetRichCurve()->Reset();
	StandardEngineSetup.TorqueCurve.GetRichCurve()->AddKey(TorqueKey1.X, TorqueKey1.Y);
	StandardEngineSetup.TorqueCurve.GetRichCurve()->AddKey(TorqueKey2.X, TorqueKey2.Y);
	StandardEngineSetup.TorqueCurve.GetRichCurve()->AddKey(TorqueKey3.X, TorqueKey3.Y);
	
	

}
void ACarGame_416Pawn::SetupBoostEngine(float MaxRPM, FVector2D TorqueKey1, FVector2D TorqueKey2, FVector2D TorqueKey3)
{
	// Engine 
	// Torque setup
	BoostEngineSetup.MaxRPM = MaxRPM;
	BoostEngineSetup.TorqueCurve.GetRichCurve()->Reset();
	BoostEngineSetup.TorqueCurve.GetRichCurve()->AddKey(TorqueKey1.X, TorqueKey1.Y);
	BoostEngineSetup.TorqueCurve.GetRichCurve()->AddKey(TorqueKey2.X, TorqueKey2.Y);
	BoostEngineSetup.TorqueCurve.GetRichCurve()->AddKey(TorqueKey3.X, TorqueKey3.Y);
}

void ACarGame_416Pawn::Test(int32 f)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Test: %f"), f));
}

void ACarGame_416Pawn::SetControllerReference(APlayerController* Controller)
{
	PlayerController = Controller;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("SetController")));
}

void ACarGame_416Pawn::EnableRace()
{
	bCanRace = true;
}
void ACarGame_416Pawn::DisableRace()
{
	bCanRace = false;
}

/***********************************************************/
/************************Replication************************/
/***********************************************************/
/*
void ACarGame_416Pawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	//DOREPLIFETIME_CONDITION(ACarGame_416Pawn, Location, COND_SkipOwner);
	//DOREPLIFETIME_CONDITION(ACarGame_416Pawn, Rotation, COND_SkipOwner);
	DOREPLIFETIME(ACarGame_416Pawn, Local);
	//DOREPLIFETIME(ACarGame_416Pawn, Rotation);
}

void ACarGame_416Pawn::OnRep_Local()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnRepLocal")));
}
/*
void ACarGame_416Pawn::OnRep_Rotation()
{
	NextRotation = Rotation;
	CorrectRotation();
}
*
void ACarGame_416Pawn::Server_ClientMoveForward_Implementation(float Val)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("ClientMoveForward")));
	GetVehicleMovementComponent()->SetThrottleInput(Val);
}
bool ACarGame_416Pawn::Server_ClientMoveForward_Validate(float Val)
{
	return true;
}
void ACarGame_416Pawn::UpdateClientState(FVector Loc, FRotator Rot, FVector AngVel, FVector LinVel)
{
	//NextLocation = Loc;
	//CorrectedLocation = Loc;
	//NextRotation = Rot;
	NextLocation = Loc;
	CurLocation = GetActorLocation();
	CurRotation = GetActorRotation();
	NextRotation = Rot;
	float LocationDelta = (NextLocation - CurLocation).Size();
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("CurLoc %f, %f, %f NextLoc %f, %f, %f"), CurLocation.X, CurLocation.Y, CurLocation.Z, NextLocation.X, NextLocation.Y, NextLocation.Z));
	if (LocationDelta > 200.0f)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Snap Correction %f"), LocationDelta));
		
		CorrectedLocation = NextLocation;
		//CorrectLocation();
		//GetMesh()->SetSimulatePhysics(false);
		//SetActorLocation(NextLocation, false, nullptr, ETeleportType::TeleportPhysics);
		//GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetAllPhysicsPosition(NextLocation);
	}
	else if (LocationDelta > 10.0f)
	{
		
		FVector DirectionVector = NextLocation - CurLocation;
		DirectionVector.Normalize();
		CorrectedLocation = (DirectionVector*(LocationDelta / 5.0f)) + CurLocation;
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Calc Loc: %f, %f, %f NextLoc: %f, %f, %f"), CorrectedLocation.X, CorrectedLocation.Y, CorrectedLocation.Z, NextLocation.X, NextLocation.Y, NextLocation.Z));
		//CorrectLocation();
		GetMesh()->SetAllPhysicsPosition(CorrectedLocation);
	}
	float RotationDelta = (Rot.Vector() - GetActorRotation().Vector()).Size();
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("RotDelta: %f"), RotationDelta));
	//CorrectRotation();
	GetMesh()->SetAllPhysicsRotation(NextRotation);
	GetMesh()->SetPhysicsAngularVelocity(AngVel, false, NAME_None);
	GetMesh()->SetPhysicsLinearVelocity(LinVel, false, NAME_None);
}
void ACarGame_416Pawn::CorrectLocation()
{
	CorrectionTimelineLocation->PlayFromStart();
}
void ACarGame_416Pawn::CorrectRotation()
{
	CorrectionTimelineRotation->PlayFromStart();
}
void ACarGame_416Pawn::CorrectionTimelineUpdateFloatLocation(float Val)
{
	//FVector Interp = FMath::VInterpTo(GetActorLocation(), CorrectedLocation, FDelta, 1.0f);
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("Timeline Val:%f"), Val));
	GetMesh()->SetAllPhysicsPosition(FMath::Lerp(CurLocation, CorrectedLocation, Val));
}
void ACarGame_416Pawn::CorrectionTimelineUpdateFloatRotation(float Val)
{
	GetMesh()->SetAllPhysicsRotation(FMath::Lerp(CurRotation, NextRotation,Val));
}
*/
void ACarGame_416Pawn::NetMulticast_UpdateClientState_Implementation(FVector Loc, FRotator Rot, FVector AngVel, FVector LinVel)
{
	
	if (Role != ROLE_Authority && !IsLocallyControlled())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("NextLoc %f, %f, %f"), NextLocation.X, NextLocation.Y, NextLocation.Z));
		//UpdateClientState(Loc, Rot, AngVel, LinVel);
	}
}
bool ACarGame_416Pawn::NetMulticast_UpdateClientState_Validate(FVector Loc, FRotator Rot, FVector AngVel, FVector LinVel)
{
	return true;
}
void ACarGame_416Pawn::Server_UpdateClientState_Implementation(FVector Loc, FRotator Rot, FVector AngVel, FVector LinVel)
{
	if (Role == ROLE_Authority)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("ServerUpdate")));
	}
}
bool ACarGame_416Pawn::Server_UpdateClientState_Validate(FVector Loc, FRotator Rot, FVector AngVel, FVector LinVel)
{
	
	return true;
}



void ACarGame_416Pawn::SendClientStateUpdate()
{	
	//if (Role == ROLE_Authority)
	/*
	if(GetWorld())
	{
		if (GetWorld()->IsServer())
		{
			NetMulticast_UpdateClientState(FVector_NetQuantize100(GetActorLocation()), GetActorRotation(), FVector_NetQuantize100(GetMesh()->GetPhysicsAngularVelocity()), FVector_NetQuantize100(GetMesh()->GetPhysicsLinearVelocity()));
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("ServerUpdate")));
		}
	}
	else
	{
		Server_UpdateClientState(GetActorLocation(), GetActorRotation(), GetMesh()->GetPhysicsAngularVelocity(), GetMesh()->GetPhysicsLinearVelocity());
	}
	/***************************Replicate Properties********************/
	if (GetWorld())
	{
		if (GetWorld()->IsServer())
		{
			//Local = 1;
			NetMulticast_UpdateClientState(FVector_NetQuantize100(GetActorLocation()), GetActorRotation(), FVector_NetQuantize100(GetMesh()->GetPhysicsAngularVelocity()), FVector_NetQuantize100(GetMesh()->GetPhysicsLinearVelocity()));
		}
	}
}

void ACarGame_416Pawn::Tag(FVector AngVel, FVector LinVel, bool GotTagged)
{
	AMyGameStateBase* MGSB = (AMyGameStateBase*)(GetWorld()->GetGameState());
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Tag")));
	if (GetController())
	{
		if (GotTagged)
		{
			((ACarGame_PlayerController*)GetController())->TagSwitch(MGSB->ItClass, GetTransform(), this, GotTagged, AngVel, LinVel);
		}
		else
		{
			((ACarGame_PlayerController*)GetController())->TagSwitch(MGSB->NotItClass, GetTransform(), this, GotTagged, AngVel, LinVel);
		}
	}
	

}


/***************************************************************/


void ACarGame_416Pawn::EnableIncarView(const bool bState)
{
	if (bState != bInCarCameraActive)
	{
		bInCarCameraActive = bState;
		
		if (bState == true)
		{
			OnResetVR();
			Camera->Deactivate();
			InternalCamera->Activate();
		}
		else
		{
			InternalCamera->Deactivate();
			Camera->Activate();
		}
		
		InCarSpeed->SetVisibility(bInCarCameraActive);
		InCarGear->SetVisibility(bInCarCameraActive);
	}
}



/////////////////////////TICK///////////////////////////
void ACarGame_416Pawn::Tick(float Delta)
{
	Super::Tick(Delta);
	DeltaTotal += Delta;
	FDelta = Delta;

	// Setup the flag to say we are in reverse gear
	bInReverseGear = GetVehicleMovement()->GetCurrentGear() < 0;
	
	// Update phsyics material
	UpdatePhysicsMaterial();

	// Update the strings used in the hud (incar and onscreen)
	UpdateHUDStrings();

	// Set the string in the incar hud
	SetupInCarHUD();

	bool bHMDActive = false;
#if HMD_MODULE_INCLUDED
	if ((GEngine->HMDDevice.IsValid() == true ) && ( (GEngine->HMDDevice->IsHeadTrackingAllowed() == true) || (GEngine->IsStereoscopic3D() == true)))
	{
		bHMDActive = true;
	}
#endif // HMD_MODULE_INCLUDED
	if( bHMDActive == false )
	{
		if ( (InputComponent) && (bInCarCameraActive == true ))
		{
			FRotator HeadRotation = InternalCamera->RelativeRotation;
			HeadRotation.Pitch += InputComponent->GetAxisValue(LookUpBinding);
			HeadRotation.Yaw += InputComponent->GetAxisValue(LookRightBinding);
			InternalCamera->RelativeRotation = HeadRotation;
		}
	}	

	// Pass the engine RPM to the sound component
	float RPMToAudioScale = 2500.0f / GetVehicleMovement()->GetEngineMaxRotationSpeed();
	EngineSoundComponent->SetFloatParameter(EngineAudioRPM, GetVehicleMovement()->GetEngineRotationSpeed());

	UpdateWheelEffects(Delta);
	
}

void ACarGame_416Pawn::TagTimerEvent()
{
	///////// TAG MODE
	if (DeltaTotal > 5 && bIsIt)
	{
		TagBox->GetOverlappingActors(OverlappingActors, ACarGame_416Pawn::StaticClass());
		for (AActor* It : OverlappingActors)
		{
			if (It != this)
			{
				ACarGame_416Pawn* CGP = (ACarGame_416Pawn*)It;
				if (!((AMyPlayerState*)((ACarGame_PlayerController*)CGP->GetController())->PlayerState)->bIsFinished)
				{
					CGP->Tag(CGP->GetMesh()->GetPhysicsAngularVelocity(), CGP->GetMesh()->GetPhysicsLinearVelocity(), true);
					Tag(GetMesh()->GetPhysicsAngularVelocity(), GetMesh()->GetPhysicsLinearVelocity(), false);
					DeltaTotal = 0;
					break;
				}
				//((ACarGame_PlayerController*)GetController())->UnPossess();
				//((ACarGame_PlayerController*)((ACarGame_416Pawn*)It)->GetController());
				//GetMesh()->GetPhysicsAngularVelocity(), GetMesh()->GetPhysicsLinearVelocity()

			}
		}
	}
}

void ACarGame_416Pawn::BeginPlay()
{
	Super::BeginPlay();

	bool bWantInCar = false;
	// First disable both speed/gear displays 
	bInCarCameraActive = false;
	InCarSpeed->SetVisibility(bInCarCameraActive);
	InCarGear->SetVisibility(bInCarCameraActive);

	// Enable in car view if HMD is attached
#if HMD_MODULE_INCLUDED
	//bWantInCar = UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled();
#endif // HMD_MODULE_INCLUDED

	EnableIncarView(bWantInCar);
	
	//add fcurve to timeline and connect to interp function
	/*
	CorrectionTimelineLocation->AddInterpFloat(FCurve, InterpFunctionLocation, FName{ TEXT("Float") });
	CorrectionTimelineLocation->SetTimelineLength(.1f);
	CorrectionTimelineRotation->AddInterpFloat(FCurve, InterpFunctionRotation, FName{ TEXT("Float") });
	CorrectionTimelineRotation->SetTimelineLength(.1f);
	*/

	///Start Tag Timer
	if (GetWorld())
	{
		if (GetWorld()->IsServer() && bIsIt)
		{
			GetWorld()->GetTimerManager().SetTimer(MyTimerHandle, this, &ACarGame_416Pawn::TagTimerEvent, .05f, true);
		}
	}

	GetMesh()->SetSimulatePhysics(true);

	if (GetController())
	{
		bCanRace = ((ACarGame_PlayerController*)GetController())->bCanRace;
		
	}
}

void ACarGame_416Pawn::OnResetVR()
{
#if HMD_MODULE_INCLUDED
	if (GEngine->HMDDevice.IsValid())
	{
		GEngine->HMDDevice->ResetOrientationAndPosition();
		InternalCamera->SetRelativeLocation(InternalCameraOrigin);
		GetController()->SetControlRotation(FRotator());
	}
#endif // HMD_MODULE_INCLUDED
}

void ACarGame_416Pawn::UpdateHUDStrings()
{
	float KPH = FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) * 0.036f;
	int32 KPH_int = FMath::FloorToInt(KPH);
	int32 Gear = GetVehicleMovement()->GetCurrentGear();

	// Using FText because this is display text that should be localizable
	SpeedDisplayString = FText::Format(LOCTEXT("SpeedFormat", "{0} km/h"), FText::AsNumber(KPH_int));


	if (bInReverseGear == true)
	{
		GearDisplayString = FText(LOCTEXT("ReverseGear", "R"));
	}
	else
	{
		GearDisplayString = (Gear == 0) ? LOCTEXT("N", "N") : FText::AsNumber(Gear);
	}

}

void ACarGame_416Pawn::SetupInCarHUD()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if ((PlayerController != nullptr) && (InCarSpeed != nullptr) && (InCarGear != nullptr))
	{
		// Setup the text render component strings
		InCarSpeed->SetText(SpeedDisplayString);
		InCarGear->SetText(GearDisplayString);
		
		if (bInReverseGear == false)
		{
			InCarGear->SetTextRenderColor(GearDisplayColor);
		}
		else
		{
			InCarGear->SetTextRenderColor(GearDisplayReverseColor);
		}
	}
}

void ACarGame_416Pawn::UpdatePhysicsMaterial()
{
	if (GetActorUpVector().Z < 0)
	{
		GetMesh()->SetPhysMaterialOverride(SlipperyMaterial);
		bIsLowFriction = true;
	}
	else
	{
		GetMesh()->SetPhysMaterialOverride(NonSlipperyMaterial);
		bIsLowFriction = false;
	}
}

UParticleSystem* ACarGame_416Pawn::GetDustFX(UPhysicalMaterial* PhysMaterial, float CurrentSpeed)
{
	EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(PhysMaterial);

	// determine particle FX
	UParticleSystem* WheelFX = GetWheelFX(SurfaceType);
	if (WheelFX)
	{
		const float MinSpeed = GetMinSpeed(SurfaceType);
		if (CurrentSpeed < MinSpeed)
		{
			WheelFX = NULL;
		}
	}

	return WheelFX;
}

UParticleSystem* ACarGame_416Pawn::GetWheelFX(TEnumAsByte<EPhysicalSurface> MaterialType)
{
	UParticleSystem* WheelFX = NULL;

	switch (MaterialType)
	{
	case EPhysicalSurface::SurfaceType2:		WheelFX = AsphaltFX; break;
	case EPhysicalSurface::SurfaceType3:			WheelFX = DirtFX; break;
	case EPhysicalSurface::SurfaceType1:			WheelFX = GrassFX; break;
	default:							WheelFX = NULL; break;
	}

	return WheelFX;
}

float ACarGame_416Pawn::GetMinSpeed(TEnumAsByte<EPhysicalSurface> MaterialType)
{
	float MinSpeed = 0.0f;

	switch (MaterialType)
	{
	case EPhysicalSurface::SurfaceType1:		MinSpeed = GrassMinSpeed; break;
	case EPhysicalSurface::SurfaceType2:		MinSpeed = AsphaltMinSpeed; break;
	case EPhysicalSurface::SurfaceType3:		MinSpeed = DirtMinSpeed; break;
	default:							MinSpeed = 0.0f; break;
	}

	return MinSpeed;
}

void ACarGame_416Pawn::SpawnNewWheelEffect(int WheelIndex)
{
	DustPSC[WheelIndex] = NewObject<UParticleSystemComponent>(this);
	DustPSC[WheelIndex]->bAutoActivate = true;
	DustPSC[WheelIndex]->bAutoDestroy = false;
	DustPSC[WheelIndex]->RegisterComponentWithWorld(GetWorld());
	DustPSC[WheelIndex]->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, GetVehicleMovement()->WheelSetups[WheelIndex].BoneName);
}

void ACarGame_416Pawn::UpdateWheelEffects(float DeltaTime)
{

	bTiresTouchingGround = false;

	if (GetVehicleMovement() && GetVehicleMovement()->Wheels.Num() > 0)
	{
		const float CurrentSpeed = FMath::Abs(GetVehicleMovement()->GetForwardSpeed());
		for (int32 i = 0; i < ARRAY_COUNT(DustPSC); i++)
		{
			UPhysicalMaterial* ContactMat = GetVehicleMovement()->Wheels[i]->GetContactSurfaceMaterial();
			if (ContactMat != NULL)
			{
				bTiresTouchingGround = true;
			}
			UParticleSystem* WheelFX = GetDustFX(ContactMat, CurrentSpeed);

			const bool bIsActive = DustPSC[i] != NULL && !DustPSC[i]->bWasDeactivated && !DustPSC[i]->bWasCompleted;
			UParticleSystem* CurrentFX = DustPSC[i] != NULL ? DustPSC[i]->Template : NULL;
			if (WheelFX != NULL && (CurrentFX != WheelFX || !bIsActive))
			{
				if (DustPSC[i] == NULL || !DustPSC[i]->bWasDeactivated)
				{
					if (DustPSC[i] != NULL)
					{
						DustPSC[i]->SetActive(false);
						DustPSC[i]->bAutoDestroy = true;
					}
					SpawnNewWheelEffect(i);
				}
				DustPSC[i]->SetTemplate(WheelFX);
				DustPSC[i]->ActivateSystem();
			}
			else if (WheelFX == NULL && bIsActive)
			{
				DustPSC[i]->SetActive(false);
			}
		}
	}
}


#undef LOCTEXT_NAMESPACE
