// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Sound/SoundCue.h"
#include "GameFramework/Controller.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "Classes/Components/BoxComponent.h"
#include "CarGame_416Pawn.generated.h"

class UPhysicalMaterial;
class UCameraComponent;
class USpringArmComponent;
class UTextRenderComponent;
class UInputComponent;
class UAudioComponent;
class UParticleSystemComponent;

UENUM(BlueprintType)
enum class DiffType : uint8
{
	LimitedSlip_4W UMETA(DisplayName = "LimitedSlip_4W"),
	LimitedSlip_FrontDrive UMETA(DisplayName = "LimitedSlip_FrontDrive"),
	LimitedSlip_RearDrive UMETA(DisplayName = "LimitedSlip_RearDrive"),
	Open_4W UMETA(DisplayName = "Open_4W"),
	Open_FrontDrive UMETA(DisplayName = "Open_FrontDrive"),
	Open_RearDrive UMETA(DisplayName = "Open_RearDrive"),
};
UCLASS(config=Game)
class ACarGame_416Pawn : public AWheeledVehicle
{
	GENERATED_BODY()

	/** Spring arm that will offset the camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	/** Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	/** SCene component for the In-Car view origin */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* InternalCameraBase;

	/** Camera component for the In-Car view */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* InternalCamera;

	/** Text component for the In-Car speed */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* InCarSpeed;

	/** Text component for the In-Car gear */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* InCarGear;

	/** Audio component for the engine sound */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAudioComponent* EngineSoundComponent;

public:
	ACarGame_416Pawn(const FObjectInitializer& ObjectInitializer);

	/** The current speed as a string eg 10 km/h */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	FText SpeedDisplayString;

	/** The current gear as a string (R,N, 1,2 etc) */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	FText GearDisplayString;

	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	/** The color of the incar gear text in forward gears */
	FColor	GearDisplayColor;

	/** The color of the incar gear text when in reverse */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	FColor	GearDisplayReverseColor;

	/** Are we using incar camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly)
	bool bInCarCameraActive;

	/** Are we in reverse gear */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly)
	bool bInReverseGear;
	
	//UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadWrite)
		//USoundCue* SoundCue;

	

	void OnBoostPressed();
	void OnBoostReleased();
	/*Vehicle4W->MinNormalizedTireLoad = 0.0f;
	Vehicle4W->MinNormalizedTireLoadFiltered = 0.2f;
	Vehicle4W->MaxNormalizedTireLoad = 2.0f;
	Vehicle4W->MaxNormalizedTireLoadFiltered = 2.0f;*/
	UFUNCTION(Category = Setup, BlueprintCallable)
		void SetupVehicleParams(FVector CenterOfMass, USoundCue* SoundCue, UClass* FrontWheelClass, UClass* RearWheelClass, float MinNormalizedTireLoad, float MinNormalizedTireLoadFiltered, float MaxNormalizedTireLoad, float MaxNormalizedTireLoadFiltered, TArray<FVector2D> SteeringCurve, DiffType DiffTypeEnum, FVector IntertiaTensorScale);
	
	UFUNCTION(Category = Setup, BlueprintCallable)
		void SwitchToBoostEngine();
	UFUNCTION(Category = Setup, BlueprintCallable)
		void SwitchToStandardEngine();
	UFUNCTION(Category = Setup, BlueprintCallable)
		void SetupStandardEngine(float MaxRPM, FVector2D TorqueKey1, FVector2D TorqueKey2, FVector2D TorqueKey3);
	UFUNCTION(Category = Setup, BlueprintCallable)
		void SetupBoostEngine(float MaxRPM, FVector2D TorqueKey1, FVector2D TorqueKey2, FVector2D TorqueKey3);
	UFUNCTION(Category = Setup, BlueprintCallable)
		void Test(int32 f);
	
	//UPROPERTY(Category = Engine, BlueprintReadWrite, EditAnywhere)
		float StandardMaxRPM;

	/** Initial offset of incar camera */
	FVector InternalCameraOrigin;

	// Begin Pawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End Pawn interface

	// Begin Actor interface
	virtual void Tick(float Delta) override;

	void SetControllerReference(APlayerController* Controller);

	UFUNCTION()
		void EnableRace();
	UFUNCTION()
		void DisableRace();

	/////////////Replication
public:
	
	UPROPERTY()
		float DeltaTotal = 0;
	UPROPERTY()
		float FDelta;
	UPROPERTY()
		FVector CorrectedLocation;
	UPROPERTY()
		FRotator CorrectedRotation;
	UPROPERTY(BluePrintReadOnly)
		FVector NextLocation;
	UPROPERTY()
		FRotator NextRotation;
	UPROPERTY()
		FVector CurLocation;
	UPROPERTY()
		FRotator CurRotation;


	UPROPERTY()
		bool bCanRace = true;

