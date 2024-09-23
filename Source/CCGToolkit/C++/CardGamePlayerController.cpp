// Fill out your copyright notice in the Description page of Project Settings.


#include "CardGamePlayerController.h"
#include "CardGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/GamePlay/PlayerGameUIWidget.h"
#include "Widgets/GamePlay/OpponentUIWidget.h"
#include "Widgets/GamePlay/CardWidget.h"
#include "TimerManager.h"
#include "FunctionLibraries/Misc_FunctionLibrary.h"
#include "FunctionLibraries/Deck_FunctionLibrary.h"
#include "FunctionLibraries/CardsInHand_FunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GamePlay/CardPlacement.h"
#include "FunctionLibraries/Card_FunctionLibrary.h"
#include "FunctionLibraries/CardInteraction_FunctionLibrary.h"
#include "Enums/CardGameEnums.h"
#include "FunctionLibraries/Math_Library.h"
#include "FunctionLibraries/SaveGame_FunctionLibrary.h"
#include "FunctionLibraries/Log_FunctionLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Kismet/KismetArrayLibrary.h"


ACardGamePlayerController::ACardGamePlayerController()
{
	bReplicates = true;

	UIManager = CreateDefaultSubobject<UUIManagerComponent>(TEXT("UIManager"));
}

void ACardGamePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACardGamePlayerController, PlayerDeck);
	DOREPLIFETIME(ACardGamePlayerController, CardsInHand);
	DOREPLIFETIME(ACardGamePlayerController, BoardPlayer_Ref);
	DOREPLIFETIME(ACardGamePlayerController, bTurnActive);
	DOREPLIFETIME(ACardGamePlayerController, TurnState);
	DOREPLIFETIME(ACardGamePlayerController, HitCard);
}

void ACardGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	CardGameInstance = Cast<UCardGameInstance>(GetWorld()->GetGameInstance());

	CardGameInstance->AssignPlayerController(this);

	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this, nullptr, EMouseLockMode::DoNotLock, false);
	GameStateRef = Cast<ACardGameState>(UGameplayStatics::GetGameState(this));
	PlayerState_Ref = Cast<ACardGamePlayerState>(this->PlayerState);

	//StartCoroutine("CoroBeginPlay", CoroBeginPlay());
}   

WHCoroTask* ACardGamePlayerController::CoroBeginPlay()
{
	WHCORO_PARAM(ACardGamePlayerController)
	ACardGamePlayerController* CardGamePlayerController = nullptr;

#include WHCORO_BEGIN()
	CardGamePlayerController = Cast<ACardGamePlayerController>(UGameplayStatics::GetPlayerController(D->GetWorld(), 0));
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(CardGamePlayerController, nullptr, EMouseLockMode::DoNotLock, false);

#include WHYIELD_READY()
	WHYIELD_RETURN_SECOND(2.0f);
	
	D->GameStateRef = Cast<ACardGameState>(UGameplayStatics::GetGameState(CardGamePlayerController));
	D->PlayerState_Ref = Cast<ACardGamePlayerState>(CardGamePlayerController->PlayerState);

#include WHCORO_END()
}

void ACardGamePlayerController::ClientPostLogin_C_Implementation()
{
	StartCoroutine("CoroClientPostLogin", CoroClientPostLogin());
}

WHCoroTask* ACardGamePlayerController::CoroClientPostLogin()
{
	WHCORO_PARAM(ACardGamePlayerController)
	EGameState CurrentState;
#include WHCORO_BEGIN()
#include WHYIELD_READY()	
	WHYIELD_RETURN_SECOND(1.0f);
	UCardGameInstance* CardGameInstance = D->CardGameInstance;
	
	if (CardGameInstance->GetGameState(EGameState::Playing, CurrentState))
	{
		D->SetupGameUI_C();
		D->UpdateGameUI_C(false);
	}
#include WHCORO_END()
}

void ACardGamePlayerController::SetCountdownTimer_C_Implementation(int32 Time)
{
	if (CountdownTimer_Widget)
	{
		CountdownTimer_Widget->Timer = Time;
		if (CountdownTimer_Widget->Timer <1)
		{
			CountdownTimer_Widget->RemoveFromParent();
		}
	}
	else
	{
		UClass* CountdownTimer_WidgetClass = LoadClass<UCountdownWidget>(NULL, TEXT("WidgetBlueprint'/Game/CCGToolkitC++/Blueprints/Widgets/Additional/CountdownWidget.CountdownWidget_C''"));
		CountdownTimer_Widget = CreateWidget<UCountdownWidget>(this, CountdownTimer_WidgetClass);
		CountdownTimer_Widget->AddToViewport(5);

		CountdownTimer_Widget->Timer = Time;
		if (Time < 1)
		{
			CountdownTimer_Widget->RemoveFromParent();
		}
	}
}

bool ACardGamePlayerController::MatchEnd_C_Implementation(EEndGameResults Result)
{
	FTimerHandle MatchEndTimerHandle;
	if (DoOnce)
	{
		if (bTurnActive)
		{
			bTurnActive = false;
			OnRep_bTurnActive_C();
		}
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FName(TEXT("EndClientMatchState_C")), Result);
		GetWorldTimerManager().SetTimer(MatchEndTimerHandle, TimerDel, 0.5, false);
	}
	return false;
}

void ACardGamePlayerController::CallCreateCard_C(FName CardName, UDragDropOperation* DragDropOperation, ECardSet InCardSet, int32 CardHandleIndex)
{
	if (bTurnActive)
	{
		FTransform SpawnTransform;

		DragDropOperationRef = DragDropOperation;
		Temp_CreateCardName = CardName;
		Temp_ChosenCardSet = InCardSet;
		TempHandIndex = CardHandleIndex;
		SetInteractionState_C(EPlayerStateEnum::PlacingCard);
		FVector SpawnTransformLocation = UMisc_FunctionLibrary::MouseDistanceInWorldSpace(this, 1500, SpawnTransform);
		CreatePlaceableCard_Client_C(Temp_CreateCardName, InCardSet, SpawnTransformLocation);
	}
}

bool ACardGamePlayerController::GetPlayerDeck_C_Implementation(TArray<FName>& Deck)
{
	Deck = PlayerDeck;
	return true;
}

void ACardGamePlayerController::Event_GetPlayerDeck_C_Implementation()
{
	TArray<FName> Deck;

	FString DeckName = LoadClientDeck_C(Deck);
	Server_ReturnPlayerDeck_C(DeckName, Deck);
}

void ACardGamePlayerController::OnDragCanceled_C()
{
	/*IsTurnActive_C();
	switch (PlayerStateEnum)
	{
	case EPlayerStateEnum::PendingAction:
		break;
	case EPlayerStateEnum::CardInteraction:
		if (ValidateInteractionState_C())
		{
			Server_RunCardInteraction_C(TalkingCard, RecievingCard, RecievingPlayer);
			CleanCardInteractionState_C();
			SetInteractionState_C(EPlayerStateEnum::PendingAction);
		}
		else
		{
			if (bIsDragging)
			{
				CleanCardInteractionState_C();
				SetInteractionState_C(EPlayerStateEnum::PendingAction);
			}
		}
		break;
	case EPlayerStateEnum::PlacingCard:
		if (bIsValidClientDrop)
		{
			Server_PlayCard_C(Temp_CreateCardName, Temp_ChosenCardSet, TargetCardPlacement, TempHandIndex, PlayCard_Client_Ref->GetActorTransform());
			SetInteractionState_C(EPlayerStateEnum::PendingAction);
		}
		else
		{
			DropCard_C(false, Error_Enum);
			DestroyClientCard_C();
		}
		break;
	default:
		break;
	}*/
}

