// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TimerManager.h"
#include "../../C++/Interface/GameState_Interface.h"
#include "../../C++/Interface/CardsInHand_Interface.h"
#include "../../CardGame/Structs/Structs.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../CardGame/WHOO.h"
#include "../AIWidget/AI_InfoBarWidget.h"
#include "../../C++/Interface/PlayerState_Interface.h"
#include "../../C++/Interface/Controller_Interface.h"
#include "../../C++/Interface/Deck_Interface.h"
#include "CardGameAIController.generated.h"

class ABoardPlayer;
class ACardGameAI_Pawn;
class ACardGameState;
class ACardGameMode;
class ACard3D;
/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API ACardGameAIController : public AAIController, public IGameState_Interface, public ICardsInHand_Interface, public IController_Interface, public IDeck_Interface, public IWHOO
{
	GENERATED_BODY()
public:
	ACardGameAIController();

	virtual void OnPossess(APawn* InPawn) override;

	virtual void BeginPlay() override;

	WHCoroTask* CoroBeginPlay();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool ChangeActivePlayerTurn_C(bool InTurnActive);
	virtual bool ChangeActivePlayerTurn_C_Implementation(bool TurnActive_P) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool GetPlayerDeck_C(TArray<FName>& Deck);
	virtual bool GetPlayerDeck_C_Implementation(TArray<FName>& Deck) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool MatchBegin_C();
	virtual bool MatchBegin_C_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool MatchEnd_C(EEndGameResults Result);
	virtual bool MatchEnd_C_Implementation(EEndGameResults Result);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool RequestChangePlayerTurn_C();
	virtual bool RequestChangePlayerTurn_C_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool DrawCard_C(FName CardNaem, bool IgnoreMaxCards, int32 NumberOfCardsToDraw);
	virtual bool DrawCard_C_Implementation(FName CardNaem, bool IgnoreMaxCards, int32 NumberOfCardsToDraw);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool AddCardToPlayersHand_C(FName CardName);
	virtual bool AddCardToPlayersHand_C_Implementation(FName CardName);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool RemoveCardFromDeck_C(bool RemoveAll, int32 IndexToRemove);
	virtual bool RemoveCardFromDeck_C_Implementation(bool RemoveAll, int32 IndexToRemove);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool RemoveCardFromHand_C(FName Card, int32 Index, bool RemoveAll);
	virtual  bool RemoveCardFromHand_C_Implementation(FName Card, int32 Index, bool RemoveAll);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	ACard3D* CreatePlayableCard_C(FTransform SpawnTransform);
	virtual ACard3D* CreatePlayableCard_C_Implementation(FTransform SpawnTransform);

	void Event_AddCardToHand_C();

	UFUNCTION(BlueprintImplementableEvent)
	TArray<FName> SortCardsByFocus(const TArray<FName>& CardsArray, const TArray<EAIPersonalityFocus>& FocusArray, bool IncludeOrderByValue);

	UFUNCTION(BlueprintCallable)
	bool AIPlayCard(const FName& CardName, ECardSet CardSet);

	UFUNCTION(BlueprintCallable)
	bool RunCardInteraction(FSCardInteraction AICardInteraction_Struct);

	UFUNCTION(BlueprintCallable)
	void UpdateAIPlayerState();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool EvaluateBoardState();

	UFUNCTION(BlueprintCallable)
	void UpdateAIState();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure)
	float GetFocusPriority(EAIPersonalityFocus Focus, EPriority& ReturnPriority);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CalculatePriorityFocus();

	void LoadAIPersonality(FName RowName);

	UFUNCTION(BlueprintCallable)
	void SetupAI();

	void SetupAIDeck();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CreateAIDebugUI();
	
