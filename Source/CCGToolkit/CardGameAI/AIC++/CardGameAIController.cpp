// Fill out your copyright notice in the Description page of Project Settings.


#include "CardGameAIController.h"
#include "Engine/World.h"
#include "CCGToolkit/C++/GamePlay/BoardPlayer.h"
#include "CardGameAI_Pawn.h"
#include "CCGToolkit/C++/CardGameState.h"
#include "CCGToolkit/C++/CardGameMode.h"
#include "CCGToolkit/C++/GamePlay/Card3D.h"
#include "Net/UnrealNetwork.h"
#include "../../CardGame/Structs/Structs.h"
#include "../../CardGame/WHOO.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetMathLibrary.h"
#include "../../C++/FunctionLibraries/Card_FunctionLibrary.h"
#include "../../C++/FunctionLibraries/Math_Library.h"
#include "../../C++/FunctionLibraries/Deck_FunctionLibrary.h"
#include "../../C++/FunctionLibraries/CardsInHand_FunctionLibrary.h"
#include "../../C++/FunctionLibraries/Controller_FunctionLibrary.h"
#include "../../C++/FunctionLibraries/Placement_FunctionLibrary.h"
#include "../../C++/FunctionLibraries/CardInteraction_FunctionLibrary.h"
#include "../../C++/FunctionLibraries/Misc_FunctionLibrary.h"


ACardGameAIController::ACardGameAIController()
{
	bReplicates = true;

	bDoOnce = true;

	ActivePriorityFocusList_Array = {EAIPersonalityFocus::Increase_Player_Health, EAIPersonalityFocus::Damage_Cards, EAIPersonalityFocus::Damage_Opponent_Player, EAIPersonalityFocus::Playing_Cards, EAIPersonalityFocus::Picking_up_cards, EAIPersonalityFocus::Opponent_Hindrance};

	ValidInteractionTypes = {ECardType::HERO, ECardType::Creature};
}

void ACardGameAIController::OnPossess(APawn* InPawn)
{
	ControlledPawn_Ref = Cast<ACardGameAI_Pawn>(InPawn);
	ControlledPawn_Ref->OwningController_Ref = this;

	ControlledPawn_Ref->InitAttributes();

	UBlackboardComponent* BlackboardComponent;

	UBlackboardData* BlackboardAsset = LoadObject<UBlackboardData>(NULL, TEXT("BlackboardData'/Game/CCGToolkit/CardGameAI/AIBlueprints/CardGameAI_Blackboard.CardGameAI_Blackboard'"));
	AAIController::UseBlackboard(BlackboardAsset, BlackboardComponent);
	AIBlackboard_Ref = BlackboardComponent;
	UBehaviorTree* BTAsset = LoadObject<UBehaviorTree>(NULL, TEXT("BehaviorTree'/Game/CCGToolkit/CardGameAI/AIBlueprints/CardGameAI_BehaviorTree.CardGameAI_BehaviorTree'"));
	AAIController::RunBehaviorTree(BTAsset);

	LoadAIPersonality(Default_Personality);
	SetupAI();
	UpdateAIPlayerState();
	GetWorldTimerManager().SetTimer(CreateAIDebugUIHandle, this, &ACardGameAIController::CreateAIDebugUI, 0.2f, false);
}

void ACardGameAIController::BeginPlay()
{
	GameState_Ref = Cast<ACardGameState>(UGameplayStatics::GetGameState(this));
	GameMode_Ref = Cast<ACardGameMode>(UGameplayStatics::GetGameMode(this));
	//StartCoroutine("CoroBeginPlay", CoroBeginPlay());
}

WHCoroTask* ACardGameAIController::CoroBeginPlay()
{
	WHCORO_PARAM(ACardGameAIController)

#include WHCORO_BEGIN()
#include WHYIELD_READY()
	WHYIELD_RETURN_SECOND(2.0f);

	D->GameMode_Ref = Cast<ACardGameMode>(UGameplayStatics::GetGameMode(D));
	D->GameState_Ref = Cast<ACardGameState>(UGameplayStatics::GetGameState(D));

#include WHCORO_END()
}