void ACardGamePlayerController::DestroyClientCard_C_Implementation()
{
	PlayCard_Client_Ref->Destroy();
	SetInteractionState_C(EPlayerStateEnum::PendingAction);
}

bool ACardGamePlayerController::DrawCard_C_Implementation(FName CardName, bool IgnoreMaxCards, int32 NumberOfCardsToDraw)
{
	if (HasAuthority())
	{
		CardToAdd = CardName;
		if ((UDeck_FunctionLibrary::CountCardsInDeck(PlayerDeck)-NumberOfCardsToAdd) > 0)
		{
			if (IgnoreMaxCards)
			{
				NumberOfCardsToAdd += NumberOfCardsToDraw;
				
				if (!DrawCardDelayTimerHandle.IsValid())
				{
					GetWorldTimerManager().SetTimer(DrawCardDelayTimerHandle, this, &ACardGamePlayerController::AddCardToHand_C, CardPickupDelay, true);
				}
			}
			else
			{
				if (UCardsInHand_FunctionLibrary::CountCardsInHand(CardsInHand) < MaxCardsInHand)
				{
					if ((NumberOfCardsToDraw + NumberOfCardsToAdd + UCardsInHand_FunctionLibrary::CountCardsInHand(CardsInHand)) <= MaxCardsInHand)
					{
						NumberOfCardsToAdd += NumberOfCardsToDraw;

						if (!DrawCardDelayTimerHandle.IsValid())
						{
							GetWorldTimerManager().SetTimer(DrawCardDelayTimerHandle, this, &ACardGamePlayerController::AddCardToHand_C, CardPickupDelay, true);
						}
					}
					else
					{
						NumberOfCardsToAdd += MaxCardsInHand - (UCardsInHand_FunctionLibrary::CountCardsInHand(CardsInHand) + NumberOfCardsToAdd);
						if (!DrawCardDelayTimerHandle.IsValid())
						{
							GetWorldTimerManager().SetTimer(DrawCardDelayTimerHandle, this, &ACardGamePlayerController::AddCardToHand_C, CardPickupDelay, true);
						}
					}
				}
			}
		}
	}
	return true;
}

ACard3D* ACardGamePlayerController::CreatePlayableCard_C_Implementation(FTransform SpawnTransform)
{
	if (HasAuthority())
	{
		UClass* Card3DClass = LoadClass<ACard3D>(NULL, TEXT("Blueprint'/Game/CCGToolkitC++/Blueprints/GamePlay/3DCard.3DCard_C'"));
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.Owner = this;
		PlayCard_Server_Ref = GetWorld()->SpawnActor<ACard3D>(Card3DClass, SpawnTransform, ActorSpawnParameters);
		return PlayCard_Server_Ref;
	}
	return nullptr;
}

bool ACardGamePlayerController::Developer_AddCardToHand_C_Validate(FName CardToAdd_P)
{
	return true;
}

void ACardGamePlayerController::Developer_AddCardToHand_C_Implementation(FName InCardToAdd)
{
	CardToAdd = InCardToAdd;

	FName CardName;
	if (CardToAdd == "None")
	{
		CardName = UDeck_FunctionLibrary::GetCardFromDeck(PlayerDeck, 0, false);
		IController_Interface* Controller_Interface = Cast<IController_Interface>(this);
		Controller_Interface->Execute_AddCardToPlayersHand_C(this, CardName);
		IDeck_Interface* Deck_Interface = Cast<IDeck_Interface>(this);
		Deck_Interface->Execute_RemoveCardFromDeck_C(this, false, 0);
	}
	else
	{
		CardName = CardToAdd;
		IController_Interface* Controller_Interface = Cast<IController_Interface>(this);
		Controller_Interface->Execute_AddCardToPlayersHand_C(this, CardName);
	}

	Server_UpdatePlayerState_C();

	if (NumberOfCardsToAdd <= 0)
	{
		GetWorldTimerManager().ClearTimer(DrawCardDelayTimerHandle);
	}

}

void ACardGamePlayerController::AddCardToHand_C()
{
	if (NumberOfCardsToAdd >= 0)
	{
		NumberOfCardsToAdd--;

		FName CardName;
		if (CardToAdd == "None")
		{
			CardName = UDeck_FunctionLibrary::GetCardFromDeck(PlayerDeck, 0, false);
		}
		else
		{
			CardName = CardToAdd;
		}

		IController_Interface* Controller_Interface = Cast<IController_Interface>(this);
		Controller_Interface->Execute_AddCardToPlayersHand_C(this, CardName);

		if (CardToAdd == "None")
		{
			IDeck_Interface* Deck_Interface = Cast<IDeck_Interface>(this);
			Deck_Interface->Execute_RemoveCardFromDeck_C(this, false, 0);
		}

		Server_UpdatePlayerState_C();

		if (NumberOfCardsToAdd <= 0)
		{
			GetWorldTimerManager().ClearTimer(DrawCardDelayTimerHandle);
			DrawCardDelayTimerHandle.Invalidate();
		}
	}
	else
	{
		GetWorldTimerManager().ClearTimer(DrawCardDelayTimerHandle);
		DrawCardDelayTimerHandle.Invalidate();
	}
}

void ACardGamePlayerController::AddCardToCardManager_C_Implementation(FName CardName, ECardSet FromCardSet)
{
	PlayerGameUI_Ref->CardManagerWidget->AddCardWidget_C(CardName, FromCardSet);
}

void ACardGamePlayerController::RemoveCardFromCardManager_C_Implementation(UCardWidget* CardWidget_P, int32 OR_CardHandIndex, bool OR_RemoveALL)
{
	PlayerGameUI_Ref->CardManagerWidget->RemoveCardWidgetfromHand_C(CardWidget_P, OR_CardHandIndex, OR_RemoveALL);
}

bool ACardGamePlayerController::Server_SetupDeck_C_Validate()
{
	return true;
}

void ACardGamePlayerController::Server_SetupDeck_C_Implementation()
{
	Event_GetPlayerDeck_C();
}

bool ACardGamePlayerController::Server_ReturnPlayerDeck_C_Validate(const FString& DeckName, const TArray<FName>& DeckArray)
{
	return true;
}

void ACardGamePlayerController::Server_ReturnPlayerDeck_C_Implementation(const FString& DeckName, const TArray<FName>& DeckArray)
{
	SetupDeck_C(DeckName, DeckArray);
	Server_UpdatePlayerState_C();
	CreateDisplayMessage_C("Deck Created!", FLinearColor::Green, false, 0.0, true);
}

bool ACardGamePlayerController::MatchBegin_C_Implementation()
{
	if (HasAuthority())
	{
		ICardsInHand_Interface* CardsInHand_Interface = Cast<ICardsInHand_Interface>(this);
		CardsInHand_Interface->Execute_DrawCard_C(this, "None", false, CardsInFirstHand);
		Server_UpdatePlayerHealth_C();
		CreateDisplayMessage_C("Game has Started", FLinearColor::Yellow, false, 0.0, true);
	}
	return true;
}

