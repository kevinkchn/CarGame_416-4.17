// Fill out your copyright notice in the Description page of Project Settings.

#include "NetWorkedGameInstance.h"
#include "MyPlayerState.h"
#include "Engine/Engine.h"
#include "Classes/Kismet/GameplayStatics.h"
#include "Classes/Engine/LocalPlayer.h"
#include "CarGame_PlayerController.h"

UNetWorkedGameInstance::UNetWorkedGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//Bind function for creating a session
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UNetWorkedGameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UNetWorkedGameInstance::OnStartOnlineGameComplete);
	
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UNetWorkedGameInstance::OnFindSessionsComplete);
	
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UNetWorkedGameInstance::OnJoinSessionComplete);
	
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UNetWorkedGameInstance::OnDestroySessionComplete);

	SetupNetworkErrorHandlers();
}
void UNetWorkedGameInstance::Init()
{
	Super::Init();

	GetEngine()->OnNetworkFailure().AddUObject(this, &UNetWorkedGameInstance::HandleNetworkError);
	GetEngine()->OnTravelFailure().AddUObject(this, &UNetWorkedGameInstance::HandleTravelError);

}
void UNetWorkedGameInstance::SetupNetworkErrorHandlers()
{
	
}
////////////////////////Hosting Session/////////////////////////
void UNetWorkedGameInstance::StartOnlineGame(bool bIsLAN, int32 MaxNumPlayers, FName SelectedMapName, FName GameName)
{
	ULocalPlayer* const Player = GetFirstGamePlayer();
	// Call our custom HostSession function. GameSessionName is a GameInstance variable
	MapName = SelectedMapName;
	MyGameName = GameName;
	HostSession(Player->GetPreferredUniqueNetId(), GameSessionName, bIsLAN, true, MaxNumPlayers, GameName);
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Attempting Host Session"));
}
bool UNetWorkedGameInstance::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers, FName GameName)
{
	//Get OnlineSubsystem to work wth
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	
	if (OnlineSub)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Attempting Host Session"));
		//get session interface so can call "CreateSession" func on it
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid())
		{
			//Fill in Session settings we want to use
			SessionSettings = MakeShareable(new FOnlineSessionSettings());
			
			SessionSettings->bIsLANMatch = bIsLAN;
			SessionSettings->bUsesPresence = bIsPresence;
			SessionSettings->NumPublicConnections = MaxNumPlayers;
			SessionSettings->NumPrivateConnections = 0;
			SessionSettings->bAllowInvites = true;
			SessionSettings->bAllowJoinInProgress = true;
			SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
			SessionSettings->bShouldAdvertise = true;
			
			//SessionSettings->Set(SETTING_MAPNAME, MapName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);
			SessionSettings->Set(SETTING_GAMENAME, GameName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);
			//set delegate to the Handle of the SessionInterface
			OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
			//Delegate should be caled when this is complete
			return Sessions->CreateSession(*UserId, SessionName, *SessionSettings);

		}
	}
	else
	{	
		///////////////dbug
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No Online Subsystem found"));
	}

	return false;
}

void UNetWorkedGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	/////////////////////dbug
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnCreateSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	// Get the OnlineSubsystem so we can get the Session Interface
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		//Get Session Interface to call StartSession func
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			//clear CreateSessionComplete delegate handle
			Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				//Set StartSession delegate handle
				OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

				///////////////dbug
				FOnlineSessionSettings* sets = Sessions->GetSessionSettings(SessionName);
				//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Host Session Settings: bAllowInvites %d, bAllowJoinInProgress %d, bAllowJoinViaPresence %d, bAllowJoinViaPresenceFriendsOnly %d, bAntiCheatProtected %d, bIsDedicated %d, bIsLANMatch %d, bShouldAdvertise %d, 	BuildUniqueId %d, bUsesPresence %d, bUsesStats %d, NumPublicConnections %d"), 
																							//sets->bAllowInvites,sets->bAllowJoinInProgress, sets->bAllowJoinViaPresence, sets->bAllowJoinViaPresenceFriendsOnly, sets->bAntiCheatProtected, sets->bIsDedicated, sets->bIsLANMatch, sets->bShouldAdvertise, sets->BuildUniqueId, sets->bUsesPresence, sets->bUsesStats, sets->NumPublicConnections));


				//StartSessionComplete will call after this
				Sessions->StartSession(SessionName);
			}
		}
	}
}

