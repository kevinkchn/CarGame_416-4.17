// Fill out your copyright notice in the Description page of Project Settings.

#include "CarGame_PlayerController.h"
#include "CarGame_416Pawn.h"
#include "MyGameStateBase.h"
#include "Engine/Engine.h"
#include "Lobby_GameMode.h"
#include "MyPlayerState.h"
#include "Engine/GameInstance.h" 
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"


ACarGame_PlayerController::ACarGame_PlayerController()
{
	bReplicates = true;
}

void ACarGame_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("EnterPressed", EInputEvent::IE_Pressed, this, &ACarGame_PlayerController::EnterPressed);
	InputComponent->BindAction("EscapePressed", EInputEvent::IE_Pressed, this, &ACarGame_PlayerController::EscapePressed);
}
void ACarGame_PlayerController::EnterPressed()
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("EnterPressed: Fire")));
	BP_EnterPressed();
}
void ACarGame_PlayerController::EscapePressed()
{
	BP_EscapePressed();
}

void ACarGame_PlayerController::PlayerTick(float DeltaSeconds)
{
	Super::PlayerTick(DeltaSeconds);
	
	/*if (!CarPawn)
	{
		APawn* const controlledPawn = GetPawn();
		if (controlledPawn)
		{
			CarPawn = Cast<ACarGame_416Pawn>(controlledPawn);
			if (CarPawn)
			{
				CarPawn->SetControllerReference(this);
			}
		}
	}*/
}
void ACarGame_PlayerController::StartRacing()
{
	bCanRace = true;
	if (GetPawn())
	{
		((ACarGame_416Pawn*)GetPawn())->bCanRace = true;
	}
}
void ACarGame_PlayerController::Client_EndRacing_Implementation(bool Victory)
{
	bCanRace = false;
	if (GetPawn())
	{
		((ACarGame_416Pawn*)GetPawn())->DisableRace();
		BP_EndRacing(Victory);
	}
	
}
bool ACarGame_PlayerController::Client_EndRacing_Validate(bool Victory)
{
	return true;
}
/////////////////////////////CHAT////////////////////////////////
void ACarGame_PlayerController::SendChatMessage(const FText& Content)
{
	Server_SendChatMessage(Content);
}
void ACarGame_PlayerController::Server_SendChatMessage_Implementation(const FText& Content)
{
	AMyGameStateBase* const MyGameState = GetWorld() != NULL ? GetWorld()->GetGameState<AMyGameStateBase>() : NULL;
	(FText::FromString(PlayerState->PlayerName));
	MyGameState->SendChatMessage(FText::Format(NSLOCTEXT("CarGame", "ChatFmt", "{0}: {1}"), FText::FromString(PlayerState->PlayerName), Content));
}
bool ACarGame_PlayerController::Server_SendChatMessage_Validate(const FText& Content)
{
	return true;
}

/*
void ACarGame_PlayerController::Client_AssignTeam_Implementation(PlayerRole prole, FString Name)
{
	UNetWorkedGameInstance* PlayerInstance = Cast<UNetWorkedGameInstance>(GetGameInstance());
	switch (prole)
	{
	case PlayerRole::Driver:
		PlayerInstance->Role = PlayerRole::Driver;
		break;
	case PlayerRole::NonDriver:
		PlayerInstance->Role = PlayerRole::NonDriver;
		break;
	case PlayerRole::Spectator:
		PlayerInstance->Role = PlayerRole::Spectator;
		break;
	case PlayerRole::Lobby:
		PlayerInstance->Role = PlayerRole::Lobby;
		break;
	case PlayerRole::None:
		PlayerInstance->Role = PlayerRole::None;
		break;
	default:
		PlayerInstance->Role = PlayerRole::None;
		break;
	}
	SwitchTeam(prole);

	
}
bool ACarGame_PlayerController::Client_AssignTeam_Validate(PlayerRole prole, FString Name)
{
	return true;
}
*/
/////// FROM GAMEMODE - Add Players to Client's Lobby Instance Post Login
void ACarGame_PlayerController::JoinLobby(PlayerRole prole) // Add Self to lobby instance
{
	Client_JoinLobby(prole, PlayerState->PlayerName);
}
void ACarGame_PlayerController::AddToLobby(PlayerRole prole, const FString& Name) //Add other players to lobby instance
{
	Client_AddPlayerToLobby(prole, Name);
}