bool ACardGamePlayerController::AddCardToPlayersHand_C_Implementation(FName CardName)
{
	CardsInHand.Add(CardName);
	AddCardName = UCardsInHand_FunctionLibrary::GetCardInHand(CardsInHand, 0, true);
	AddCardToCardManager_C(AddCardName, ECardSet::Empty);
	Server_UpdatePlayerState_C();
	return true;
}

bool ACardGamePlayerController::ChangeActivePlayerTurn_C_Implementation(bool TurnActive)
{
	if (HasAuthority())
	{
		if (TurnActive)
		{
			TurnState = EGameTurn::TurnActive;
			OnRep_TurnState_C();
			bTurnActive = true;
			OnRep_bTurnActive_C();

			IPlayerState_Interface* PlayerState_Interface = Cast<IPlayerState_Interface>(PlayerState_Ref);
			if (PlayerState_Interface)
			{
				PlayerState_Interface->Execute_GetManaForTurn_C(PlayerState_Ref);
				ICardsInHand_Interface* CardsInHand_Interface = Cast<ICardsInHand_Interface>(this);
				CardsInHand_Interface->Execute_DrawCard_C(this, "None", false, CardsToDrawPerTurn);
				PlayerState_Ref->PlayerTurn++;
				CreateDisplayMessage_C("Your Turn", FLinearColor::Yellow, true, 1.0, true);
				UpdateGameUI_C(false);
			}
		}
		else
		{
			TurnState = EGameTurn::TurnInactive;
			OnRep_TurnState_C();
			bTurnActive = false;
			OnRep_bTurnActive_C();
			CreateDisplayMessage_C("Opponents Turn!", FLinearColor::Red, false, 1.0, true);
			UpdateGameUI_C(false);
		}
	}
	return true;
}

bool ACardGamePlayerController::RemoveCardFromDeck_C_Implementation(bool RemoveAll, int32 IndexToRemove)
{
	UDeck_FunctionLibrary::RemoveCardFromDeck(RemoveAll, IndexToRemove, PlayerDeck);
	Server_UpdatePlayerState_C();
	return true;
}

bool ACardGamePlayerController::RemoveCardFromHand_C_Implementation(FName Card, int32 Index, bool RemoveAll)
{
	if (HasAuthority())
	{
		if (RemoveAll)
		{
			CardsInHand.Empty();
			RemoveCardFromCardManager_C(nullptr, 0, true);
			PlayerState_Ref->ForceUpdateUI_C();
			Server_UpdatePlayerState_C();
			return true;
		}
		else
		{
			if (UCardsInHand_FunctionLibrary::HasCardInHand(CardsInHand, Card, Index))
			{
				CardsInHand.RemoveAt(Index);
				RemoveCardFromCardManager_C(nullptr, Index, false);
				Server_UpdatePlayerState_C();
				return true;
			}
			else
			{
				Error_Enum = EErrors::CardNotFoundInHandServer;
				return false;
			}
		}
	}
	return false;
}

bool ACardGamePlayerController::Server_RequestChangeTurnState_C_Validate()
{
	return true;
}

void ACardGamePlayerController::Server_RequestChangeTurnState_C_Implementation()
{
	ACardGameState* CardGameState = Cast<ACardGameState>(GetWorld()->GetGameState());
	if (CardGameState->RequestChangeTurnState_C(this))
	{
		CardGameState->Server_RequestChangeTurnState_C(this);
	}
}

bool ACardGamePlayerController::Server_UpdatePlayerHealth_C_Validate()
{
	return true;
}

void ACardGamePlayerController::Server_UpdatePlayerHealth_C_Implementation()
{
	if (HasAuthority())
	{
		ACardGamePlayerState* CardGamePlayerState = Cast<ACardGamePlayerState>(PlayerState);
		BoardPlayer_Ref->Update_Health_C(CardGamePlayerState->Health);
	}
}

bool ACardGamePlayerController::Server_UpdatePlayerState_C_Validate()
{
	return true;
}

void ACardGamePlayerController::Server_UpdatePlayerState_C_Implementation()
{
	TArray<ACard3D*> PlayerCards;
	TArray<ACard3D*> OppenedCards;

	if (HasAuthority())
	{
		if (GameStateRef)
		{
			GameStateRef->GetBoardState_C(PlayerState_Ref->CardGamePlayerId, PlayerCards, OppenedCards);
			PlayerState_Ref->UpdatePlayersCardStates_C(CardsInHand.Num(), PlayerDeck.Num(), PlayerCards.Num());
		}
	}
}

bool ACardGamePlayerController::Server_PlayCard_C_Validate(FName CardName, ECardSet CardSet_P, ACardPlacement* GoalPlacement, int32 CardInHandIndex, FTransform SpawnTransform)
{
	return true;
}

void ACardGamePlayerController::Server_PlayCard_C_Implementation(FName CardName, ECardSet InCardSet, ACardPlacement* GoalPlacement, int32 CardInHandIndex, FTransform SpawnTransform)
{
	PlayCard_C(CardName, InCardSet, GoalPlacement, CardInHandIndex, SpawnTransform);
}

bool ACardGamePlayerController::Server_RunCardInteraction_C_Validate(ACard3D* TalkingCard_P, ACard3D* RecievingCard_P, ABoardPlayer* BoardPlayer)
{
	return true;
}

void ACardGamePlayerController::Server_RunCardInteraction_C_Implementation(ACard3D* TalkingCard_P, ACard3D* RecievingCard_P, ABoardPlayer* BoardPlayer)
{
	RunCardInteraction_C(TalkingCard_P, RecievingCard_P, BoardPlayer);
}

bool ACardGamePlayerController::SetSkipManaCheck_C_Validate(bool SkipCheck)
{
	return true;
}

void ACardGamePlayerController::SetSkipManaCheck_C_Implementation(bool SkipCheck)
{
	bSkipManaCheck = SkipCheck;
}

bool ACardGamePlayerController::ReshuffleDeck_C_Validate()
{
	return true;
}

void ACardGamePlayerController::ReshuffleDeck_C_Implementation()
{
	Event_GetPlayerDeck_C();
}

bool ACardGamePlayerController::ClearCardsInHand_C_Validate()
{
	return true;
}

void ACardGamePlayerController::ClearCardsInHand_C_Implementation()
{
	CardsInHand.Empty();
	Server_UpdatePlayerState_C();
}

bool ACardGamePlayerController::SpawnAIOpponent_C_Validate()
{
	return true;
}

void ACardGamePlayerController::SpawnAIOpponent_C_Implementation()
{
	ACardGameMode* CardGameMode = Cast<ACardGameMode>(UGameplayStatics::GetGameMode(this));
	CardGameMode->ForceSpawnAIOpponent_C();
}