bool ACardGameAIController::ChangeActivePlayerTurn_C_Implementation(bool InTurnActive)
{
	if (HasAuthority())
	{
		bTurnActive = InTurnActive;
		AIBlackboard_Ref->SetValueAsBool(TurnActive, bTurnActive);
		if (bTurnActive)
		{
			IPlayerState_Interface* PlayerState_Interface = Cast<IPlayerState_Interface>(ControlledPawn_Ref);
			PlayerState_Interface->Execute_GetManaForTurn_C(ControlledPawn_Ref);
			ControlledPawn_Ref->PlayerTurn++;
			ICardsInHand_Interface* CardsInHand_Interface = Cast<ICardsInHand_Interface>(this);
			if (CardsInHand_Interface)
			{
				CardsInHand_Interface->Execute_DrawCard_C(this, "None", false, 1);
			}
			UpdateAIPlayerState();
		}
	}
	return false;
}

bool ACardGameAIController::GetPlayerDeck_C_Implementation(TArray<FName>& Deck)
{
	Deck = AIDeck;
	return false;
}

bool ACardGameAIController::MatchBegin_C_Implementation()
{
	if (HasAuthority())
	{
		ICardsInHand_Interface* CardsInHand_Interface = Cast<ICardsInHand_Interface>(this);
		if (CardsInHand_Interface)
		{
			CardsInHand_Interface->Execute_DrawCard_C(this, "None", false, CardsInFirstHand);
		}
		UpdateAIState();
	}
	return false;
}

bool ACardGameAIController::MatchEnd_C_Implementation(EEndGameResults Result)
{
	if (bDoOnce)
	{
		if (bTurnActive)
		{
			bTurnActive = false;
		}
		AIBlackboard_Ref->SetValueAsBool(TurnActive, false);
		bDoOnce = false;
	}
	return false;
}

bool ACardGameAIController::RequestChangePlayerTurn_C_Implementation()
{
	ACardGameState* CardGameState = Cast<ACardGameState>(UGameplayStatics::GetGameState(this));
	if (CardGameState)
	{
		if (CardGameState->RequestChangeTurnState_C(this))
		{
			CardGameState->Server_RequestChangeTurnState_C(this);
		}
	}
	return false;
}

bool ACardGameAIController::DrawCard_C_Implementation(FName InCardName, bool IgnoreMaxCards, int32 NumberOfCardsToDraw)
{
	if (HasAuthority())
	{
		CardToAdd = InCardName;
		if (UDeck_FunctionLibrary::CountCardsInDeck(AIDeck) - NumberOfCardsToAdd > 0 )
		{
			if (!IgnoreMaxCards && !(UCardsInHand_FunctionLibrary::CountCardsInHand(AICardsInHand) >= MaxCardsInHand) && ((NumberOfCardsToDraw + UCardsInHand_FunctionLibrary::CountCardsInHand(AICardsInHand) + NumberOfCardsToAdd) > MaxCardsInHand))
			{
				NumberOfCardsToAdd = MaxCardsInHand - (UCardsInHand_FunctionLibrary::CountCardsInHand(AICardsInHand) + NumberOfCardsToAdd) + NumberOfCardsToAdd;
			}
			else
			{
				NumberOfCardsToAdd = NumberOfCardsToAdd + NumberOfCardsToDraw;
			}
			if (!DrawCardHandle.IsValid())
			{
				GetWorldTimerManager().SetTimer(DrawCardHandle, this, &ACardGameAIController::Event_AddCardToHand_C, CardPickupDelay, true);
			}
		}
	}
	return false;
}

bool ACardGameAIController::AddCardToPlayersHand_C_Implementation(FName CardName)
{
	AICardsInHand.Add(CardName);
	UpdateAIPlayerState();
	return true;
}

bool ACardGameAIController::RemoveCardFromDeck_C_Implementation(bool RemoveAll, int32 IndexToRemove)
{
	UDeck_FunctionLibrary::RemoveCardFromDeck(RemoveAll, IndexToRemove, AIDeck);
	UpdateAIPlayerState();
	return true;
}

