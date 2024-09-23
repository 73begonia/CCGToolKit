// Fill out your copyright notice in the Decription page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CCGToolkit/C++/Interface/Controller_Interface.h"
#include "CCGToolkit/C++/Interface/Deck_Interface.h"
#include "CCGToolkit/C++/Interface/CardsInHand_Interface.h"
#include "CCGToolkit/C++/Interface/GameState_Interface.h"
#include "CCGToolkit/C++/Widgets/Additional/CountdownWidget.h"
#include "CCGToolkit/C++/Widgets/GamePlay/CardWidget.h"
#include "CCGToolkit/C++/CardGameState.h"
#include "CCGToolkit/C++/CardGamePlayerState.h"
#include "CCGToolkit/C++/GamePlay/BoardPlayer.h"
#include "CCGToolkit/C++/Widgets/GamePlay/PlayerGameUIWidget.h"
#include "CCGToolkit/C++/Widgets/GamePlay/OpponentUIWidget.h"
#include "GamePlay/CardPlacement.h"
#include "GamePlay/TargetDragSelection.h"
#include "../CardGame/Structs/Structs.h"

#include "CCGToolkit/CardGame/Structs/Structs.h"
#include "CCGToolkit/CardGame/Define/Define.h"
#include "CCGToolkit/CardGame/WHOO.h"
#include "CardGameInstance.h"
#include "InputCoreTypes.h"
#include "../StackStateMachine/UIManagerComponent.h"

#include "CardGamePlayerController.generated.h"


class UPlayerGameUIWidget;
class UCardWidget;
/**
 *
 */
UCLASS()
class CCGTOOLKIT_API ACardGamePlayerController : public APlayerController, public IController_Interface, public IDeck_Interface, public ICardsInHand_Interface, public IGameState_Interface, public IWHOO
{
	GENERATED_BODY()

public:

	WHCoroTask* CoroBeginPlay();

	UFUNCTION(Client, Reliable)
	void ClientPostLogin_C();
	void ClientPostLogin_C_Implementation();

	WHCoroTask* CoroClientPostLogin();

	UFUNCTION(Client, Reliable)
	void SetCountdownTimer_C(int32 Time);
	void SetCountdownTimer_C_Implementation(int32 Time);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool MatchEnd_C(EEndGameResults Result);
	virtual bool MatchEnd_C_Implementation(EEndGameResults Result) override;

	UFUNCTION(Client, Reliable)
	void EndClientMatchState_C(EEndGameResults NewParam);
	void EndClientMatchState_C_Implementation(EEndGameResults NewParam);

	UFUNCTION(Client, Reliable)
	void UpdateGameUI_C(bool ForceCleanUpdate);
	void UpdateGameUI_C_Implementation(bool ForceCleanUpdate);

	UFUNCTION(BlueprintCallable)
	void CallCreateCard_C(FName CardName, UDragDropOperation* DragDropOperation, ECardSet CardSet_P, int32 CardHandleIndex);
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void DropCard_C(bool CardPlayed, EErrors Errors);
	void DropCard_C_Implementation(bool CardPlayed, EErrors Errors);

	UFUNCTION(Client, Unreliable)
	void CreateDisplayMessage_C(const FString& Message, FLinearColor SpecifiedColor, bool ToScreen, float ScreenDuration, bool ToMessageLogger);
	void CreateDisplayMessage_C_Implementation(const FString& Message, FLinearColor SpecifiedColor, bool ToScreen, float ScreenDuration, bool ToMessageLogger);

	UFUNCTION(Client, Unreliable)
	void LogNotificationMessage_C(const FString& Message, FLinearColor Color);
	void LogNotificationMessage_C_Implementation(const FString& Message, FLinearColor Color);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool GetPlayerDeck_C(TArray<FName>& Deck);
	virtual bool GetPlayerDeck_C_Implementation(TArray<FName>& Deck) override;

	UFUNCTION(Client, Reliable)
	void Event_GetPlayerDeck_C();
	void Event_GetPlayerDeck_C_Implementation();

	UFUNCTION(BlueprintCallable)
	void OnDragCanceled_C();