bool ACardGamePlayerController::ValidatePlacementOwner(int32 PlayerIndex, int32 PlacementPlayerIndex, FSCard Card_Struct)
{
	if (Card_Struct.Placement_Settings.Owner == EPlayers::SelfAndOpponent)
	{
		return true;
	}
	else
	{
		if (Card_Struct.Placement_Settings.Owner == EPlayers::Self && (PlacementPlayerIndex == 0 || PlacementPlayerIndex == PlayerIndex))
		{
			return true;
		}
		else
		{
			if (Card_Struct.Placement_Settings.Owner == EPlayers::Opponent && (PlacementPlayerIndex == 0 || PlacementPlayerIndex != PlayerIndex))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
}

void ACardGamePlayerController::NotifyCardTurnActive_C()
{
	GameStateRef->BeginPlayerTurn_C(PlayerState_Ref->CardGamePlayerId);
}

void ACardGamePlayerController::NotifyCardsEndTurn_C()
{
	GameStateRef->EndOfPlayerTurn_C(PlayerState_Ref->CardGamePlayerId);
}

void ACardGamePlayerController::SetCardLocation_C(FVector HoldLocation, ACard3D* Card, FRotator Rotation)
{
	if (Card)
	{
		int32 DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);
		FVector NewLoacation = UKismetMathLibrary::VInterpTo(Card->GetActorLocation(), HoldLocation, DeltaTime, 10.0);
		Card->SetActorLocation(NewLoacation,false,false);
		if (bEnableInHandMovementRotation)
		{
			FRotator ReturnValue;
			if (Rotation.Yaw > 0)
			{
				ReturnValue.Roll = (Card->GetActorLocation().Y - HoldLocation.Y) / 5 * -1;
				ReturnValue.Pitch = (Card->GetActorLocation().X - HoldLocation.X) / 5;
			}
			else
			{
				ReturnValue.Roll = (Card->GetActorLocation().Y - HoldLocation.Y) / 5;
				ReturnValue.Pitch = (Card->GetActorLocation().X - HoldLocation.X) / 5 * -1.0;
			}
			ReturnValue.Yaw = Rotation.Yaw;

			FRotator NewRotator = UKismetMathLibrary::RInterpTo(Card->GetActorRotation(), ReturnValue, DeltaTime, 5.0);
			Card->SetActorRotation(NewRotator, TeleportFlagToEnum(false));
		}
	}
}

bool ACardGamePlayerController::ValidateCardPlacement_C(AActor* InHitActor)
{
	HitActor = InHitActor;
	ACardPlacement* CardPlacement = Cast<ACardPlacement>(HitActor);
	if (CardPlacement)
	{
		if (TargetCardPlacement == CardPlacement)
		{
			return bIsValidClientDrop;
		}
		else   
		{
			FSCard CardStruct;
			CardPlacement_Ref = CardPlacement;
			ACardGamePlayerState* CardGamePlayerState = Cast<ACardGamePlayerState>(this->PlayerState);
			if (CardGamePlayerState)
			{
				UDeck_FunctionLibrary::GetCardData(Temp_CreateCardName, Temp_ChosenCardSet, CardStruct);
				if(ValidatePlacement_C(CardPlacement_Ref, CardStruct))
				{
					if(ValidatePlacementOwner(CardGamePlayerState->CardGamePlayerId, CardPlacement_Ref->PlayerIndex, CardStruct))
					{
						if (TargetCardPlacement)
						{
							if (TargetCardPlacement == CardPlacement_Ref)
							{
								bIsValidClientDrop = true;
								return true;
							}
							else
							{
								TargetCardPlacement = CardPlacement_Ref;
								bIsValidClientDrop = true;
								return true;
							}
						}
						else
						{
							TargetCardPlacement = CardPlacement_Ref;
							bIsValidClientDrop = true;
							return true;
						}
					}
					else
					{
						Error_Enum = EErrors::IncorrectPlayerPlacementfoCard;
						bIsValidClientDrop = false;
						return false;
					}
				}
				return false;
			}
			else
			{
				bIsValidClientDrop = false;
				return bIsValidClientDrop;
			}
		}
	}
	else
	{
		ACard3D* Card3D = Cast<ACard3D>(HitActor);
		if (Card3D)
		{
			if (UKismetMathLibrary::EqualEqual_ObjectObject(Card3D, TargetCardPlacement))
			{
				return bIsValidClientDrop;
			}
			else
			{
				CardOnBoardRef = Card3D;
				CardPlacement_Ref = Card3D->PlacementOwner;
				ACardGamePlayerState* CardGamePlayerState = Cast<ACardGamePlayerState>(this->PlayerState);
				if (CardGamePlayerState)
				{
					FSCard CardStruct;
					UDeck_FunctionLibrary::GetCardData(Temp_CreateCardName, Temp_ChosenCardSet, CardStruct);
					if (ValidatePlacement_C(CardPlacement_Ref, CardStruct))
					{
						if (ValidatePlacementOwner(CardGamePlayerState->CardGamePlayerId, CardPlacement_Ref->PlayerIndex, CardStruct))
						{
							if (TargetCardPlacement)
							{
								if (TargetCardPlacement == CardPlacement_Ref)
								{
									bIsValidClientDrop = true;
									return bIsValidClientDrop;
								}
								else
								{
									CardPlacement_Ref = TargetCardPlacement;
									bIsValidClientDrop = true;
									return bIsValidClientDrop;
								}
							}
							else
							{
								CardPlacement_Ref = TargetCardPlacement;
								bIsValidClientDrop = true;
								return bIsValidClientDrop;
							}
						}
						else
						{
							Error_Enum = EErrors::IncorrectPlayerPlacementfoCard;
							bIsValidClientDrop = false;
							return bIsValidClientDrop;
						}
					}
					return bIsValidClientDrop;
				}
				return bIsValidClientDrop;
			}
		}
		else
		{
			Error_Enum = EErrors::NoSupportedDropLocationFound;
			bIsValidClientDrop = false;
			return bIsValidClientDrop;
		}
	}
}

ACard3D* ACardGamePlayerController::CreatePlaceableCard_Client_C(FName Name, ECardSet InCardSet, FVector SpawnTransformLocation)
{
	FRotator ReturnRotation;
	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	SpawnParameters.Owner = this;
	UClass* Card3DClass = LoadClass<ACard3D>(NULL, TEXT("Blueprint'/Game/CCGToolkitC++/Blueprints/GamePlay/3DCard.3DCard_C'"));

	UMisc_FunctionLibrary::GetWorldRotationForPlayer(FRotator(0, 0, 0), ReturnRotation);
	PlayCard_Client_Ref = GetWorld()->SpawnActor<ACard3D>(Card3DClass, SpawnTransformLocation, ReturnRotation, SpawnParameters);
	PlayCard_Client_Ref->SetClientSideData_C(Name, InCardSet);
	return PlayCard_Client_Ref;
}

ACard3D* ACardGamePlayerController::CreatePlaceableCard_Server_C(FTransform SpawnTransform)
{
	if (HasAuthority())
	{
		UClass* Card3DClass = LoadClass<ACard3D>(NULL, TEXT("Blueprint'/Game/CCGToolkitC++/Blueprints/GamePlay/3DCard.3DCard_C'"));
		FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
		SpawnParameters.Owner = this;
		PlayCard_Server_Ref = GetWorld()->SpawnActor<ACard3D>(Card3DClass, SpawnTransform, SpawnParameters);
		return PlayCard_Server_Ref;
	}
	return nullptr;
}

bool ACardGamePlayerController::ServerValidateCardPlacement_C(ACardPlacement* CardPlacement, FSCard CardStruct, ACardPlacement*& ValidCardPlacement)
{
	if (HasAuthority())
	{
		ACardGamePlayerState* CardGamePlayerState = Cast<ACardGamePlayerState>(PlayerState);
		if (CardGamePlayerState != nullptr)
		{
			if (ValidatePlacementOwner(CardGamePlayerState->CardGamePlayerId, CardPlacement->PlayerIndex, CardStruct))
			{
				FPlacementState PlacementState = CardPlacement->GetPlacementState_C();
				if (PlacementState.ValidCardTypesArray.Find(CardStruct.Type) >= 0)
				{
					if (PlacementState.InIsFull)
					{
						Error_Enum = EErrors::PlacementFull;
						Error_Enum = EErrors::InvalidPlacement;
						return false;
					}
					else
					{
						return true;
					}
				}
				else
				{
					Error_Enum = EErrors::PlacementDoesNotSupportCardType;
					Error_Enum = EErrors::InvalidPlacement;
					return false;
				}
			}
		}
		else
		{
			Error_Enum = EErrors::CastFailedtoPlayer;
			return false;
		}
	}
	return false;
}

ACard3D* ACardGamePlayerController::SetCustomCardData_C(ACard3D* Card, bool ActiveAbility)
{
	Card->IsAbilityActive = ActiveAbility;
	return Card;
}

void ACardGamePlayerController::PlayCard_C(FName CardName, ECardSet InCardSet, ACardPlacement* CardPlacement, int32 CardHandIndex, FTransform SpawnTransform)
{
	if (HasAuthority()) 
	{
		FSCard CardStruct;
		UDeck_FunctionLibrary::GetCardData(CardName, InCardSet, CardStruct);
		if (UCardsInHand_FunctionLibrary::HasCardInHand(CardsInHand, CardName, CardHandIndex))
		{
			ACardPlacement* ValidCardPlacement;
			if (ServerValidateCardPlacement_C(CardPlacement, CardStruct, ValidCardPlacement))
			{
				ECardPlayConditions ConditionFailure = ECardPlayConditions::None;
				if (UCard_FunctionLibrary::ValidateCardPlayConditions(CardName, this, ConditionFailure))
				{
					int32 Cost;
					if (UCard_FunctionLibrary::ChargeCardManaCost(PlayerState_Ref->CardGamePlayerId, bSkipManaCheck, CardStruct, EManaAccount::Placement, Cost))
					{
						ICardsInHand_Interface* CardsInHand_Interface = Cast<ICardsInHand_Interface>(this);
						bool Success = CardsInHand_Interface->Execute_RemoveCardFromHand_C(this, CardName, CardHandIndex, false);
						if (Success)
						{
							ACard3D* Card3DRef = CreatePlaceableCard_Server_C(SpawnTransform);
							ACard3D* AddCard = UCard_FunctionLibrary::SetupCard(Card3DRef, PlayerState_Ref->CardGamePlayerId, CardName, CardSet, CardStruct, false);
							UCard_FunctionLibrary::AddCardToBoardPlacement(AddCard, CardPlacement, PlayerState_Ref->CardGamePlayerId);
							GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Blue, "Player:" + FString::FromInt(PlayerState_Ref->CardGamePlayerId) + "Played Card:" + CardName.ToString());
							DestroyClientCard_C();
							DropCard_C(true, Error_Enum);
							Server_UpdatePlayerState_C();
						}
						else
						{
							DestroyClientCard_C();
							DropCard_C(false, Error_Enum);
							Server_UpdatePlayerState_C();
						}
					}
					else
					{
						LogNotificationMessage_C("Not Enough Mana", FColor::Red);
						DestroyClientCard_C();
						DropCard_C(false, Error_Enum);
						Server_UpdatePlayerState_C();
					}
				}
				else
				{
					LogNotificationMessage_C(UMath_Library::GetEnumValueAsString("ECardPlayConditions", ConditionFailure), FColor::Yellow);
					DestroyClientCard_C();
					DropCard_C(false, Error_Enum);
					Server_UpdatePlayerState_C();
				}
			}
			else
			{
				LogNotificationMessage_C("Placement Not Valid", FColor::Red);
				DestroyClientCard_C();
				DropCard_C(false, Error_Enum);
				Server_UpdatePlayerState_C();
			}
		}
	}
}

