// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Online.h"
#include "FindSessionsCallbackProxy.h"
#include "Engine/NetDriver.h"
#include "MyEnums.h"
#include "NetWorkedGameInstance.generated.h"

class ACarGame_PlayerController;
/**
 * D:\Program Files\Epic Games\UE_4.16\Engine\Source\Runtime\Engine\Classes\Engine\NetDriver.h
 */

USTRUCT(BlueprintType)
struct FCustomSessionSearchResult
{
	GENERATED_BODY();
	UPROPERTY(BlueprintReadWrite, Category = "Network")
	FName GameName;
	UPROPERTY(BlueprintReadWrite, Category = "Network")
	FName Map;
	UPROPERTY(BlueprintReadWrite, Category = "Network")
	FBlueprintSessionResult BPSessionResult;
};

UCLASS()
class CARGAME_416_API UNetWorkedGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UNetWorkedGameInstance(const FObjectInitializer& ObjectInitializer);

	/*****************************
	*	Function to host a game!
	*
	*	@Param		UserID			User that started the request
	*	@Param		SessionName		Name of the Session
	*	@Param		bIsLAN			Is this is LAN Game?
	*	@Param		bIsPresence		"Is the Session to create a presence Session"
	*	@Param		MaxNumPlayers	        Number of Maximum allowed players on this "Session" (Server)
	*/
	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers, FName GameName);
	
	UFUNCTION(BlueprintCallable, Category = "Network")
		void StartOnlineGame(bool bIsLAN, int32 MaxNumPlayers, FName SelectedMapName, FName GameName);

	// Delegate called when session created
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	//Delegate called when session started
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	//Handles registered delgates for creating/starting a session
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;

	TSharedPtr<class FOnlineSessionSettings> SessionSettings;

	/**
	*	Delegate fired when a session create request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	/**
	*	Func fired when a session start request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);

	/***********************************
	*	Func for finding online sessions
	*
	*	@param UserId user that initiated the request
	*	@param bIsLAN are we searching LAN matches
	*	@param bIsPresence are we searching presence sessions
	*/
	void FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence);

	UFUNCTION(BlueprintCallable, Category = "Network")
		void FindOnlineGames(bool bIsLAN);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Network")
		void SessionResultsReady();
	//Delegate for searching sessions
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	// Handle to registered delegate for searching a session
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	/**
	*	Func fired when a session search query has completed
	*
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnFindSessionsComplete(bool bWasSuccessful);

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;
	
	/*********************************************
	*	Func to join a session via search result
	*
	*	@param SessionName name of session
	*	@param SearchResult Session to join
	*
	*	@return bool true if successful, false otherwise
	*/
	bool JoinOnlineSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	UFUNCTION(BlueprintCallable, Category = "Network")
	void JoinOnlineGame(int32 SessionSearchResultIndex, FName GameName);

	// delegate for joining a session
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	// handle for joinsession delegate
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	/**
	*	Func fired when a session join request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	/**********************************
	*	Destroy Session
	*/
	UFUNCTION(BlueprintCallable, Category = "Network")
		void DestroySessionAndLeaveGame();

	//Delegate for destroying a session
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	//handle for OnDestroySessionCompleteDelegate
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
	/**
	*	Delegate fired when a destroying an online session has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	void Init();

	/*NETWORK ERROR HANDLING FUNCS************************/
	void SetupNetworkErrorHandlers();
	void HandleNetworkError(UWorld * World, UNetDriver * NetDriver, ENetworkFailure::Type FailureType, const FString & ErrorString);
	void HandleTravelError(UWorld * World, ETravelFailure::Type FailureType, const FString & ErrorString);






	//////////Properties

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network")
		FString MainMenuMapName = FString("Main");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network")
		FName MapName = TEXT("TestMap");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network")
		bool bIsLAN;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network")
		int32 MaxNumPlayers;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network")
		TArray<struct FCustomSessionSearchResult> SessionResultStructArray;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network")
		bool bSessionSearchResultsReady = false;

	UPROPERTY(BlueprintReadWrite, Category = "Network")
		bool bErrorStatus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network")
		FString ErrorMessage;

	UPROPERTY()
		TArray<ACarGame_PlayerController*> PlayerControllers;

	///// SESSION SETTINGS
	UPROPERTY()
	FName SETTING_GAMENAME = TEXT("SETTING_GAMENAME");
	UPROPERTY(BlueprintReadOnly, Category = Network)
		FName MyGameName;

	///// RETURN TO LOBBY
	UFUNCTION()
		void TravelToTransitionLevel();
	UFUNCTION()
		void TravelToNextLevel();

	///// GAME SETTINGS
	UPROPERTY()
		EGameMode NextGameMode;
	UPROPERTY()
		FString NextMapName;


	

};