void UNetWorkedGameInstance::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
	/////////////////////
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnStartSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	// Get the Online Subsystem so we can get the Session Interface
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		//get session interface
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			//clear delegate
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
		}
	}
	if (bWasSuccessful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName("Lobby"), true, "?listen");
	}
}

///////////////Implementation for Finding Sessions//////////////////
void UNetWorkedGameInstance::FindOnlineGames(bool bIsLAN)
{
	ULocalPlayer* const Player = GetFirstGamePlayer();
	FindSessions(Player->GetPreferredUniqueNetId(), bIsLAN, true);
}
void UNetWorkedGameInstance::FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		//get session interface
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid())
		{
			SessionSearch = MakeShareable(new FOnlineSessionSearch());

			////dbug
			//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Is Lan: %d"), bIsLAN));
			
			SessionSearch->bIsLanQuery = bIsLAN;
			SessionSearch->MaxSearchResults = 10000;
			SessionSearch->PingBucketSize = 50;
			//if (bIsPresence)
			//{
				SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);

			//}
			SessionSearch->QuerySettings.Set(SETTING_MAPNAME, FString(TEXT("")), EOnlineComparisonOp::NotEquals);
			TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();

			//Set the delegate to the delegate handle of the OnFindSessionsComplete func
			OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
			
			
			//dbug
			//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Host Session Settings: bAllowInvites %s, "),
				//(*(*SessionSearch->QuerySettings.SearchParams.Find(SEARCH_PRESENCE)).ToString())));
			//(*(*SessionSearch->QuerySettings.SearchParams.Find(SEARCH_PRESENCE)).ToString());

			//Call SessionInterface func, which will call delegate once finished
			Sessions->FindSessions(*UserId, SearchSettingsRef);
		}
	}
	else
	{
		//if something goes wrong, call with false
		OnFindSessionsComplete(false);
	}
}

void UNetWorkedGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	/////////////////////dbug
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnFindSessionsComplete bSuccess: %d"), bWasSuccessful));
	
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			//clear delegate handle
			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
			
			////////////debug
			//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Num Search Results: %d"), SessionSearch->SearchResults.Num()));
			
			if (SessionSearch->SearchResults.Num() > 0)
			{
				//////////////dbug
				// "SessionSearch->SearchResults" is an Array that contains all the information. You can access the Session in this and get a lot of information.
				// This can be customized later on with your own classes to add more information that can be set and displayed
				SessionResultStructArray.Empty();
				for (int32 SearchIdx = 0; SearchIdx < SessionSearch->SearchResults.Num(); SearchIdx++)
				{
					FBlueprintSessionResult result;
					result.OnlineResult = SessionSearch->SearchResults[SearchIdx];
					FCustomSessionSearchResult customResult;
					customResult.BPSessionResult = result;

					//retrieve game name session setting
					FString name;
					(*SessionSearch->SearchResults[SearchIdx].Session.SessionSettings.Settings.Find(SETTING_GAMENAME)).Data.GetValue(name);
					customResult.GameName = FName(*name);
					//customResult.Map = (*SessionSearch->SearchResults[SearchIdx].Session.SessionSettings.Settings.Find(SETTING_MAPNAME)).ToString();

					SessionResultStructArray.Add(customResult);
					// OwningUserName is just the SessionName for now. I guess you can create your own Host Settings class and GameSession Class and add a proper GameServer Name here.
					// This is something you can't do in Blueprint for example!
					//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Session Number: %d | Sessionname: %s "), SearchIdx + 1, *(SessionSearch->SearchResults[SearchIdx].Session.OwningUserName)));
				}
				/////////////////
			}
			bSessionSearchResultsReady = bWasSuccessful;
			SessionResultsReady();
		}
	}
}
void UNetWorkedGameInstance::SessionResultsReady_Implementation()
{

}
/////////////////////////Joining Session//////////////////////////
void UNetWorkedGameInstance::JoinOnlineGame(int32 SessionSearchResultIndex, FName GameName)
{
	ULocalPlayer* const Player = GetFirstGamePlayer();

	FOnlineSessionSearchResult SearchResult;

	//filter session from ourself
	if (SessionSearch->SearchResults[SessionSearchResultIndex].Session.OwningUserId != Player->GetPreferredUniqueNetId())
	{
		MyGameName = GameName;
		SearchResult = SessionSearch->SearchResults[SessionSearchResultIndex];
		JoinOnlineSession(Player->GetPreferredUniqueNetId(), GameSessionName, SearchResult);
	}

	
}
bool UNetWorkedGameInstance::JoinOnlineSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult)
{
	//return bool
	bool bSuccessful = false;

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid())
		{
			OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
			// Call the "JoinSession" Function with the passed "SearchResult". 
			bSuccessful = Sessions->JoinSession(*UserId, SessionName, SearchResult);
		}
	}
	return bSuccessful;
}

void UNetWorkedGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	//////////////////dbug
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnJoinSessionComplete %s, %d"), *SessionName.ToString(), static_cast<int32>(Result)));

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		
		if (Sessions.IsValid())
		{
			//clear delegate
			Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);

			//get first local PlayerController so can call "ClientTravel" to get to ServerMap
			APlayerController* const PlayerController = GetFirstLocalPlayerController();
			//APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

			// We need a FString to use ClientTravel and we can let the SessionInterface contruct such a
			// String for us by giving him the SessionName and an empty String. We want to do this, because
			// Every OnlineSubsystem uses different TravelURLs
			FString TravelURL;
			if (PlayerController && Sessions->GetResolvedConnectString(SessionName, TravelURL))
			{
				//call ClientTravel
				PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
			}
		}
	}

}

////////////////////////////////Destroy Session////////////////////////////
void UNetWorkedGameInstance::DestroySessionAndLeaveGame()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid()) {
			Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
			Sessions->DestroySession(GameSessionName);
		}
	}
}
void UNetWorkedGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	/////////dbug
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnDestroySessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);

			if(bWasSuccessful)
			{
				UGameplayStatics::OpenLevel(GetWorld(), FName(*MainMenuMapName), true);
			}
		}
	}
}

/////////////////////////////HANDLE NETWORK ERRORS////////////////////////////
void UNetWorkedGameInstance::HandleNetworkError(UWorld * World, UNetDriver * NetDriver, ENetworkFailure::Type FailureType, const FString & ErrorString)
{
	/*ErrorMessage = FString(TEXT("Unkown Network Error."));
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ENetworkFailure"), true);
	if (EnumPtr)
	{
		ErrorMessage = EnumPtr->GetNameByValue((int64)FailureType).ToString();
		ErrorMessage.Append(FString(TEXT(": ")));
		ErrorMessage.Append(ErrorString);
	}
	bErrorStatus = true;
	DestroySessionAndLeaveGame();*/
}

void UNetWorkedGameInstance::HandleTravelError(UWorld * World, ETravelFailure::Type FailureType, const FString & ErrorString)
{
	/*ErrorMessage = FString(TEXT("Unkown Travel Error."));
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ENetworkFailure"), true);
	if (EnumPtr)
	{
		ErrorMessage = EnumPtr->GetNameByValue((int64)FailureType).ToString();
		ErrorMessage.Append(FString(TEXT(": ")));
		ErrorMessage.Append(ErrorString);
	}
	bErrorStatus = true;
	DestroySessionAndLeaveGame();*/
}

void UNetWorkedGameInstance::TravelToTransitionLevel()
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Server Travel: Transition")));
	GetWorld()->ServerTravel(FString("/Game/Maps/Transition") + FString("?game=/Game/Blueprints/GameModes/Transition_GameModeBaseBP.Transition_GameModeBaseBP"), true, false);
}

void UNetWorkedGameInstance::TravelToNextLevel()
{
	FString GM;
	switch (NextGameMode)
	{
	case EGameMode::LOBBY:
		GM = "Lobby_GameModeBP.Lobby_GameModeBP";
		break;
	case EGameMode::MAINMENU:
		GM = "MainMenu_GameModeBP.MainMenu_GameModeBP";
		break;
	case EGameMode::RACETAG:
		GM = "AsymmetricRace_GameModeBaseBP.AsymmetricRace_GameModeBaseBP";
		break;
	}
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Server Travel: %s"), *NextMapName));
	GetWorld()->ServerTravel(FString("/Game/Maps/") + NextMapName + FString("?game=/Game/Blueprints/GameModes/") + GM, true, false);
}