void ACardGamePlayerController::DetectCardInteraction_C()
{
	AActor* InHitActor;
	UMisc_FunctionLibrary::MouseToWorldLocation(this, InHitActor);

	if (Cast<ACard3D>(InHitActor))
	{
		HitCard = Cast<ACard3D>(InHitActor);
		if (HitCard->OwningPlayerID == PlayerState_Ref->CardGamePlayerId)
		{
			if (bIsCardSelected)
			{
				if (TalkingCard)
				{
					if (HitCard == TalkingCard)
					{
						EPlatform ReturnPlatform;
						if (CardGameInstance && CardGameInstance->GetCurrentPlatform_C(ReturnPlatform))
						{
							if (HitCard->PreviewEnabled)
							{
								HitCard->DisableMobileCardPreview_C();
								CleanCardInteractionState_C();
								SetInteractionState_C(EPlayerStateEnum::PendingAction);
							}
							else
							{
								HitCard->EnableMobileCardPreview_C();
								EnabledMobilePreview = HitCard;
							}
						}
						else
						{
							CleanCardInteractionState_C();
							SetInteractionState_C(EPlayerStateEnum::PendingAction);
						}
					}
					else
					{
						if (TalkingCard->PreviewEnabled)
						{
							HitCard->DisableMobileCardPreview_C();
							CleanCardInteractionState_C();
							SetInteractionState_C(EPlayerStateEnum::PendingAction);
						}
						else
						{
							if (TalkingCard->CanAttackCards)
							{
								RecievingCard = HitCard;
								RecievingCard->Event_Selected_C(PlayerState_Ref->CardGamePlayerId);
							}
							else
							{
								CleanCardInteractionState_C();
								SetInteractionState_C(EPlayerStateEnum::PendingAction);
							}
						}
					}
				}
				else
				{
					TalkingCard = HitCard;
					TalkingCard->Event_Selected_C(PlayerState_Ref->CardGamePlayerId);
				}
			}
			else
			{
				bIsCardSelected = true;
				SetInteractionState_C(EPlayerStateEnum::CardInteraction);
				TalkingCard = HitCard;
				TalkingCard->Event_Selected_C(PlayerState_Ref->CardGamePlayerId);
			}
		}
		else
		{
			if (TalkingCard)
			{
				if (HitCard == TalkingCard)
				{
					EPlatform ReturnPlatform;
					if (CardGameInstance && CardGameInstance->GetCurrentPlatform_C(ReturnPlatform))
					{
						if (HitCard->PreviewEnabled)
						{
							HitCard->DisableMobileCardPreview_C();
							CleanCardInteractionState_C();
							SetInteractionState_C(EPlayerStateEnum::PendingAction);
						}
						else
						{
							HitCard->EnableMobileCardPreview_C();
							EnabledMobilePreview = HitCard;
						}
					}
					else
					{
						CleanCardInteractionState_C();
						SetInteractionState_C(EPlayerStateEnum::PendingAction);
					}
				}
				else
				{
					if (TalkingCard->PreviewEnabled)
					{
						HitCard->DisableMobileCardPreview_C();
						CleanCardInteractionState_C();
						SetInteractionState_C(EPlayerStateEnum::PendingAction);
					}
					else
					{
						if (TalkingCard->CanAttackCards)
						{
							RecievingCard = HitCard;
							RecievingCard->Event_Selected_C(PlayerState_Ref->CardGamePlayerId);
						}
						else
						{
							CleanCardInteractionState_C();
							SetInteractionState_C(EPlayerStateEnum::PendingAction);
						}
					}
				}
			}
			else
			{
				EPlatform ReturnPlatform;
				if (CardGameInstance && CardGameInstance->GetCurrentPlatform_C(ReturnPlatform))
				{
					if (HitCard->PreviewEnabled)
					{
						HitCard->DisableMobileCardPreview_C();
						CleanCardInteractionState_C();
						SetInteractionState_C(EPlayerStateEnum::PendingAction);
					}
					else
					{
						HitCard->EnableMobileCardPreview_C();
						EnabledMobilePreview = HitCard;
					}
				}
				else
				{
					CleanCardInteractionState_C();
					SetInteractionState_C(EPlayerStateEnum::PendingAction);
				}
			}
		}
	}
	else
	{
		CleanCardInteractionState_C();
		SetInteractionState_C(EPlayerStateEnum::PendingAction);
	}
}