private:
	
	/*UPROPERTY(ReplicatedUsing = OnRep_Local)
		float Local;
	UFUNCTION()
		void OnRep_Local();*/
	UFUNCTION()
		void TagTimerEvent();
	UFUNCTION()
		void SendClientStateUpdate();
	UFUNCTION(Server, Unreliable, WithValidation)
		void Server_UpdateClientState(FVector Loc, FRotator Rot, FVector AngVel, FVector LinVel);
	UFUNCTION(NetMulticast, Unreliable, WithValidation)
		void NetMulticast_UpdateClientState(FVector Loc, FRotator Rot, FVector AngVel, FVector LinVel);

	/*
	UPROPERTY(ReplicatedUsing = OnRep_Rotation)
		FRotator Rotation;
	UFUNCTION()
		void OnRep_Rotation();
		*/

	/*
	UFUNCTION(Server, Unreliable, WithValidation)
		void Server_ClientMoveForward(float Val);
	UFUNCTION()
		void CorrectLocation();
	UFUNCTION()
		void CorrectRotation();
	
	
	UFUNCTION()
		void UpdateClientState(FVector Loc, FRotator Rot, FVector AngVel, FVector LinVel);
	

	UFUNCTION()
		void CorrectionTimelineUpdateFloatLocation(float Val);
	UFUNCTION()
		void CorrectionTimelineUpdateFloatRotation(float Val);
		*/
	UPROPERTY()
		UCurveFloat* FCurve;
	UPROPERTY()
		UTimelineComponent* CorrectionTimelineLocation;
	UPROPERTY()
		UTimelineComponent* CorrectionTimelineRotation;
	
	FTimerHandle MyTimerHandle;

	FOnTimelineFloat InterpFunctionLocation{};
	FOnTimelineFloat InterpFunctionRotation{};

	UPROPERTY()
		APlayerController* PlayerController;
protected:
	virtual void BeginPlay() override;

	

public:
	// End Actor interface

	/** Handle pressing forwards */
	void MoveForward(float Val);

	/** Setup the strings used on the hud */
	void SetupInCarHUD();

	/** Update the physics material used by the vehicle mesh */
	void UpdatePhysicsMaterial();

	/** Handle pressing right */
	void MoveRight(float Val);
	/** Handle handbrake pressed */
	void OnHandbrakePressed();
	/** Handle handbrake released */
	void OnHandbrakeReleased();
	/** Switch between cameras */
	void OnToggleCamera();
	/** Handle reset VR device */
	void OnResetVR();

	static const FName LookUpBinding;
	static const FName LookRightBinding;
	static const FName EngineAudioRPM;

	

private:
	/** 
	 * Activate In-Car camera. Enable camera and sets visibility of incar hud display
	 *
	 * @param	bState true will enable in car view and set visibility of various
	 */
	void EnableIncarView( const bool bState );

	/** Update the gear and speed strings */
	void UpdateHUDStrings();

	/* Are we on a 'slippery' surface */
	bool bIsLowFriction;
	/** Slippery Material instance */
	UPhysicalMaterial* SlipperyMaterial;
	/** Non Slippery Material instance */
	UPhysicalMaterial* NonSlipperyMaterial;

	FVehicleEngineData StandardEngineSetup;
	FVehicleEngineData BoostEngineSetup;

	int SteeringInput = 0;

public:
	/** Returns SpringArm subobject **/
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }
	/** Returns Camera subobject **/
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	/** Returns InternalCamera subobject **/
	FORCEINLINE UCameraComponent* GetInternalCamera() const { return InternalCamera; }
	/** Returns InCarSpeed subobject **/
	FORCEINLINE UTextRenderComponent* GetInCarSpeed() const { return InCarSpeed; }
	/** Returns InCarGear subobject **/
	FORCEINLINE UTextRenderComponent* GetInCarGear() const { return InCarGear; }
	/** Returns EngineSoundComponent subobject **/
	FORCEINLINE UAudioComponent* GetEngineSoundComponent() const { return EngineSoundComponent; }


	///////////TAG GAMEPLAY//////////////
	UPROPERTY(Category = Tag, BlueprintReadWrite, EditAnywhere)
		UBoxComponent* TagBox;
	UPROPERTY(Category = Tag, BlueprintReadWrite, EditAnywhere)
		bool bIsIt = false;
	UPROPERTY()
		TArray<AActor*> OverlappingActors;
	UPROPERTY(Category = Tag, BlueprintReadWrite, EditAnywhere)
		TSubclassOf<ACarGame_416Pawn> ItClass;
	UPROPERTY(Category = Tag, BlueprintReadWrite, EditAnywhere)
		TSubclassOf<ACarGame_416Pawn> NotItClass;


	///////////// Wheel Effects ////////////////////
	/** FX under wheel on asphalt */
	UPROPERTY(EditDefaultsOnly, Category = Effect)
		UParticleSystem* AsphaltFX;
	/** FX under wheel on dirt */
	UPROPERTY(EditDefaultsOnly, Category = Effect)
		UParticleSystem* DirtFX;
	/** FX under wheel on grass */
	UPROPERTY(EditDefaultsOnly, Category = Effect)
		UParticleSystem* GrassFX;
	UPROPERTY(EditDefaultsOnly, Category = Effect)
		float AsphaltMinSpeed;
	/** min speed to show FX on dirt */
	UPROPERTY(EditDefaultsOnly, Category = Effect)
		float DirtMinSpeed;
	/** min speed to show FX on grass */
	UPROPERTY(EditDefaultsOnly, Category = Effect)
		float GrassMinSpeed;
	/** dust FX components */
	UPROPERTY(Transient)
		UParticleSystemComponent* DustPSC[4];
	bool bTiresTouchingGround = false;

	/** determine correct FX */
	UParticleSystem* GetDustFX(UPhysicalMaterial* PhysMaterial, float CurrentSpeed);
	void UpdateWheelEffects(float DeltaTime);
	void SpawnNewWheelEffect(int WheelIndex);

private:
	UFUNCTION()
		void Tag(FVector AngVel, FVector LinVel, bool GotTagged);

protected:
	/** get FX for material type */
	UParticleSystem* GetWheelFX(TEnumAsByte<EPhysicalSurface> MaterialType);

	/** get min speed for material type */
	float GetMinSpeed(TEnumAsByte<EPhysicalSurface> MaterialType);

};