	UFUNCTION(Client, Reliable)
	void DestroyClientCard_C();
	void DestroyClientCard_C_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool DrawCard_C(FName CardNaem, bool IgnoreMaxCards, int32 NumberOfCardsToDraw);
	virtual bool DrawCard_C_Implementation(FName CardNaem, bool IgnoreMaxCards, int32 NumberOfCardsToDraw) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	ACard3D* CreatePlayableCard_C(FTransform SpawnTransform);
	virtual ACard3D* CreatePlayableCard_C_Implementation(FTransform SpawnTransform) override;
	


	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable)
	void Developer_AddCardToHand_C(FName CardToAdd_P);
	bool Developer_AddCardToHand_C_Validate(FName CardToAdd_P);
	void Developer_AddCardToHand_C_Implementation(FName CardToAdd_P);

	void AddCardToHand_C();

	UFUNCTION(Client, Reliable)
	void AddCardToCardManager_C(FName CardName, ECardSet FromCardSet);
	void AddCardToCardManager_C_Implementation(FName CardName, ECardSet FromCardSet);

	UFUNCTION(Client, Reliable)
	void RemoveCardFromCardManager_C(UCardWidget* CardWidget_P, int32 OR_CardHandIndex, bool OR_RemoveALL);
	void RemoveCardFromCardManager_C_Implementation(UCardWidget* CardWidget_P, int32 OR_CardHandIndex, bool OR_RemoveALL);

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetupDeck_C();
	bool Server_SetupDeck_C_Validate();
	void Server_SetupDeck_C_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ReturnPlayerDeck_C(const FString& DeckName, const TArray<FName>& DeckArray);
	bool Server_ReturnPlayerDeck_C_Validate(const FString& DeckName, const TArray<FName>& DeckArray);
	void Server_ReturnPlayerDeck_C_Implementation(const FString& DeckName, const TArray<FName>& DeckArray);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool MatchBegin_C();
	virtual bool MatchBegin_C_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool AddCardToPlayersHand_C(FName CardName);
	virtual bool AddCardToPlayersHand_C_Implementation(FName CardName);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool ChangeActivePlayerTurn_C(bool TurnActive);
	virtual bool ChangeActivePlayerTurn_C_Implementation(bool TurnActive) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool RemoveCardFromDeck_C(bool RemoveAll, int32 IndexToRemove);
	virtual bool RemoveCardFromDeck_C_Implementation(bool RemoveAll, int32 IndexToRemove) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool RemoveCardFromHand_C(FName Card, int32 Index, bool RemoveAll);
	virtual bool RemoveCardFromHand_C_Implementation(FName Card, int32 Index, bool RemoveAll) override;

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_RequestChangeTurnState_C();
	bool Server_RequestChangeTurnState_C_Validate();
	void Server_RequestChangeTurnState_C_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdatePlayerHealth_C();
	bool Server_UpdatePlayerHealth_C_Validate();
	void Server_UpdatePlayerHealth_C_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdatePlayerState_C();
	bool Server_UpdatePlayerState_C_Validate();
	void Server_UpdatePlayerState_C_Implementation();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_PlayCard_C(FName CardName, ECardSet CardSet_P, ACardPlacement* GoalPlacement, int32 CardInHandIndex, FTransform SpawnTransform);
	bool Server_PlayCard_C_Validate(FName CardName, ECardSet InCardSet, ACardPlacement* GoalPlacement, int32 CardInHandIndex, FTransform SpawnTransform);
	void Server_PlayCard_C_Implementation(FName CardName, ECardSet CardSet_P, ACardPlacement* GoalPlacement, int32 CardInHandIndex, FTransform SpawnTransform);

	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable)
	void Server_RunCardInteraction_C(ACard3D* TalkingCard_P, ACard3D* RecievingCard_P, ABoardPlayer* BoardPlayer);
	bool Server_RunCardInteraction_C_Validate(ACard3D* TalkingCard_P, ACard3D* RecievingCard_P, ABoardPlayer* BoardPlayer);
	void Server_RunCardInteraction_C_Implementation(ACard3D* TalkingCard_P, ACard3D* RecievingCard_P, ABoardPlayer* BoardPlayer);

	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable)
	void SetSkipManaCheck_C(bool SkipCheck);
	bool SetSkipManaCheck_C_Validate(bool SkipCheck);
	void SetSkipManaCheck_C_Implementation(bool SkipCheck);

	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable)
	void ReshuffleDeck_C();
	bool ReshuffleDeck_C_Validate();
	void ReshuffleDeck_C_Implementation();

	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable)
	void ClearCardsInHand_C();
	bool ClearCardsInHand_C_Validate();
	void ClearCardsInHand_C_Implementation();

	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable)
	void SpawnAIOpponent_C();
	bool SpawnAIOpponent_C_Validate();
	void SpawnAIOpponent_C_Implementation();

	bool ValidatePlacementOwner(int32 PlayerIndex, int32 PlacementPlayerIndex, FSCard Card_Struct);

	void NotifyCardTurnActive_C();

	void NotifyCardsEndTurn_C();

	bool RemoveCardFromHandOLD_C(bool ClearAll, FName CardName, int32 IndexToRemove);

	UFUNCTION(BlueprintCallable)
	void SetCardLocation_C(FVector HoldLocation, ACard3D* Card, FRotator Rotation);

	UFUNCTION(BlueprintCallable)
	bool ValidateCardPlacement_C(AActor* InHitActor);

	ACard3D* CreatePlaceableCard_Client_C(FName Name, ECardSet CardSet_P, FVector SpawnTransformLocation);

	ACard3D* CreatePlaceableCard_Server_C(FTransform SpawnTransform);

	bool ServerValidateCardPlacement_C(ACardPlacement* CardPlacement, FSCard CardStruct, ACardPlacement*& ValidCardPlacement);

	ACard3D* SetCustomCardData_C(ACard3D* Card, bool ActiveAbility);

	void PlayCard_C(FName CardName, ECardSet CardSet_P, ACardPlacement* CardPlacement, int32 CardHandIndex, FTransform SpawnTransform);

	UFUNCTION(BlueprintCallable)
	void DetectCardInteraction_C();

	UFUNCTION(BlueprintCallable)
	bool ValidateInteractionState_C();

	UFUNCTION(BlueprintCallable)
	bool CheckPlayerState(EViewState ValidCardViewState, EPlayerStateEnum ValidateInteractionState1, EPlayerStateEnum ValidateInteractionState12);

	bool ValidatePlacement_C(ACardPlacement* PlacementTarget, FSCard InCard_Struct);

	bool EnableDragSelection_C();

	UFUNCTION(BlueprintCallable)
	void DetectInteractionOnMove_C();

	void RunCardInteraction_C(ACard3D* Interaction_TalkingCard, ACard3D* Interacition_RecievingCard, ABoardPlayer* InteractionRecievingPlayer);

	bool CreateDragActorVisual_C(bool UseActorLocation);

	void SetupGameUI_C();

	void SetupDeck_C(FString DeckName, TArray<FName> PlayerDeck);

	void SetTimer_C(int32 Time);

	FString LoadClientDeck_C(TArray<FName>& Deck);

	void FilterWeightedCards_C();

	void ShufflePlayerDeck_C(TArray<FName>& TargetArray);

	void AddCardToHand_C(FName CardToAdd_P, ECardSet FromCardSet);

	FName GetCardInHand_C(int32 Index, bool LastIndex);

	int32 CountCardsInHand_C();

	int32 CountCardsInDeck_C();

	bool HasCardInHand_C(FName CardName, int32 Index);

	UFUNCTION()
	void OnRep_bTurnActive_C();

	UFUNCTION()
	void OnRep_TurnState_C();

	void ResetDoOnce();

	UFUNCTION(BlueprintCallable)
	void SetInteractionState_C(EPlayerStateEnum ChangeToState);

	UFUNCTION(BlueprintCallable)
	bool IsTurnActive_C();

	UFUNCTION(BlueprintCallable)
	void CleanCardInteractionState_C();

	UFUNCTION(BlueprintCallable)
	void CleanCardPlacementState();

	bool IsPlatformMobile_C();

	void printaa();
	void printbb();