bool ACardGamePlayerController::ValidateInteractionState_C()
{
	bValidInteraction = false;
	if (bIsCardSelected)
	{
		if (RecievingPlayer || ((TalkingCard && RecievingCard)) && (TalkingCard != RecievingCard))
		{
			bValidInteraction = true;
			return bValidInteraction;
		}
		else
		{
			return bValidInteraction;
		}
	}
	return bValidInteraction;
}

bool ACardGamePlayerController::CheckPlayerState(EViewState ValidCardViewState, EPlayerStateEnum ValidateInteractionState1, EPlayerStateEnum ValidateInteractionState12)
{
	if (PlayerGameUI_Ref->CardManagerWidget->CardViewState == ValidCardViewState && (PlayerStateEnum == ValidateInteractionState1 || PlayerStateEnum == ValidateInteractionState12))
		return true;
	return false;
}

bool ACardGamePlayerController::ValidatePlacement_C(ACardPlacement* PlacementTarget, FSCard InCard_Struct)
{
	FPlacementState PlacementState = PlacementTarget->GetPlacementState_C();
	if (PlacementState.ValidCardTypesArray.Find(InCard_Struct.Type) >= 0)
	{
		if (PlacementState.InIsFull)
		{
			Error_Enum = EErrors::PlacementFull;
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		Error_Enum = EErrors::PlacementDoesNotSupportCardType;
		return false;
	}
}

bool ACardGamePlayerController::EnableDragSelection_C()
{
	if (bIsDragging)
	{
		return true;
	}
	else
	{
		CleanCardInteractionState_C();
		DetectCardInteraction_C();
		bIsDragging = true;
		return false;
	}
}

void ACardGamePlayerController::DetectInteractionOnMove_C()
{	
	EnableDragSelection_C();

	AActor* InHitActor;
	FVector Location = UMisc_FunctionLibrary::MouseToWorldLocation(this, InHitActor);
	ACard3D* Card3D = Cast<ACard3D>(InHitActor);
	if (Card3D)
	{
		HitCard = Card3D; 
		if (TalkingCard->CanAttackCards)
		{
			if (HitCard != RecievingCard)
			{
				if (RecievingCard)
				{
					RecievingCard->Event_Deselected_C();					
				}
				RecievingCard = nullptr;
				if (HitCard != TalkingCard)
				{
					if (TalkingCard)
					{
						RecievingCard = HitCard;
						RecievingPlayer = nullptr;
						RecievingCard->Event_Selected_C(PlayerState_Ref->CardGamePlayerId);
					}
				}
			}
		}
		if (CreateDragActorVisual_C(true))
		{
			TargetDragSelectionActor->SetSelectionProperties(Location, RecievingCard || RecievingPlayer);
		}
	}
	else
	{
		ABoardPlayer* BoardPlayer = Cast<ABoardPlayer>(InHitActor);
		if (BoardPlayer)
		{
			HitPlayer = BoardPlayer;
			if (TalkingCard->CanAttackPlayer)
			{
				RecievingPlayer = BoardPlayer;
			}
			if (RecievingCard)
			{
				RecievingCard->Event_Deselected_C();
				RecievingCard = nullptr;
			}
			if (CreateDragActorVisual_C(true))
			{
				TargetDragSelectionActor->SetSelectionProperties(Location, RecievingCard || RecievingPlayer);
			}
		}
		else
		{
			if (RecievingCard)
			{
				RecievingCard->Event_Deselected_C();
				RecievingCard = nullptr;
				RecievingPlayer = nullptr;
			}
			if (CreateDragActorVisual_C(true))
			{
				TargetDragSelectionActor->SetSelectionProperties(Location, RecievingCard || RecievingPlayer);
			}
		}
	}
}

void ACardGamePlayerController::RunCardInteraction_C(ACard3D* Interaction_TalkingCard, ACard3D* Interacition_RecievingCard, ABoardPlayer* Interaction_RecievingPlayer)
{
	ACard3D* InteracitionRecievingCard;
	ACard3D* InteractionTalkingCard;
	ABoardPlayer* InteractionRecievingPlayer;
	if (HasAuthority())
	{
		InteractionTalkingCard = Interaction_TalkingCard;
		InteracitionRecievingCard = Interacition_RecievingCard;
		InteractionRecievingPlayer = Interaction_RecievingPlayer;

		int32 Cost;
		bool ManaCostValid = UCard_FunctionLibrary::ChargeCardManaCost(PlayerState_Ref->CardGamePlayerId, bSkipManaCheck, Interaction_TalkingCard->CardDataStruct, EManaAccount::Attack, Cost);
		if (ManaCostValid)
		{
			if (InteractionRecievingPlayer)
			{
				EInteractionConditions InteractionConditionFailure;
				bool Success = UCardInteraction_FunctionLibrary::DealDamageToPlayer(false, InteractionTalkingCard, InteractionRecievingPlayer, 0, InteractionConditionFailure);
				if (!Success)
				{
					UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EInteractionConditions"), true);
					FString InteractionConditionFailureStr(EnumPtr->GetNameByValue((int)InteractionConditionFailure).ToString());
					LogNotificationMessage_C(InteractionConditionFailureStr, FLinearColor::Yellow);
				}
			}
			else
			{
				EInteractionConditions CallingCardConditions;
				EInteractionConditions TalkingCardConditions;
				bool Success = UCardInteraction_FunctionLibrary::DealDamageToCard(false, InteractionTalkingCard, InteracitionRecievingCard, 0, CallingCardConditions, TalkingCardConditions);
				/*if (!Success)
				{
					if (CallingCardConditions != EInteractionConditions::None)
					{
						FString Message = "CallingCard:" + UMath_Library::GetEnumValueAsString("EInteractionConditions", CallingCardConditions);
						LogNotificationMessage_C_Implementation(Message, FLinearColor::Yellow);
					}
					if (TalkingCardConditions != EInteractionConditions::None)
					{
						FString Message = "TalkingCard:"+ UMath_Library::GetEnumValueAsString("EInteractionConditions", TalkingCardConditions);
					}
				}*/
			}
		}
		else
		{
			LogNotificationMessage_C("Not Enough Mana to perform Interaction", FLinearColor::Red);
		}
	}
}

bool ACardGamePlayerController::CreateDragActorVisual_C(bool UseActorLocation)
{
	AActor* HitActorRef;
	FVector ReturnValue;
	if (TargetDragSelectionActor)
	{
		if (TalkingCard)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (TalkingCard)
		{
			if (!UseActorLocation)
			{
				ReturnValue = UMisc_FunctionLibrary::MouseToWorldLocation(this, HitActorRef);
			}
			else
			{
				ReturnValue = TalkingCard->GetActorLocation();
			}
			FTransform SpawnTransform = FTransform(ReturnValue);

			UClass* TargetDragSelectionClass = LoadClass<ATargetDragSelection>(NULL, TEXT("Blueprint'/Game/CCGToolkitC++/Blueprints/GamePlay/TargetDragSelection.TargetDragSelection_C'"));
			TargetDragSelectionActor = GetWorld()->SpawnActor<ATargetDragSelection>(TargetDragSelectionClass, SpawnTransform);

			return true;
		}
		else
		{
			return false;
		}
	}
}

void ACardGamePlayerController::SetupGameUI_C()
{
	if (this->IsLocalController())
	{
		UClass* PlayerGameUIWidgetClass = LoadClass<UPlayerGameUIWidget>(NULL, TEXT("WidgetBlueprint'/Game/CCGToolkitC++/Blueprints/Widgets/Gameplay/BP_PlayerGameUIWidget.BP_PlayerGameUIWidget_C'"));
		UClass* OpponentUIWidgetclass = LoadClass<UOpponentUIWidget>(NULL, TEXT("WidgetBlueprint'/Game/CCGToolkitC++/Blueprints/Widgets/Gameplay/BP_OpponentUIWidget.BP_OpponentUIWidget_C'"));
		PlayerGameUI_Ref = CreateWidget<UPlayerGameUIWidget>(this, PlayerGameUIWidgetClass);
		OpponentUI_Ref = CreateWidget<UOpponentUIWidget>(this, OpponentUIWidgetclass);
		PlayerGameUI_Ref->AddToViewport(2);
		OpponentUI_Ref->AddToViewport(1);
	}
}

void ACardGamePlayerController::SetupDeck_C(FString DeckName, TArray<FName> InPlayerDeck)
{
	if (HasAuthority())
	{
		//UKismetSystemLibrary::PrintString(this, TEXT("Player:" + UGameplayStatics::GetPlatformName().Append("   Chosen Deck: ").Append(DeckName), false, true,FLinearColor::Blue, 2.0));
		TempDeck = InPlayerDeck;

		if (bShuffleDeck)
		{
			ShufflePlayerDeck_C(TempDeck);
		}
		if (bEnableWeightedCards)
		{
			PlayerDeck = UDeck_FunctionLibrary::FilterWeightedCardsInDeck(TempDeck);
		}
		else
		{
			PlayerDeck = TempDeck;
		}
		ACardGamePlayerState* CardGamePlayerState = Cast<ACardGamePlayerState>(PlayerState);
		if (CardGamePlayerState)
		{
			MaxCardsInHand = PlayerDeck.Num();
		}
	}
}

void ACardGamePlayerController::SetTimer_C(int32 Time)
{
	if (!CountdownTimer_Widget)
	{
		UClass* CountdownTimer_WidgetClass = LoadClass<UCountdownWidget>(NULL, TEXT("WidgetBlueprint'/Game/CCGToolkit/Blueprints/Widgets/Additional/Countdown.Countdown_C'"));
		CountdownTimer_Widget = CreateWidget<UCountdownWidget>(GetWorld(), CountdownTimer_WidgetClass);
		CountdownTimer_Widget->AddToViewport(5);
	}
	CountdownTimer_Widget->Timer = Time;
	if (CountdownTimer_Widget->Timer < 1)
	{
		CountdownTimer_Widget->RemoveFromParent();
	}
}

FString ACardGamePlayerController::LoadClientDeck_C(TArray<FName>& Deck)
{
	TArray<FName> CustomDeck;
	bool DeckEditable;
	TArray<FName> MyOutRowNames = {""};
	UDataTable* PreBuildDecks_C_DataTable = LoadObject<UDataTable>(NULL, TEXT("DataTable'/Game/CCGToolkitC++/CardGame/DataTable/PreBuildDecks_C_DataTable.PreBuildDecks_C_DataTable'"));

	UCardGameInstance* TempCardGameInstance= Cast<UCardGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (TempCardGameInstance)
	{
		bool DeckValid = USaveGame_FunctionLibrary::LoadCustomDeck(TempCardGameInstance->SelectedCardSet, CustomDeck, DeckEditable);
		if (DeckValid)
		{
			Deck = CustomDeck;
			return TempCardGameInstance->SelectedCardSet;
		}
		else
		{
			UDataTableFunctionLibrary::GetDataTableRowNames(PreBuildDecks_C_DataTable, MyOutRowNames);
			int32 RandomDeckID = UKismetMathLibrary::RandomIntegerInRange(0, MyOutRowNames.Num()-1);
			DeckValid = USaveGame_FunctionLibrary::LoadCustomDeck(MyOutRowNames[RandomDeckID].ToString(), CustomDeck, DeckEditable);
			if (DeckValid)
			{
				Deck = CustomDeck;
				return CardGameInstance->SelectedCardSet;
			}
			else
			{
				CreateDisplayMessage_C("Unable to load a valid deck", FLinearColor::Red, true, 0, true);
				Deck = {"None"};
				FString DeckName = "INVALID";
				return DeckName;
			}
		}
	}
	else
	{
		Deck = { "None" };
		FString DeckName = "INVALID";
		return DeckName;
	}
}

void ACardGamePlayerController::FilterWeightedCards_C()
{
	FSCard CardStruct;
	int32 ReturnValue;

	PlayerDeck.Empty();
	PlayerDeck.SetNum(TempDeck.Num());
	for (int32 i = 0; i<TempDeck.Num(); i++)
	{
		UDeck_FunctionLibrary::GetCardData(TempDeck[i], CardSet, CardStruct);
		if (CardStruct.DeckSettings.Weight >= 2)
		{
			if ((CardStruct.DeckSettings.Weight + i) > (PlayerDeck.Num()-1))
			{
				ReturnValue = i;
			}
			else
			{
				ReturnValue = CardStruct.DeckSettings.Weight + i;
			}
			WeightedFilterIndex = ReturnValue;
			if (PlayerDeck[WeightedFilterIndex] == "None")
			{
				UMath_Library::SetArrayElement(TempDeck[i], PlayerDeck, WeightedFilterIndex);
			}
			while(PlayerDeck[WeightedFilterIndex] != "None")
			{
				if ((WeightedFilterIndex + 1) > (PlayerDeck.Num()-1))
				{
					WeightedFilterIndex = 1;
				}
				else
				{
					WeightedFilterIndex++;
				}
			}
		}
	}
	UKismetSystemLibrary::PrintString(this, TEXT("______________________________Weighted Cards_________________________________"));
	UKismetSystemLibrary::PrintString(this, TEXT("______________________________Remove Player Deck Cards From Temp_________________________________"));
	for (int32 i = 0; i < PlayerDeck.Num(); i++)
	{
		TempDeck.Remove(PlayerDeck[i]);
	}
	UKismetSystemLibrary::PrintString(this, TEXT("______________________________Fill In Empty Deck Places_________________________________"));
	for (int32 i = 0; i< PlayerDeck.Num(); i++)
	{
		if (PlayerDeck[i] == "None")
		{
			if (TempDeck.Num() > 0)
			{
				UMath_Library::SetArrayElement(TempDeck[0], PlayerDeck, i);
				TempDeck.RemoveAt(0);
			}
		}
	}
	UKismetSystemLibrary::PrintString(this, "______________________________These are the Cards_________________________________");
	for (int32 i = 0; i < PlayerDeck.Num(); i++)
	{
		UKismetSystemLibrary::PrintString(this, FString::FromInt(i).Append(":").Append(PlayerDeck[i].ToString()), false, true);
	}
}

void ACardGamePlayerController::ShufflePlayerDeck_C(TArray<FName>& TargetArray)
{
	UMath_Library::ShuffleArray(TargetArray);
}

void ACardGamePlayerController::AddCardToHand_C(FName CardToAdd_P, ECardSet FromCardSet)
{
	CardsInHand.Add(CardToAdd_P);
	AddCardToCardManager_C(UCardsInHand_FunctionLibrary::GetCardInHand(CardsInHand, 0, true), FromCardSet);
}

bool ACardGamePlayerController::RemoveCardFromHandOLD_C(bool ClearAll, FName CardName, int32 IndexToRemove)
{
	if (HasAuthority())
	{
		if (ClearAll)
		{
			CardsInHand.Empty();
			return true;
		}
		else
		{
			if (UCardsInHand_FunctionLibrary::HasCardInHand(CardsInHand, CardName, IndexToRemove))
			{
				CardsInHand.RemoveAt(IndexToRemove);
				return true;
			}
			else
			{
				Error_Enum = EErrors::CardNotFoundInHandServer;
				return false;
			}
		}
	}
	else
	{
		return false;
	}
}

FName ACardGamePlayerController::GetCardInHand_C(int32 Index, bool LastIndex)
{
	int32 ReturnValue;
	if (LastIndex)
	{
		ReturnValue = CardsInHand.Num()-1;
	}
	else
	{
		ReturnValue = Index;
	}
	return CardsInHand[ReturnValue];
}

int32 ACardGamePlayerController::CountCardsInHand_C()
{
	return UCardsInHand_FunctionLibrary::CountCardsInHand(CardsInHand);
}

int32 ACardGamePlayerController::CountCardsInDeck_C()
{
	return UDeck_FunctionLibrary::CountCardsInDeck(PlayerDeck);
}

bool ACardGamePlayerController::HasCardInHand_C(FName CardName, int32 Index)
{
	return CardName == CardsInHand[Index];
}

void ACardGamePlayerController::OnRep_bTurnActive_C()
{
	UpdateGameUI_C(false);
}

void ACardGamePlayerController::OnRep_TurnState_C()
{
	UpdateGameUI_C(false);
}

void ACardGamePlayerController::ResetDoOnce()
{
	DoOnce = true;
}

bool ACardGamePlayerController::IsTurnActive_C()
{
	return bTurnActive;
}

void ACardGamePlayerController::CleanCardInteractionState_C()
{
	if (IsPlatformMobile_C() && EnabledMobilePreview)
		EnabledMobilePreview->DisableMobileCardPreview_C();

	if (TargetDragSelectionActor)
		TargetDragSelectionActor->Destroy();

	if (RecievingCard)
		RecievingCard->Event_Deselected_C();

	if (TalkingCard)
		TalkingCard->Event_Deselected_C();

	TargetDragSelectionActor = nullptr;
	RecievingCard = nullptr;
	TalkingCard = nullptr;
	HitCard = nullptr;
	RecievingPlayer = nullptr;
	bIsDragging = false;
	bIsCardSelected = false;
}

void ACardGamePlayerController::CleanCardPlacementState()
{
	if (PlayCard_Client_Ref)
	{
		DestroyClientCard_C();
	}
}

bool ACardGamePlayerController::IsPlatformMobile_C()
{
	EPlatform ReturnPlatform;

	return CardGameInstance->GetCurrentPlatform_C(ReturnPlatform);
}

void ACardGamePlayerController::SetInteractionState_C(EPlayerStateEnum ChangeToState)
{
	if (PlayerStateEnum != ChangeToState)
		PlayerStateEnum = ChangeToState;
}

void ACardGamePlayerController::LogNotificationMessage_C_Implementation(const FString& Message, FLinearColor Color)
{
	PlayerGameUI_Ref->BP_NotificationManagerWidget->LogNotificationMessage_C(Message, Color);
}

void ACardGamePlayerController::CreateDisplayMessage_C_Implementation(const FString& Message, FLinearColor SpecifiedColor, bool ToScreen, float ScreenDuration, bool ToMessageLogger)
{
	if (ToScreen)
	{
		UMisc_FunctionLibrary::CreateScreenDisplayMessage(Message, SpecifiedColor, ScreenDuration);
	}
	if (ToMessageLogger)
	{
		ULog_FunctionLibrary::AddMessageToLog(this, Message, SpecifiedColor);
	}
}

void ACardGamePlayerController::DropCard_C_Implementation(bool CardPlayed, EErrors Errors)
{
	bIsPlayed = CardPlayed;
	if (bIsPlayed)
	{
		PlayerGameUI_Ref->CardManagerWidget->SetCardViewState_C(EViewState::Default, false);
		UpdateGameUI_C(false);
	}
	else
	{
		CreateDisplayMessage_C("DEBUG: "+ UMath_Library::GetEnumValueAsString("EErrors", Errors), FLinearColor::White, false, 0.0, true);
		UCardWidget* TempCardWidget = Cast<UCardWidget>(DragDropOperationRef->Payload);
		if (TempCardWidget)
		{
			TempCardWidget->SetDefaults();
		}
		PlayerGameUI_Ref->CardManagerWidget->SetCardViewState_C(EViewState::Default, false);
	}
}

void ACardGamePlayerController::UpdateGameUI_C_Implementation(bool ForceCleanUpdate)
{
	if (PlayerGameUI_Ref)
	{
		Server_UpdatePlayerHealth_C();

		IPlayerUI_Interface* OpponentUI_Interface = Cast<IPlayerUI_Interface>(OpponentUI_Ref);
		IPlayerUI_Interface* PlayerGameUI_Interface = Cast<IPlayerUI_Interface>(PlayerGameUI_Ref);

		OpponentUI_Interface->Execute_UpdateUIPlayerStats_C(OpponentUI_Ref,ForceCleanUpdate);
		PlayerGameUI_Interface->Execute_UpdateUIPlayerStats_C(PlayerGameUI_Ref, false);
		PlayerGameUI_Interface->Execute_UpdateUITurnState_C(PlayerGameUI_Ref,bTurnActive, TurnState);	
	}
}

void ACardGamePlayerController::EndClientMatchState_C_Implementation(EEndGameResults NewParam)
{
	FTimerHandle EndClientMatchStateTimerHandle;
	FTimerDelegate TimerDel;
	PlayerGameUI_Ref->CardManagerWidget->SetCardViewState_C(EViewState::HideHand, true);
	OpponentUI_Ref->SetCardViewState_C(EViewState::HideHand, true);

	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EEndGameResults"), true);
	FString NewParamStr(EnumPtr->GetNameByValue((int)NewParam).ToString());


	FLinearColor ReturnValue;
	switch (NewParam)
	{
	case EEndGameResults::Victory:
		ReturnValue = FLinearColor::Green;
		break;
	case EEndGameResults::Defeat:
		ReturnValue = FLinearColor::Red;
		break;
	case EEndGameResults::Draw:
		ReturnValue = FLinearColor::Yellow;
		break;
	default:
		break;
	}

	TimerDel.BindUFunction(this, FName(TEXT("CreateDisplayMessage_C")), NewParamStr, ReturnValue, true, 3.0, false);
	GetWorldTimerManager().SetTimer(EndClientMatchStateTimerHandle, TimerDel, 0.2, false);
}