public:

	UPROPERTY(BlueprintReadWrite, Category = "System")
	bool bAIReady;

	UPROPERTY(BlueprintReadWrite, Category = "System")
	bool bShuffleDeck = true;

	UPROPERTY(BlueprintReadWrite, Category = "System")
	bool bPrintAIDebugLogs;

	UPROPERTY(BlueprintReadWrite, Category = "System")
	bool bTurnActive;

	UPROPERTY(BlueprintReadWrite, Category = "System")
	bool ShowDebugUI;

	UPROPERTY(BlueprintReadWrite, Category = "System")
	bool bEnableWeightedCards;

	UPROPERTY(BlueprintReadWrite, Category = "System")
	FName SelectedDeck;

	UPROPERTY(BlueprintReadWrite, Category = "System")
	ACardGameAI_Pawn* ControlledPawn_Ref;

	UPROPERTY(BlueprintReadWrite, Category = "System", Replicated)
	ABoardPlayer* BoardPlayer_Ref;

	UPROPERTY(BlueprintReadWrite, Category = "System")
	ACardGameMode* GameMode_Ref;

	UPROPERTY(BlueprintReadWrite, Category = "System")
	ACardGameState* GameState_Ref;

	UPROPERTY(BlueprintReadWrite, Category = "System")
	int32 MaxCardsInHand = 7;

	UPROPERTY(BlueprintReadWrite, Category = "AIPersonality")
	FName Default_Personality = "Personality_1";

	UPROPERTY(BlueprintReadWrite, Category = "AIPersonality")
	FName AI_PersonalityName;

	UPROPERTY(BlueprintReadWrite, Category = "AIPersonality")
	EAIDifficulty AI_Difficulty = EAIDifficulty::Easy;

	UPROPERTY(BlueprintReadWrite, Category = "AIPersonality")
	EAIRisk AI_Risk = EAIRisk::Low_Risk;

	UPROPERTY(BlueprintReadWrite, Category = "AIPersonality")
	float AI_PlaySpeed;

	UPROPERTY(BlueprintReadWrite, Category = "AIPersonality")
	int32 AI_Awareness;

	UPROPERTY(BlueprintReadWrite, Category = "AIPersonality")
	FSAIPersonality AI_Personality_Struct;

	UPROPERTY(BlueprintReadWrite, Category = "AIPersonality")
	TArray<EAIPersonalityFocus> AI_PersonalityFocus;

	UPROPERTY(BlueprintReadWrite, Category = "AIPersonality")
	FSPointAllocation AIPointAllocation_Struct;

	UPROPERTY(BlueprintReadWrite)
	UBlackboardComponent* AIBlackboard_Ref;

	UPROPERTY(BlueprintReadWrite, Category = "BlackBoardKeys")
	FName PlaySpeed = "PlaySpeed";

	UPROPERTY(BlueprintReadWrite, Category = "BlackBoardKeys")
	FName Awareness = "Awareness";

	UPROPERTY(BlueprintReadWrite, Category = "BlackBoardKeys")
	FName Difficulty = "Difficulty";

	UPROPERTY(BlueprintReadWrite, Category = "BlackBoardKeys")
	FName Risk = "Risk";

	UPROPERTY(BlueprintReadWrite, Category = "BlackBoardKeys")
	FName TurnState = "TurnState";

	UPROPERTY(BlueprintReadWrite, Category = "BlackBoardKeys")
	FName TurnActive = "bTurnActive";

	UPROPERTY(BlueprintReadWrite, Category = "Card Manager")
	int32 NumberOfCardsToAdd;

	UPROPERTY(BlueprintReadWrite, Category = "Card Manager")
	int32 CardsInFirstHand = 5;

	UPROPERTY(BlueprintReadWrite, Category = "Card Manager")
	float CardPickupDelay = 0.2;

	UPROPERTY(BlueprintReadWrite, Category = "Card Manager")
	FName CardToAdd;

	UPROPERTY(BlueprintReadWrite, Category = "Card Manager", ReplicatedUsing = OnRep_AICardsInHand)
	TArray<FName> AICardsInHand;

	UFUNCTION()
	void OnRep_AICardsInHand();

	UPROPERTY(BlueprintReadWrite, Category = "AIPlayer", Replicated)
	TArray<FName> AIDeck;

	UPROPERTY(BlueprintReadWrite, Category = "AIPlayer")
	TArray<FName> AIPlayableDecks;

	UPROPERTY(BlueprintReadWrite, Category = "AIPlayer")
	TArray<FName> TempDeck;

	UPROPERTY(BlueprintReadWrite, Category = "Play Data")
	TArray<FSPlaySub> PlayList;

	UPROPERTY(BlueprintReadWrite, Category = "Play Data")
	TArray<FName> PlayableCardsInHandList;

	UPROPERTY(BlueprintReadWrite, Category = "Play Data")
	TArray<FSCardInteraction> ValidPlaysForTurn_Array;

	UPROPERTY(BlueprintReadWrite, Category = "Play Data")
	TArray<ACard3D*> CardsOnBoardWithTurnPoints;

	UPROPERTY(BlueprintReadWrite, Category = "Play Data")
	TArray<EAIPersonalityFocus> TEMP_AIFocusPriority;

	UPROPERTY(BlueprintReadWrite, Category = "Play Data")
	TArray<ECardType> ValidInteractionTypes;

	UPROPERTY(BlueprintReadWrite, Category = "Play Data")
	FSEvaluationState EvaluationStateTotal_Struct;

	UPROPERTY(BlueprintReadWrite, Category = "Play Data")
	FSAIState EvaluationState_Self;

	UPROPERTY(BlueprintReadWrite, Category = "Play Data")
	FSAIState EvaluationState_Opponent;

	UPROPERTY(BlueprintReadWrite, Category = "Priority | Arrays")
	TArray<EAIPersonalityFocus> ActivePriorityFocusList_Array;

	UPROPERTY(BlueprintReadWrite, Category = "Priority | Arrays")
	TArray<EAIPersonalityFocus> PriorityFocusList_Array;

	UPROPERTY(BlueprintReadWrite, Category = "Priority | Arrays")
	TArray<EPriority> PriorityList_Array;

	UPROPERTY(BlueprintReadWrite, Category = "Priority | Arrays")
	TArray<EPriority> Temp_PriorityList_Array;

	UPROPERTY(BlueprintReadWrite, Category = "Priority | Arrays")
	TArray<EPriority> Temp_Priority_Array;

	UPROPERTY(BlueprintReadWrite, Category = "Priority | Arrays")
	TArray<float> PriorityValue_Array;

	UPROPERTY(BlueprintReadWrite, Category = "Priority | StateValues")
	float Health_Low = 0.1;

	UPROPERTY(BlueprintReadWrite, Category = "Priority | StateValues")
	float CardsInHand_Low = 0.2;

	UPROPERTY(BlueprintReadWrite, Category = "Priority | StateValues")
	float BoardState_Low = 0.3;

	bool bDoOnce = true;

	FTimerHandle CreateAIDebugUIHandle;

	FTimerHandle DrawCardHandle;
};