public:

	UCardGameInstance* CardGameInstance;

	UCountdownWidget* CountdownTimer_Widget;

	TArray<FName> TempDeck;

	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<FName> PlayerDeck;

	UPROPERTY(Replicated)
	TArray<FName> CardsInHand;

	UPROPERTY(BlueprintReadOnly, Replicated)
	ABoardPlayer* BoardPlayer_Ref;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_bTurnActive_C)
	bool bTurnActive;

	UPROPERTY(ReplicatedUsing = OnRep_TurnState_C)
	EGameTurn TurnState = EGameTurn::Waiting;

	UPROPERTY(BlueprintReadOnly)
	UPlayerGameUIWidget* PlayerGameUI_Ref;

	UOpponentUIWidget* OpponentUI_Ref;

	ACardGamePlayerState* PlayerState_Ref;

	bool bShuffleDeck = true;

	bool bEnableWeightedCards = true;

	int32 WeightedFilterIndex = 0;

	TArray<UCardWidget*> CardWidget;

	FName CardToAdd = "";

	ECardSet CardSet_Ref = ECardSet::BasicSet;

	int32 NumberOfCardsToAdd = 0;

	float CardPickupDelay = 0.2;

	UPROPERTY(BlueprintReadWrite)
	FName Temp_CreateCardName;

	UPROPERTY(BlueprintReadWrite)
	ECardSet Temp_ChosenCardSet = ECardSet::BasicSet;

	TSharedPtr<UDragDropOperation> Temp_DragDropOperation;

	bool bTemp_CardIsClone;

	FVector Temp_Location;

	UPROPERTY(BlueprintReadWrite)
	int32 TempHandIndex = 0;

	ACardGameState* GameStateRef;

	UPROPERTY(BlueprintReadWrite)
	EErrors Error_Enum = EErrors::NotaValidPlacement;

	UPROPERTY(BlueprintReadWrite)
	EPlayerStateEnum PlayerStateEnum = EPlayerStateEnum::PendingAction;

	ECardSet CardSet = ECardSet::BasicSet;

	int32 MaxCardsInHand = 7;

	int32 CardsInFirstHand = 5;

	int32 CardsToDrawPerTurn = 1;

	bool bEnabledMobileCardPreview;

	ACard3D* EnabledMobilePreview;

	ACard3D* PlayCard_Server_Ref;

	UPROPERTY(BlueprintReadWrite)
	ACard3D* PlayCard_Client_Ref;

	ACard3D* CardOnBoardRef;

	UPROPERTY(BlueprintReadWrite)
	ACardPlacement* TargetCardPlacement;

	ACardPlacement* CardPlacement_Ref;

	UPROPERTY(BlueprintReadWrite)
	bool bEnableInHandMovementRotation = true;

	UPROPERTY(BlueprintReadWrite)
	bool bIsValidClientDrop;

	bool bPlayCardSuccess;

	bool bIsPlayed;

	UPROPERTY(BlueprintReadWrite)
	float CardHoldDistance = 1700;

	int32 CardInHandIndex_Ref = 0;

	UDragDropOperation* DragDropOperationRef;

	UPROPERTY(Replicated)
	ACard3D* HitCard;

	ABoardPlayer* HitPlayer;

	AActor* HitActor;

	UPROPERTY(BlueprintReadWrite)
	ACard3D* TalkingCard;

	UPROPERTY(BlueprintReadWrite)
	ACard3D* RecievingCard;

	UPROPERTY(BlueprintReadWrite)
	ABoardPlayer* RecievingPlayer;

	ATargetDragSelection* TargetDragSelectionActor;

	UPROPERTY(BlueprintReadWrite)
	bool bIsDragging;

	bool bIsCardSelected;

	bool bValidInteraction;

	bool bSkipManaCheck;

	bool DoOnce = true;

	FTimerHandle DrawCardDelayTimerHandle;

	FTimerHandle BeginPlayDelayTimerHandle;

	FName AddCardName;

protected:

	virtual void BeginPlay() override;

	ACardGamePlayerController();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UUIManagerComponent* UIManager;
};