bool ACardGameAIController::RemoveCardFromHand_C_Implementation(FName Card, int32 Index, bool RemoveAll)
{
	if (HasAuthority())
	{
		if (RemoveAll)
		{
			AICardsInHand.Empty();
			UpdateAIState();
			return true;
		}
		else
		{
			if (AICardsInHand.Find(Card) >= 0)
			{
				AICardsInHand.RemoveAt(AICardsInHand.Find(Card));
				UpdateAIState();
				return true;
			}
			return false;
		}
	}
	return false;
}

ACard3D* ACardGameAIController::CreatePlayableCard_C_Implementation(FTransform SpawnTransform)
{
	UClass* Card3DClass = LoadClass<ACard3D>(NULL, TEXT("Blueprint'/Game/CCGToolkitC++/Blueprints/GamePlay/3DCard.3DCard_C'"));
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined;
	SpawnParameters.Owner = this;

	return GetWorld()->SpawnActor<ACard3D>(Card3DClass, SpawnTransform, SpawnParameters);
}

void ACardGameAIController::Event_AddCardToHand_C()
{
	if (NumberOfCardsToAdd >= 1)
	{
		NumberOfCardsToAdd--;
		FName CardName;
		IController_Interface* Controller_Interface = Cast<IController_Interface>(this);
		IDeck_Interface* Deck_Interface = Cast<IDeck_Interface>(this);
		if (CardToAdd == "None")
		{
			CardName = UDeck_FunctionLibrary::GetCardFromDeck(AIDeck, 0, false);
			Controller_Interface->Execute_AddCardToPlayersHand_C(this, CardName);
			Deck_Interface->Execute_RemoveCardFromDeck_C(this, false, 0);
		}
		else
		{
			CardName = CardToAdd;
			Controller_Interface->Execute_AddCardToPlayersHand_C(this, CardName);
		}
		UpdateAIState();
		if (NumberOfCardsToAdd <= 0)
		{
			GetWorldTimerManager().ClearTimer(DrawCardHandle);
			DrawCardHandle.Invalidate();
		}
	}
	else
	{
		GetWorldTimerManager().ClearTimer(DrawCardHandle);
		DrawCardHandle.Invalidate();
	}	
}

bool ACardGameAIController::AIPlayCard(const FName& CardName, ECardSet CardSet)
{
	if (CardName != "None")
	{
		FSCard CardStruct;
		int32 SelfPlayer;
		int32 Oppenent;
		int32 SelfAndOpponent;
		int32 PlayerID = 0;
		TArray<ACardPlacement*> ReturnAvailablePlacements;

		UDeck_FunctionLibrary::GetCardData(CardName, ECardSet::Empty, CardStruct);
		UController_FunctionLibrary::GetControllerIDForBothPlayers_ServerOnly(this, SelfPlayer, Oppenent);
		if (FMath::RandBool())
		{
			SelfAndOpponent = Oppenent;
		}
		else
		{
			SelfAndOpponent = SelfPlayer;
		}

		switch (CardStruct.Placement_Settings.Owner)
		{
		case EPlayers::Self:
			PlayerID = SelfPlayer;
			break;
		case EPlayers::Opponent:
			PlayerID = Oppenent;
			break;
		case EPlayers::SelfAndOpponent:
			PlayerID = SelfAndOpponent;
			break;
		default:
			break;
		}

		if (UPlacement_FunctionLibrary::GetPlayersAvailablePlacementsForCard(PlayerID, CardName, ReturnAvailablePlacements))
		{
			IPlayerState_Interface* PlayerState_Interface = Cast<IPlayerState_Interface>(ControlledPawn_Ref);
			PlayerState_Interface->Execute_DecreasePlayerMana_C(ControlledPawn_Ref, CardStruct.Placement_Settings.Mana_Cost);
			ICardsInHand_Interface* CardsInHand_Interface = Cast<ICardsInHand_Interface>(this);
			CardsInHand_Interface->Execute_RemoveCardFromHand_C(this, CardName, 0, false);
			IController_Interface* Controlller_Interface = Cast<IController_Interface>(this);

			FVector SpawnTransform;
			SpawnTransform.X = GetPawn()->GetActorTransform().GetLocation().X;
			SpawnTransform.Y = GetPawn()->GetActorTransform().GetLocation().Y;
			SpawnTransform.Z = GetPawn()->GetActorTransform().GetLocation().Z / 100;
			ACard3D* CardCreated = Controlller_Interface->Execute_CreatePlayableCard_C(this, FTransform(SpawnTransform));

			FSCard SetupCard;
			ACard3D* Card_Ref = UCard_FunctionLibrary::SetupCard(CardCreated, ControlledPawn_Ref->PlayerID, CardName, CardSet, SetupCard, false);
			UCard_FunctionLibrary::AddCardToBoardPlacement(Card_Ref, ReturnAvailablePlacements[UKismetMathLibrary::RandomIntegerInRange(0, ReturnAvailablePlacements.Num() - 1)], ControlledPawn_Ref->PlayerID);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "AI Player: " + FString::FromInt(ControlledPawn_Ref->CardGameAIID) + "   Played Card: " + CardName.ToString());
			
			return true;
		}
		return false;
	}
	return false;
}

