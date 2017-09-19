#pragma once

UENUM(BlueprintType)
enum class FailMessage : uint8
{
	NOT_ENOUGH_DRIVERS UMETA(DisplayName = "Not Enough Drivers"),
	NOT_ENOUGH_NONDRIVERS UMETA(DisplayName = "Not Enough Non Drivers"),
	PLAYERS_NOT_READY UMETA(DisplayName = "Players not ready")
};

UENUM(BlueprintType)
enum class PlayerRole : uint8
{
	Driver UMETA(DisplayName = "Driver"),
	NonDriver UMETA(DisplayName = "NonDriver"),
	Spectator UMETA(DisplayName = "Spectator"),
	Lobby UMETA(DisplayName = "Lobby"),
	None UMETA(DisplayName = "None")
};

UENUM(BlueprintType)
enum class ChatColor : uint8
{
	SERVER UMETA(DisplayName = "Server"),
	PLAYER UMETA(DisplayName = "Player"),
	GAME UMETA(DisplayName = "Game")
};

UENUM(BlueprintType)
enum class EGameMode : uint8
{
	RACETAG UMETA(DisplayName = "Race Tag"),
	FREEZETAG UMETA(DisplayName = "Freeze Tag"),
	HIDEANDSEEK UMETA(DisplayName = "Hide And Seek"),
	LOBBY UMETA(DisplayName = "Lobby"),
	MAINMENU UMETA(DisplayName = "Main Menu")
};