/////// Client Has pressed button to switch Teams
void ACarGame_PlayerController::FromBP_SwitchTeam(PlayerRole prole)
{
	Server_SwitchTeam(prole);
}
/////// Client Pressed Ready button ////////////
void ACarGame_PlayerController::FromBP_ReadyUp()
{
	AMyPlayerState* MyState = Cast<AMyPlayerState>(PlayerState);
	if (MyState->IsReady)
	{
		MyState->IsReady = false;
	}
	else
	{
		MyState->IsReady = true;
	}
}
////////////REPLICATED FUNCS/////////////////
//add other player to player's lobby
void ACarGame_PlayerController::Client_AddPlayerToLobby_Implementation(PlayerRole prole, const FString& Name)
{
	BP_AddPlayerToLobby(prole, Name);
}

bool ACarGame_PlayerController::Client_AddPlayerToLobby_Validate(PlayerRole prole, const FString& Name)
{
	return true;
}
// add self to lobby
void ACarGame_PlayerController::Client_JoinLobby_Implementation(PlayerRole prole, const FString& Name)
{
	//UNetWorkedGameInstance* PlayerInstance = Cast<UNetWorkedGameInstance>(GetGameInstance());
	BP_AddSelfToLobby(prole, Name);
}
bool ACarGame_PlayerController::Client_JoinLobby_Validate(PlayerRole prole, const FString& Name)
{
	return true;
}
///switch teams
void ACarGame_PlayerController::Server_SwitchTeam_Implementation(PlayerRole prole)
{
	ALobby_GameMode* GM = (ALobby_GameMode*)GetWorld()->GetAuthGameMode();
	if (GM->TryToSwitchTeams(this, prole))
	{
		NetMulticast_SwitchTeam(prole, PlayerState->PlayerName);
		return;
	}
	//fail case
	if (prole == PlayerRole::NonDriver)	
	{
		Client_SwitchTeamFailed(FailMessage::NOT_ENOUGH_DRIVERS);
	}
	
}
bool ACarGame_PlayerController::Server_SwitchTeam_Validate(PlayerRole prole)
{
	return true;
}
void ACarGame_PlayerController::NetMulticast_SwitchTeam_Implementation(PlayerRole prole, const FString& Name)
{
	BP_SwitchTeam(prole, Name);
}
bool ACarGame_PlayerController::NetMulticast_SwitchTeam_Validate(PlayerRole prole, const FString& Name)
{
	return true;
}
void ACarGame_PlayerController::Client_SwitchTeamFailed_Implementation(FailMessage Fail)
{
	BP_SwitchTeamFailed(Fail);
}
bool ACarGame_PlayerController::Client_SwitchTeamFailed_Validate(FailMessage Fail)
{
	return true;
}

void ACarGame_PlayerController::Client_RemoveWidgets_Implementation()
{
	//HideWidgets();
	//Object Iterator for All User Widgets! 
	for (TObjectIterator<UUserWidget> Itr; Itr; ++Itr)
	{
		UUserWidget* LiveWidget = *Itr;

		// If the Widget has no World, Ignore it (It's probably in the Content Browser!) 
		if (!LiveWidget->GetWorld())
		{
			continue;
		}
		else
		{
			LiveWidget->RemoveFromParent();
		}
	}
}
bool ACarGame_PlayerController::Client_RemoveWidgets_Validate()
{
	return true;
}


void ACarGame_PlayerController::TagSwitch(TSubclassOf<ACarGame_416Pawn> SwitchClass, FTransform SwitchTransform, AActor* DestroyActor, bool IsIt, FVector AngVel, FVector LinVel)
{
	DestroyActor->Destroy();
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APawn* PossessPawn = GetWorld()->SpawnActor<ACarGame_416Pawn>(SwitchClass, SwitchTransform, SpawnInfo);

	((ACarGame_416Pawn*)PossessPawn)->GetMesh()->SetPhysicsAngularVelocity(AngVel, false, NAME_None);
	((ACarGame_416Pawn*)PossessPawn)->GetMesh()->SetPhysicsLinearVelocity(LinVel, false, NAME_None);
	Possess(PossessPawn);

	Client_Tag(IsIt);
}

void ACarGame_PlayerController::Client_Tag_Implementation(bool IsIt)
{
	
	((AMyPlayerState*)PlayerState)->bIsIt = IsIt;
	BP_Tag(IsIt);
}
bool ACarGame_PlayerController::Client_Tag_Validate(bool IsIt)
{
	return true;
}

void ACarGame_PlayerController::Client_FinishGame_Implementation()
{
	AMyPlayerState* MPS = (AMyPlayerState*)PlayerState;
	if (MPS->bIsIt)
	{
		bCanRace = false;
		if (GetPawn())
		{
			((ACarGame_416Pawn*)GetPawn())->DisableRace();
			BP_EndRacing(false);
		}
	}
	BP_FinishGame();
}
bool ACarGame_PlayerController::Client_FinishGame_Validate()
{
	return true;
}
/*
IMPLEMENT BP CALLBACKS FOR SWITCHING TEAMS
*/






