// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameFramework/Controller.h"
#include "CCGToolkit/C++/Enums/CardGameEnums.h"
#include "GamePlay/Card3D.h"
#include "GamePlay/CardPlacement.h"
#include "GamePlay/Graveyard.h"
#include "../CardGame/Structs/Structs.h"
#include "CardGamePlayerController.h"
#include "Widgets/GamePlay/CardManagerWidget.h"
#include "CardGameMode.h"
#include "Interface/GameState_Interface.h"
#include "CardGameInstance.h"
#include "CardGameState.generated.h"

/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API ACardGameState : public AGameState ,public	IGameState_Interface
{
	GENERATED_BODY()
protected:
	ACardGameState();

public:
	virtual void BeginPlay() override;
	void EventBeginPlay();

	UFUNCTION(Server, Unreliable)
	void OnGameStart_C();
	void OnGameStart_C_Implementation();

	UFUNCTION(Server, Reliable)
	void Server_RequestChangeTurnState_C(AController* Controller);
	void Server_RequestChangeTurnState_C_Implementation(AController* Controller);

	UFUNCTION(Server, Unreliable)
	void Server_ForceChangeTurnState_C();
	void Server_ForceChangeTurnState_C_Implementation();

	UFUNCTION(Server, Reliable)
	void Server_NotifyEndGameState_C(EEndGameResults Player1Result, EEndGameResults Player2Result);
	void Server_NotifyEndGameState_C_Implementation(EEndGameResults Player1Result, EEndGameResults Player2Result);

	void GameTimer_C();

	void TurnTimer_C();

	void RestetTurnTimer_C();

	bool CheckPlayerTurnState_C(EGameTurn GameTurnState);

	bool RequestChangeTurnState_C(AController* Controller);

	void AddCardToBoard_C(ACard3D* CardReference, int32 PlayerID);

	void RemoveCardOnBoard_C(ACard3D* CardReference, int32 PlayerID);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	void GetBoardState_C(int32 PlayerID, TArray<ACard3D*>& PlayerCards, TArray<ACard3D*>& OppenedCards);

	void CompilePlacementsPerPlayer_C();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	void GetCardPlacementReferences_C(int32 PlayerID, TArray<ACardPlacement*>& PlacementArray, int32& Total);

	void GetGraveyardReferencesPerPlayer_C();

	AGraveyard* GetGraveyardReference_C(int32 PlayerID);

	void RecordGameStateSnapshot_C();

	void RecordBattleHistory_C(FSBattleHistory BattleHistoryStruct);

	void RotatePlayerTurn_C();

	void EndOfPlayerTurn_C(int32 PlayerID);

	void BeginPlayerTurn_C(int32 PlayerID);

	bool ValidatePlayerTurnChange_C(ACardGamePlayerController* PlayerController);

public:
	int32 CountdownTimer = 4;

	UCardGameInstance* CardGameInstance;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 GameTime_Seconds = 0;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 GameTime_Minutes = 0;

	FTimerHandle GameTimer_Ref;

	UPROPERTY(Replicated)
	EGameTurn GameTurnState = EGameTurn::Waiting;

	FTimerHandle TurnTimer_Ref;

	int32 TurnDuration_Seconds = 0;

	int32 TurnDuration_Minutes = 2;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 TurnTime_Seconds = 59;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 TurnTime_Minutes = 0;

	UCardManagerWidget* CardSystem_Ref;

	ACardGameMode* CardGameMode_Ref;

	EPlatform Platform = EPlatform::Windows;

	float GameSeconds = 1.0;

	TArray<AController*> PlayerTurn_Array;

	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<AActor*> PlayerState_Array;

	UPROPERTY(Replicated)
	bool bGameActive;
	
	bool EnableBattleHistory = true;

	UPROPERTY(Replicated)
	TArray<ACard3D*> ActiveCards_Player1;

	UPROPERTY(Replicated)
	TArray<ACard3D*> ActiveCards_Player2;

	UPROPERTY(Replicated)
	TArray<ACardPlacement*> CardPlacements_Player1;

	int32 TotalCardPlacementPositions_Player1 = 0;

	UPROPERTY(Replicated)
	TArray<ACardPlacement*> CardPlacements_Player2;

	int32 TotalCardPlacementPositions_Player2 = 0;

	UPROPERTY(Replicated)
	AGraveyard* Graveyards_Player1;

	UPROPERTY(Replicated)
	AGraveyard* Graveyards_Player2;

	TArray<FSGameStateSnapshot> GameSnapshot;

	TArray<FSBattleHistory> BattleHistory;

	TArray<ACard3D*> CardReferenceArray;

	FTimerHandle DelayTimerHandle;

	FTimerHandle NotifyEndGameStateTimerHandle;
};