void ACardGameAIController::UpdateAIState()
{
	if (HasAuthority())
	{
		TArray<FName> PlayerableCardsInHand;
		PlayerableCardsInHand = UCardsInHand_FunctionLibrary::FindPlayableCardsInHand(this);
		PlayableCardsInHandList = SortCardsByFocus(PlayerableCardsInHand, PriorityFocusList_Array, true);
		if (GameState_Ref)
		{
			TArray<ACard3D*> PlayerCards;
			TArray<ACard3D*> OpponentCards;
			GameState_Ref->GetBoardState_C(ControlledPawn_Ref->PlayerID, PlayerCards, OpponentCards);
			CardsOnBoardWithTurnPoints = UCard_FunctionLibrary::FilterCardsWithTurnPoints(PlayerCards, ValidInteractionTypes);
			EvaluateBoardState();
			UpdateAIPlayerState();
		}
	}
}

float ACardGameAIController::GetFocusPriority_Implementation(EAIPersonalityFocus Focus, EPriority& ReturnPriority)
{
	return 0;
}

void ACardGameAIController::UpdateAIPlayerState()
{
	ControlledPawn_Ref->ForceUIUpdate(AICardsInHand.Num(), AIDeck.Num());
}

bool ACardGameAIController::EvaluateBoardState_Implementation()
{
	return false;
}

bool ACardGameAIController::RunCardInteraction(FSCardInteraction AICardInteraction_Struct)
{
	int32 Cost;

	ACard3D* Card3D = Cast<ACard3D>(AICardInteraction_Struct.TalkingCard);
	ABoardPlayer* BoardPlayer = Cast<ABoardPlayer>(AICardInteraction_Struct.RecievingActor);
	ACard3D* Card3D_2 = Cast<ACard3D>(AICardInteraction_Struct.RecievingActor);
	if (Card3D_2 && !(!Card3D->bInGraveyard && !Card3D_2->bInGraveyard))
	{
		return false;
	}
	else if(Card3D_2 && !BoardPlayer)
	{
		return false;
	}
	if (UCard_FunctionLibrary::ChargeCardManaCost(ControlledPawn_Ref->PlayerID, false, Card3D->CardDataStruct, EManaAccount::Attack, Cost))
	{
		EInteractionConditions CallingCardCondition;
		EInteractionConditions TalkingCardCondition;
		EInteractionConditions InteractionConditionFailure;
		switch (AICardInteraction_Struct.Interaction_Type)
		{
		case EPlayType::AttackCard:
			UpdateAIState();
			return UCardInteraction_FunctionLibrary::DealDamageToCard(false, Card3D, Card3D_2, 0, CallingCardCondition, TalkingCardCondition);
			break;
		case EPlayType::AttackPlayer:
			UpdateAIState();
			return UCardInteraction_FunctionLibrary::DealDamageToPlayer(false, Card3D, BoardPlayer, 0, InteractionConditionFailure);
			break;
		default:
			break;
		}
	}
	return false;
}

