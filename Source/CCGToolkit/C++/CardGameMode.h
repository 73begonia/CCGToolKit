// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/Actor.h"
#include "CCGToolkit/C++/Enums/CardGameEnums.h"
#include "CCGToolkit/CardGame/WHOO.h"
#include "CardGameMode.generated.h"

class ACardGameState;
class ABoardPlayer;
class ACardGamePlayerController;
/**
 *
 */
DECLARE_LOG_CATEGORY_CLASS(CardGameModeLog, Log, All);

UCLASS()
class CCGTOOLKIT_API ACardGameMode : public AGameMode, public IWHOO
{
	GENERATED_BODY()

public:
	ACardGameMode();

	virtual void BeginPlay() override;

	WHCoroTask* CoroBeginPlay();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	UFUNCTION(Reliable, Server)
	void CheckGamePreconditions_C();
	void CheckGamePreconditions_C_Implementation();

	UFUNCTION(Unreliable, Server)
	void ForceSpawnAIOpponent_C();
	void ForceSpawnAIOpponent_C_Implementation();

	UFUNCTION(Reliable, Server)
	void Server_CheckPlayerState_C();
	void Server_CheckPlayerState_C_Implementation();

	UFUNCTION(Reliable, Server)
	void Server_EndGame_C();
	void Server_EndGame_C_Implementation();

	void GameStartCountdown_C();

	void SetGameActiveAndOnGameStart();

	AController* AddPlayerToArray(AActor* PlayerState, AController* PlayerController);

	void SetCardGamePlayerID(UObject* PlayerState, AController* Controller);

	virtual void Logout(AController* Exiting) override;

	void RemovePlayerFromGame(AController* Controller);

	void SetBoardPlayerReference();

	void SetBoardPlayerReferences(AController* Controller);

	void CreateCardGameAIOpponent();

	bool CheckIsPlayerActiveState(int32 ControllerID);

	void CollectGameResults(EEndGameResults& Player1Result, EEndGameResults& Player2Result);

	void GetPlayerControllers(AController* Player1, AController* Player2);

	int32 CalculateManaForTurn(int32 PlayerTurn);

	int32 GetTurnMana(AController* Controller);

public:
	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<AController*> GameControllersArray;

	UPROPERTY(Replicated)
	TArray<AActor*> PlayerStateArray;

	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 PlayerStartingHealth = 30;

	UPROPERTY()
	TArray<ABoardPlayer*> BoardPlayersArray;

	int32 MaxNumOfPlayers = 2;

	ACardGameState* GameStateRef;

	ACardGamePlayerController* PlayerControllerRef;

	FTimerHandle GameStartTimerHandle;

	FTimerHandle CountDownTimerHandle;

	bool bSkipStartTimer;

	bool bGameActive;

	bool bSingleClientEnabled;

	float GameSeconds;

	int32 CountdownTimer;

	bool bSpectator;

	int32 ManaMin = 1;

	int32 ManaMax = 10;
};