void ACardGameAIController::CalculatePriorityFocus_Implementation()
{
	
}

void ACardGameAIController::LoadAIPersonality(FName RowName)
{
	FString ContextString;
	UDataTable* AIPersonalityDataTable;
	AIPersonalityDataTable = LoadObject<UDataTable>(NULL, TEXT("DataTable'/Game/CCGToolkitC++/CardGame/DataTable/AIPersonality_C_DataTable.AIPersonality_C_DataTable'"));
	
	TArray<FName> RowNames;
	RowNames = AIPersonalityDataTable->GetRowNames();
	
	if (AIPersonalityDataTable)
	{
		FSAIPersonality* pRow = AIPersonalityDataTable->FindRow<FSAIPersonality>(RowName, ContextString);
		if (pRow)
		{
			AI_Personality_Struct = *pRow;
			ControlledPawn_Ref->AI_Name = pRow->AI_Name.ToString();

			UDataTable* AIPointsAllocationDataTable = LoadObject<UDataTable>(NULL, TEXT("DataTable'/Game/CCGToolkitC++/CardGame/DataTable/AIPointsAILocation_C_DataTable.AIPointsAILocation_C_DataTable'"));
			AIPointAllocation_Struct = *AIPointsAllocationDataTable->FindRow<FSPointAllocation>(pRow->AI_Points_Allocation, ContextString);
			AI_Difficulty = pRow->Difficulty;

			AIBlackboard_Ref->SetValueAsEnum(Difficulty, uint8(AI_Difficulty));
			AI_PlaySpeed = pRow->Play_Speed;
			AIBlackboard_Ref->SetValueAsFloat(PlaySpeed, AI_PlaySpeed);
			AI_Awareness;
			AIBlackboard_Ref->SetValueAsInt(PlaySpeed, AI_Awareness);
			AI_PersonalityFocus = pRow->Personality_Focus;
			AIPlayableDecks = pRow->Playable_Decks;
			AIBlackboard_Ref->SetValueAsEnum(TurnState, 1);
			AIBlackboard_Ref->SetValueAsEnum(TurnState, 0);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "CardGameAIController: Failed to get AI 'Personality' data table row");
			UE_LOG(LogTemp, Log, TEXT("CardGameAIController: Failed to get AI 'Personality' data table row"));
		}
	}
}

void ACardGameAIController::SetupAI()
{
	SetupAIDeck();
	bAIReady = true;
}

void ACardGameAIController::SetupAIDeck()
{
	TArray<FName> ReturnDeck;

	SelectedDeck = AIPlayableDecks[UKismetMathLibrary::RandomIntegerInRange(0, AIPlayableDecks.Num() - 1)];
	UCard_FunctionLibrary::LoadPreBuiltDeck(SelectedDeck, ReturnDeck);
	TempDeck = ReturnDeck;
	AIDeck.SetNum(TempDeck.Num());

	if (bShuffleDeck)
	{
		UMath_Library::ShuffleArray(TempDeck);
	}
	if (bEnableWeightedCards)
	{
		AIDeck = UDeck_FunctionLibrary::FilterWeightedCardsInDeck(TempDeck);
	}
	else
	{
		AIDeck = TempDeck;
	}
}

void ACardGameAIController::CreateAIDebugUI_Implementation()
{
	
}

void ACardGameAIController::OnRep_AICardsInHand()
{
	UpdateAIState();
	UpdateAIPlayerState();
}

void ACardGameAIController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACardGameAIController, BoardPlayer_Ref);
	DOREPLIFETIME(ACardGameAIController, AICardsInHand);
	DOREPLIFETIME(ACardGameAIController, AIDeck);
	DOREPLIFETIME(ACardGameAIController, AICardsInHand);
}