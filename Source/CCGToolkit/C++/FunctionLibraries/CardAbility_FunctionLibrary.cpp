// Fill out your copyright notice in the Description page of Project Settings.


#include "CardAbility_FunctionLibrary.h"
#include "../../CardGame/Structs/Structs.h"
#include "../GamePlay/Card3D.h"
#include "Engine/World.h"
#include "Controller_FunctionLibrary.h"
#include "GameFramework/Controller.h"
#include "Placement_FunctionLibrary.h"
#include "../Interface/Controller_Interface.h"
#include "UObject/NoExportTypes.h"
#include "Card_FunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Actor.h"
#include "../Interface/PlayerState_Interface.h"
#include "CardInteraction_FunctionLibrary.h"
#include "../Enums/CardGameEnums.h"
#include "../CardGameState.h"
#include "Deck_FunctionLibrary.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Math_Library.h"
#include "../GamePlay/CardPlacement.h"
#include "../CardGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "../../CardGameAI/AIC++/CardGameAIController.h"

void UCardAbility_FunctionLibrary::DrawCards(ACard3D* CallingCard, int32 AbilityIndex)
{
	AController* Controller = UController_FunctionLibrary::GetControllerReferenceFromID_ServerOnly(CallingCard->OwningPlayerID,GWorld->GetWorld());
	ICardsInHand_Interface* CardsInHands_Interface = Cast<ICardsInHand_Interface>(Controller);
	if (CardsInHands_Interface)
	{
		CardsInHands_Interface->Execute_DrawCard_C(Controller, "None", false, CallingCard->Ability_Struct_Refs[AbilityIndex].Ability_Int);
	}
}

void UCardAbility_FunctionLibrary::IncreaseAttack(ACard3D* CallingCard, int32 AbilityIndex)
{
	CallingCard->Attack = CallingCard->Ability_Struct_Refs[AbilityIndex].Ability_Int + CallingCard->Attack;
	CallingCard->OnRep_Attack_C();
	CallingCard->Multicast_UpdateVisualStats_C();
}

void UCardAbility_FunctionLibrary::CloneCard(ACard3D* CallingCard, int32 AbilityIndex, bool RandomPlacement)
{
	ACard3D* Card_Ref;
	ACardPlacement* GoalPlacement;
	ACardPlacement* ReturnPlacement;
	TArray<UObject*> Temp_BoardPlacement_Array;
	TArray<ACardPlacement*> ReturnAvailablePlacements;
	FSCard Card_Struct;
	AController* controller;

	for (int32 i = 1; i <= CallingCard->Ability_Struct_Refs[AbilityIndex].Ability_Int; i++)
	{
		if (RandomPlacement)
		{
			if (UPlacement_FunctionLibrary::GetPlayersAvailablePlacementsForCard(CallingCard->OwningPlayerID, CallingCard->CardDataTableName, ReturnAvailablePlacements))
			{
				GoalPlacement = ReturnAvailablePlacements[UKismetMathLibrary::RandomIntegerInRange(0, ReturnAvailablePlacements.Num()-1)];
				if (GoalPlacement)
				{
					controller = UController_FunctionLibrary::GetControllerReferenceFromID_ServerOnly(CallingCard->OwningPlayerID, GWorld->GetWorld());
					IController_Interface* controller_Interface = Cast<IController_Interface>(controller);
					if (controller_Interface)
					{
						FTransform spawntransform = CallingCard->GetActorTransform();
						spawntransform.SetLocation(FVector(spawntransform.GetLocation().X, spawntransform.GetLocation().Y, spawntransform.GetLocation().Z+600));
						ACard3D* Card3D_Ref = controller_Interface->Execute_CreatePlayableCard_C(controller,spawntransform);
						Card_Ref = UCard_FunctionLibrary::SetupCard(Card3D_Ref, CallingCard->OwningPlayerID, CallingCard->CardDataTableName, CallingCard->CardSet, Card_Struct,false);
						UCard_FunctionLibrary::SetCustomCardData(Card_Ref, false, 0);
						UCard_FunctionLibrary::AddCardToBoardPlacement(Card3D_Ref, GoalPlacement, CallingCard->OwningPlayerID);
					}
				}
			}
		}
		else
		{
			if (UPlacement_FunctionLibrary::GetClosestAvailablePlacement(CallingCard->PlacementOwner, CallingCard->CardDataTableName, ECardSet::BasicSet, ReturnPlacement))
			{
				GoalPlacement = ReturnPlacement;
				if (GoalPlacement)
				{
					controller = UController_FunctionLibrary::GetControllerReferenceFromID_ServerOnly(CallingCard->OwningPlayerID, GWorld);
					
					IController_Interface* controller_Interface = Cast<IController_Interface>(controller);
					if (controller_Interface)
					{
						FTransform spawntransform = CallingCard->GetActorTransform();
						spawntransform.SetLocation(FVector(spawntransform.GetLocation().X, spawntransform.GetLocation().Y, spawntransform.GetLocation().Z + 600));
						ACard3D* Card3D_Ref = controller_Interface->Execute_CreatePlayableCard_C(controller, spawntransform);
						Card_Ref = UCard_FunctionLibrary::SetupCard(Card3D_Ref, CallingCard->OwningPlayerID, CallingCard->CardDataTableName, CallingCard->CardSet, Card_Struct, false);
						UCard_FunctionLibrary::SetCustomCardData(Card_Ref, false, 0);
						UCard_FunctionLibrary::AddCardToBoardPlacement(Card3D_Ref, GoalPlacement, CallingCard->OwningPlayerID);
					}
				}
			}
		}
	}
}

void UCardAbility_FunctionLibrary::IncreasePlayerHealth(ACard3D* CallingCard, int32 AbilityIndex)
{
	ACardGamePlayerController* TempCardGamePlayerController = nullptr;
	ACardGameAIController* TempCardGameAIController = nullptr;
	AActor* ReplicatedPlayerState = nullptr;
	ACardGameMode* TempCardGameMode = Cast<ACardGameMode>(UGameplayStatics::GetGameMode(GWorld->GetWorld()));
	if (Cast<ACardGamePlayerController>(TempCardGameMode->GameControllersArray[CallingCard->OwningPlayerID - 1]))
	{
		TempCardGamePlayerController = Cast<ACardGamePlayerController>(TempCardGameMode->GameControllersArray[CallingCard->OwningPlayerID - 1]);
		ReplicatedPlayerState = TempCardGamePlayerController->PlayerState;
	}
	if (Cast<ACardGameAIController>(TempCardGameMode->GameControllersArray[CallingCard->OwningPlayerID - 1]))
	{
		TempCardGameAIController = Cast<ACardGameAIController>(TempCardGameMode->GameControllersArray[CallingCard->OwningPlayerID - 1]);
		ReplicatedPlayerState = TempCardGameAIController->GetPawn();
	}
	IPlayerState_Interface* PlayerState_Interface = Cast<IPlayerState_Interface>(ReplicatedPlayerState);
	if (PlayerState_Interface)
	{
		PlayerState_Interface->Execute_IncreasePlayerHealth_C(ReplicatedPlayerState, CallingCard->Ability_Struct_Refs[AbilityIndex].Ability_Int);
	}
}

void UCardAbility_FunctionLibrary::DealRetaliationDamage(ACard3D* CallingCard, int32 AbilityIndex)
{
	EInteractionConditions CallingCardCondition;
	EInteractionConditions TalkingCardCondition;
	if (CallingCard->DamageDealingCard_Ref)
	{
		UCardInteraction_FunctionLibrary::DealDamageToCard(false, CallingCard, CallingCard->DamageDealingCard_Ref, CallingCard->Ability_Struct_Refs[AbilityIndex].Ability_Int, CallingCardCondition, TalkingCardCondition);
	}	
	CallingCard->DamageDealingCard_Ref;
}

void UCardAbility_FunctionLibrary::DamageAllCardsOnBoard(ACard3D* CallingCard, int32 AbilityIndex)
{
	TArray<ACard3D*> Temp_Array;
	TArray<ACard3D*> PlayerCards;
	TArray<ACard3D*> OppenedCards;
	EInteractionConditions CallingCardCondition;
	EInteractionConditions TalkingCardCondition;
	ACardGameState* CardGameState_Ref = Cast<ACardGameState>(UGameplayStatics::GetGameState(CallingCard));
	CardGameState_Ref->GetBoardState_C(CallingCard->OwningPlayerID, PlayerCards, OppenedCards);
	Temp_Array.Append(PlayerCards);
	Temp_Array.Append(OppenedCards);
	for (int32 i = 0; i < Temp_Array.Num(); i++)
	{
		if (Temp_Array[i] !=CallingCard)
		{
			if (!Temp_Array[i]->bInGraveyard)
			{
				if (Temp_Array[i]->bIsPlaced)
				{
					UCardInteraction_FunctionLibrary::DealDamageToCard(true, CallingCard, Temp_Array[i], CallingCard->Ability_Struct_Refs[AbilityIndex].Ability_Int, CallingCardCondition, TalkingCardCondition);
				}
			}
		}
	}
}

void UCardAbility_FunctionLibrary::SpawnRandomCardFromDeck(ACard3D* CallingCard, int32 AbilityIndex)
{
	ACardPlacement* GoalPlacement;
	ACardPlacement* InitialGoalPlacement;
	ACardPlacement* ReturnPlacement;
	FName Card;
	ECardSet CardRace;
	FSCard Card_Struct;

	InitialGoalPlacement = CallingCard->PlacementOwner;

	AController* controller = UController_FunctionLibrary::GetControllerReferenceFromID_ServerOnly(CallingCard->OwningPlayerID, GWorld->GetWorld());

	for (int32 i = 1; i <= CallingCard->Ability_Struct_Refs[AbilityIndex].Ability_Int; i++)	
	{
		int32 Index = UDeck_FunctionLibrary::GetRandomCardFromDeck(controller, Card, CardRace);
		if (UPlacement_FunctionLibrary::GetClosestAvailablePlacement(CallingCard->PlacementOwner, Card, ECardSet::BasicSet, ReturnPlacement))
		{
			if (Card!="None")
			{
				GoalPlacement = ReturnPlacement;
				if (GoalPlacement)
				{
					IController_Interface* Controller_Interface = Cast<IController_Interface>(controller);
					if (Controller_Interface)
					{
						FTransform spawntransform = CallingCard->GetActorTransform();
						spawntransform.SetLocation(FVector(spawntransform.GetLocation().X, spawntransform.GetLocation().Y, spawntransform.GetLocation().Z + 600));
						ACard3D* CardCreated = Controller_Interface->Execute_CreatePlayableCard_C(controller,spawntransform);
						UCard_FunctionLibrary::SetupCard(CardCreated, CallingCard->OwningPlayerID, Card, ECardSet::Empty, Card_Struct, false);
						UCard_FunctionLibrary::AddCardToBoardPlacement(CardCreated, GoalPlacement, CallingCard->OwningPlayerID);
					}
					IDeck_Interface* Deck_Interface = Cast<IDeck_Interface>(controller);
					if (Deck_Interface)
					{
						Deck_Interface->Execute_RemoveCardFromDeck_C(controller, false, Index);
					}
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			for (int32 j = 0; j<=3 ; j++)
			{
				int32 Index_2 = UDeck_FunctionLibrary::GetRandomCardFromDeck(controller, Card, CardRace);
				if (UPlacement_FunctionLibrary::GetClosestAvailablePlacement(CallingCard->PlacementOwner, Card, ECardSet::BasicSet, ReturnPlacement))
				{
					if (Card != "None")
					{
						GoalPlacement = ReturnPlacement;
						if (GoalPlacement)
						{
							IController_Interface* Controller_Interface = Cast<IController_Interface>(controller);
							if (Controller_Interface)
							{
								FTransform spawntransform = CallingCard->GetActorTransform();
								spawntransform.SetLocation(FVector(spawntransform.GetLocation().X, spawntransform.GetLocation().Y, spawntransform.GetLocation().Z + 600));
								ACard3D* CardCreated = Controller_Interface->Execute_CreatePlayableCard_C(controller, spawntransform);
								UCard_FunctionLibrary::SetupCard(CardCreated, CallingCard->OwningPlayerID, Card, ECardSet::Empty, Card_Struct, false);
								UCard_FunctionLibrary::AddCardToBoardPlacement(CardCreated, GoalPlacement, CallingCard->OwningPlayerID);
							}
							IDeck_Interface* Deck_Interface = Cast<IDeck_Interface>(controller);
							if (Deck_Interface)
							{
								Deck_Interface->Execute_RemoveCardFromDeck_C(controller, false, Index_2);
							}
						}
						else
						{
							break;
						}
					}
					break;
				}
			}
			break;
		}
	}
}

void UCardAbility_FunctionLibrary::GiveTurnPointstoAllActiveCards(ACard3D* CallingCard, int32 AbilityIndex)
{
	TArray<ACard3D*> PlayerCards;
	TArray<ACard3D*> OppenedCards;
	ACardGameState* CardGameState = Cast<ACardGameState>(GWorld->GetGameState());
	CardGameState->GetBoardState_C(CallingCard->OwningPlayerID, PlayerCards, OppenedCards);
	for (int32 i = 0; i<PlayerCards.Num(); i++)
	{
		PlayerCards[i]->TurnPoints += CallingCard->Ability_Struct_Refs[AbilityIndex].Ability_Int;
		PlayerCards[i]->Multicast_UpdateCardVisual_C(true);
	}
}

void UCardAbility_FunctionLibrary::IncreaseTurnPoints(ACard3D* CallingCard, int32 AbilityIndex)
{
	CallingCard->TurnPoints += CallingCard->Ability_Struct_Refs[AbilityIndex].Ability_Int;
}

void UCardAbility_FunctionLibrary::DiscardRandomCardsFromHand(ACard3D* CallingCard, int32 AbilityIndex)
{
	int32 Temp_CardIndex;
	FName Temp_CardName;

	int32 SelfPlayer;
	int32 Oppenent;

	int32 ReturnValue = 0;

	FSCard Card_Struct;

	if (CallingCard->Ability_Struct_Refs[AbilityIndex].Affecting_Player == EPlayers::SelfAndOpponent)
	{
		for (int32 i = 0; i <= 2; i++)
		{
			AController* controller = UController_FunctionLibrary::GetControllerReferenceFromID_ServerOnly(CallingCard->OwningPlayerID, GWorld->GetWorld());
			UController_FunctionLibrary::GetControllerIDForBothPlayers_ServerOnly(controller, SelfPlayer, Oppenent);
			switch (CallingCard->Ability_Struct_Refs[AbilityIndex].Affecting_Player)
			{
			case EPlayers::Self:
				ReturnValue = SelfPlayer;
				break;
			case EPlayers::Opponent:
				ReturnValue = Oppenent;
				break;
			case EPlayers::SelfAndOpponent:
				ReturnValue = i;
				break;
			default:
				break;
			}

			AController* controller2 = UController_FunctionLibrary::GetControllerReferenceFromID_ServerOnly(ReturnValue, GWorld->GetWorld());

			for (int32 j = 1; j <= CallingCard->Ability_Struct_Refs[AbilityIndex].Ability_Int; j++)
			{
				FControllersState ContorllersState = UController_FunctionLibrary::GetControllersStateProfile_ServerOnly(ReturnValue);
				Temp_CardIndex = 0;
				Temp_CardName = "None";
				if (ContorllersState.CardsInHand.Num() >= 1)
				{
					Temp_CardIndex = UKismetMathLibrary::RandomIntegerInRange(0, ContorllersState.CardsInHand.Num()-1);
					Temp_CardName = ContorllersState.CardsInHand[Temp_CardIndex];
					if (Temp_CardName != "None")
					{
						ICardsInHand_Interface* CardsInHand_Interface = Cast<ICardsInHand_Interface>(controller2);
						if (CardsInHand_Interface)
						{
							if(CardsInHand_Interface->Execute_RemoveCardFromHand_C(controller2, Temp_CardName, Temp_CardIndex, false))
							{
								ACardGameState* CardGameState = Cast<ACardGameState>(GWorld->GetGameState());
								AGraveyard* ReturnGraveyard = CardGameState->GetGraveyardReference_C(ReturnValue);
								if (ReturnGraveyard)
								{
									IController_Interface* Controller_Interface = Cast<IController_Interface>(controller2);
									if (Controller_Interface)
									{
										FTransform SpawnTransform;
										SpawnTransform.SetLocation(FVector(ReturnGraveyard->GetActorLocation().X, ReturnGraveyard->GetActorLocation().Y, ReturnGraveyard->GetActorLocation().Z + 500));
										ACard3D* CardCreated = Controller_Interface->Execute_CreatePlayableCard_C(controller2, SpawnTransform);
										ACard3D* CardRef = UCard_FunctionLibrary::SetupCard(CardCreated, ReturnValue, Temp_CardName, ECardSet::Empty, Card_Struct, false);
										ReturnGraveyard->AddToGraveyard_C(CardRef, "None");
										CardRef->ForceMoveCardDirectlyToGraveyard_C(ReturnGraveyard);
									}							
								}
							}
						}
					}
				}
			}
		}
	}
	else
	{
		AController* controller = UController_FunctionLibrary::GetControllerReferenceFromID_ServerOnly(CallingCard->OwningPlayerID, GWorld->GetWorld());

		UController_FunctionLibrary::GetControllerIDForBothPlayers_ServerOnly(controller, SelfPlayer, Oppenent);
		switch (CallingCard->Ability_Struct_Refs[AbilityIndex].Affecting_Player)
		{
		case EPlayers::Self:
			ReturnValue = SelfPlayer;
			break;
		case EPlayers::Opponent:
			ReturnValue = Oppenent;
			break;
		case EPlayers::SelfAndOpponent:
			break;
		default:
			break;
		}

		AController* controller2 = UController_FunctionLibrary::GetControllerReferenceFromID_ServerOnly(ReturnValue, GWorld->GetWorld());
		
		for (int32 j = 1; j <= CallingCard->Ability_Struct_Refs[AbilityIndex].Ability_Int; j++)
		{
			Temp_CardIndex = 0;
			Temp_CardName = "None";

			FControllersState ContorllersState = UController_FunctionLibrary::GetControllersStateProfile_ServerOnly(ReturnValue);

			if (ContorllersState.CardsInHand.Num() >= 1)
			{
				Temp_CardIndex = UKismetMathLibrary::RandomIntegerInRange(0, ContorllersState.CardsInHand.Num() - 1);
				Temp_CardName = ContorllersState.CardsInHand[Temp_CardIndex];
				if (Temp_CardName != "None")
				{
					ICardsInHand_Interface* CardsInHand_Interface = Cast<ICardsInHand_Interface>(controller2);
					if (CardsInHand_Interface)
					{
						if (CardsInHand_Interface->Execute_RemoveCardFromHand_C(controller2, Temp_CardName, Temp_CardIndex, false))
						{
							ACardGameState* CardGameState = Cast<ACardGameState>(GWorld->GetGameState());
							AGraveyard* ReturnGraveyard = CardGameState->GetGraveyardReference_C(ReturnValue);
							if (ReturnGraveyard)
							{
								IController_Interface* Controller_Interface = Cast<IController_Interface>(controller2);
								if (Controller_Interface)
								{
									FTransform SpawnTransform;
									SpawnTransform.SetLocation(FVector(ReturnGraveyard->GetActorLocation().X, ReturnGraveyard->GetActorLocation().Y, ReturnGraveyard->GetActorLocation().Z + 500));
									ACard3D* CardCreated = Controller_Interface->Execute_CreatePlayableCard_C(controller2, SpawnTransform);
									ACard3D* CardRef = UCard_FunctionLibrary::SetupCard(CardCreated, ReturnValue, Temp_CardName, ECardSet::Empty, Card_Struct, false);
									ReturnGraveyard->AddToGraveyard_C(CardRef, "None");
									CardRef->ForceMoveCardDirectlyToGraveyard_C(ReturnGraveyard);
								}
							}
						}
					}
				}
			}
		}
	}
}

void UCardAbility_FunctionLibrary::PickupCardsFromGraveyard(ACard3D* CallingCard, int32 AbilityIndex, bool AddDirectlyToBoard, ECardType CardType)
{
	AGraveyard* Graveyard = nullptr;
	int32 Index;
	FName CardName;
	int32 PlayerIndex = 0;
	ACardPlacement* GoalPlacement;
	ACard3D* Card_Ref;
	FSCard Card_Struct;
	TArray<ACardPlacement*> PlacementArray;
	int32 Total = 0;
	bool ValidPlacement;
	ACard3D* CardCreated;
	ACardPlacement* ReturnPlacement;

	int32 SelfPlayer, Oppenent;

	Card_Ref = CallingCard;

	AController* controller = UController_FunctionLibrary::GetControllerReferenceFromID_ServerOnly(CallingCard->OwningPlayerID, GWorld->GetWorld());
	UController_FunctionLibrary::GetControllerIDForBothPlayers_ServerOnly(controller, SelfPlayer, Oppenent);
	switch (CallingCard->Ability_Struct_Refs[AbilityIndex].Affecting_Player)
	{
	case EPlayers::Self:
		PlayerIndex = SelfPlayer;
		break;
	case EPlayers::Opponent:
		PlayerIndex = Oppenent;
		break;
	case EPlayers::SelfAndOpponent:
		PlayerIndex = 0;
		break;
	default:
		break;
	}

	ACardGameState* CardGameState = Cast<ACardGameState>(GWorld->GetGameState());
	Graveyard = CardGameState->GetGraveyardReference_C(PlayerIndex);
	if (Graveyard)
	{
		if (Graveyard->GraveyardList.Num() > 0)
		{
			for (int32 i = 1; i<=CallingCard->Ability_Struct_Refs[AbilityIndex].Ability_Int;i++)
			{
				CardName = "None";
				Index = 0;
				for (int32 j = 0; j < Graveyard->GraveyardList.Num(); j++)
				{
					UDeck_FunctionLibrary::GetCardData(Graveyard->GraveyardList[Graveyard->GraveyardList.Num()-1-j], ECardSet::Empty, Card_Struct);
					if (CardType!=ECardType::None)
					{
						if (Card_Struct.Type == CardType)
						{
							CardName = Graveyard->GraveyardList[Graveyard->GraveyardList.Num() - 1 - j];
							Index = Graveyard->GraveyardList.Num() - 1 - j;
							break;
						}
					}
					else
					{
						CardName = Graveyard->GraveyardList[Graveyard->GraveyardList.Num() - 1 - j];
						Index = Graveyard->GraveyardList.Num() - 1 - j;
						break;
					}
				}
				if (CardName!="None")
				{
					if (AddDirectlyToBoard)
					{
						CardGameState = Cast<ACardGameState>(UGameplayStatics::GetGameState(CallingCard));
						CardGameState->GetCardPlacementReferences_C(Card_Ref->OwningPlayerID, PlacementArray, Total);
						UPlacement_FunctionLibrary::GetIsValidPlacement(PlacementArray[0], CardName, ECardSet::Empty, ValidPlacement);
						if (ValidPlacement)
						{
							GoalPlacement = PlacementArray[0];	
							controller = UController_FunctionLibrary::GetControllerReferenceFromID_ServerOnly(PlayerIndex, GWorld->GetWorld());
							FTransform SpawnTransform = UKismetMathLibrary::Conv_VectorToTransform(Graveyard->GetCardInGraveyardLocation_C(0));
							IController_Interface* Controller_Interface = Cast<IController_Interface>(controller);
							if (Controller_Interface)
							{
								CardCreated = Controller_Interface->Execute_CreatePlayableCard_C(controller, SpawnTransform);
							}
							UDeck_FunctionLibrary::GetCardData(CardName, ECardSet::Empty, Card_Struct);
							Graveyard->GraveyardStructList[Index].Health = Card_Struct.Health;
							ACard3D* CardRef = UCard_FunctionLibrary::SetupCard(CardCreated, Card_Ref->OwningPlayerID, CardName, ECardSet::Empty, Graveyard->GraveyardStructList[Index], true);
							UCard_FunctionLibrary::SetCustomCardData(CardRef, true, 0);
							if (UCard_FunctionLibrary::AddCardToBoardPlacement(CardRef, GoalPlacement, CardRef->OwningPlayerID))
							{
								Graveyard->Event_RemoveCardFromGraveyard_C(CardName, Index);
							}
						}
						else 
						{
							if (UPlacement_FunctionLibrary::GetClosestAvailablePlacement(PlacementArray[0], CardName, ECardSet::Empty, ReturnPlacement))
							{
								GoalPlacement = ReturnPlacement;
								controller = UController_FunctionLibrary::GetControllerReferenceFromID_ServerOnly(PlayerIndex, GWorld->GetWorld());
								FTransform SpawnTransform = UKismetMathLibrary::Conv_VectorToTransform(Graveyard->GetCardInGraveyardLocation_C(0));
								IController_Interface* Controller_Interface = Cast<IController_Interface>(controller);
								if (Controller_Interface)
								{
									CardCreated = Controller_Interface->Execute_CreatePlayableCard_C(controller, SpawnTransform);
								}
								UDeck_FunctionLibrary::GetCardData(CardName, ECardSet::Empty, Card_Struct);
								Graveyard->GraveyardStructList[Index].Health = Card_Struct.Health;
								ACard3D* CardRef = UCard_FunctionLibrary::SetupCard(CardCreated, Card_Ref->OwningPlayerID, CardName, ECardSet::Empty, Graveyard->GraveyardStructList[Index], true);
								UCard_FunctionLibrary::SetCustomCardData(CardRef, true, 0);
								if (UCard_FunctionLibrary::AddCardToBoardPlacement(CardRef, GoalPlacement, CardRef->OwningPlayerID))
								{
									Graveyard->Event_RemoveCardFromGraveyard_C(CardName, Index);
								}
							}
						}
					}
					else
					{
						controller = UController_FunctionLibrary::GetControllerReferenceFromID_ServerOnly(Card_Ref->OwningPlayerID, GWorld->GetWorld());
						ICardsInHand_Interface* CardsInHand_Interface = Cast<ICardsInHand_Interface>(controller);
						if (CardsInHand_Interface)
						{
							CardsInHand_Interface->Execute_DrawCard_C(controller, CardName, false, 1);
							Graveyard->Event_RemoveCardFromGraveyard_C(CardName, Index);
						}
					}
				}
			}
		}
	}
}

void UCardAbility_FunctionLibrary::ChangePlayedCardOwner(ACard3D* CallingCard, int32 AbilityIndex)
{
	ACard3D* Card_Ref = nullptr;
	ACardPlacement* GoalPlacement_Ref;
	ACardPlacement* ReturnPlacement;
	TArray<ACard3D*> Temp_CardArray_Ref;
	TArray<ACard3D*> Temp_IgnoreCardArray_Ref_0;
	int32 SelfPlayer, Oppenent;
	int32 ReturnValue = 0;
	TArray<ACard3D*> PlayerCards;
	TArray<ACard3D*> OppenentCards;
	bool ValidPlacement;
	int32 Total = 0;

	AController* controller = UController_FunctionLibrary::GetControllerReferenceFromID_ServerOnly(CallingCard->OwningPlayerID, GWorld->GetWorld());
	UController_FunctionLibrary::GetControllerIDForBothPlayers_ServerOnly(controller, SelfPlayer, Oppenent);

	TArray<ACardPlacement*> PlacementArray;

	if (CallingCard->Ability_Struct_Refs[AbilityIndex].Affecting_Player == EPlayers::SelfAndOpponent)
	{
		for (int32 i = 1; i <= 2; i++)
		{			
			switch (CallingCard->Ability_Struct_Refs[AbilityIndex].Affecting_Player)
			{
			case EPlayers::Self:
				ReturnValue = SelfPlayer;
				break;
			case EPlayers::Opponent:
				ReturnValue = Oppenent;
				break;
			case EPlayers::SelfAndOpponent:
				ReturnValue = i;
				break;
			default:
				break;
			}
			for (int32 j = 0; j < CallingCard->Ability_Struct_Refs[AbilityIndex].Ability_Int; j++)
			{
				Temp_CardArray_Ref.Empty();
				ACardGameState* CardGameState = Cast<ACardGameState>(GWorld->GetGameState());
				CardGameState->GetBoardState_C(ReturnValue, PlayerCards, OppenentCards);
				Temp_CardArray_Ref = OppenentCards;
				UMath_Library::ShuffleArray(Temp_CardArray_Ref);
				for (int32 k = 0;k<Temp_CardArray_Ref.Num();k++)
				{
					if (Temp_CardArray_Ref[k]->PlacementOwner->PlayerIndex != ReturnValue && Temp_CardArray_Ref[k]->OwningPlayerID != ReturnValue && !Temp_IgnoreCardArray_Ref_0.Contains(Temp_CardArray_Ref[k]))
					{
						Card_Ref = Temp_CardArray_Ref[k];
					}
					else
					{
						if (Temp_CardArray_Ref[k]->PlacementOwner->PlayerIndex == 0)
						{
							Card_Ref = Temp_CardArray_Ref[k];
							break;
						}
					}
				}
				if (Card_Ref)
				{
					CardGameState = Cast<ACardGameState>(GWorld->GetWorld());
					CardGameState->GetCardPlacementReferences_C(ReturnValue, PlacementArray, Total);
					UPlacement_FunctionLibrary::GetIsValidPlacement(PlacementArray[0], Card_Ref->CardDataTableName, ECardSet::Empty, ValidPlacement);
					if (ValidPlacement)
					{
						GoalPlacement_Ref = PlacementArray[0];
						Temp_IgnoreCardArray_Ref_0.Add(Card_Ref);
						CardGameState->RemoveCardOnBoard_C(Card_Ref, Card_Ref->OwningPlayerID);
						Card_Ref->PlacementOwner->RemoveCardFromPlacement_C(Card_Ref);
						Card_Ref->OwningPlayerID = ReturnValue;
						Card_Ref->OnRep_OwningPlayerID_C();
						UCard_FunctionLibrary::SetCustomCardData(Card_Ref, true, 1);
						UCard_FunctionLibrary::AddCardToBoardPlacement(Card_Ref, GoalPlacement_Ref, ReturnValue);
					}
					else
					{
						if (UPlacement_FunctionLibrary::GetClosestAvailablePlacement(PlacementArray[0], Card_Ref->CardDataTableName, ECardSet::Empty, ReturnPlacement))
						{
							GoalPlacement_Ref = ReturnPlacement;
							Temp_IgnoreCardArray_Ref_0.Add(Card_Ref);
							CardGameState->RemoveCardOnBoard_C(Card_Ref, Card_Ref->OwningPlayerID);
							Card_Ref->PlacementOwner->RemoveCardFromPlacement_C(Card_Ref);
							Card_Ref->OwningPlayerID = ReturnValue;
							Card_Ref->OnRep_OwningPlayerID_C();
							UCard_FunctionLibrary::SetCustomCardData(Card_Ref, true, 1);
							UCard_FunctionLibrary::AddCardToBoardPlacement(Card_Ref, GoalPlacement_Ref, ReturnValue);
						}
					}
				}
			}
		}

	}
	else
	{
		for (int32 i = 0; i < CallingCard->Ability_Struct_Refs[AbilityIndex].Ability_Int; i++)
		{
			Temp_CardArray_Ref.Empty();
			ACardGameState* CardGameState = Cast<ACardGameState>(GWorld->GetGameState());
			CardGameState->GetBoardState_C(ReturnValue, PlayerCards, OppenentCards);
			Temp_CardArray_Ref = OppenentCards;
			UMath_Library::ShuffleArray(Temp_CardArray_Ref);
			for (int32 j = 0; j < Temp_CardArray_Ref.Num(); j++)
			{
				if (Temp_CardArray_Ref[j]->PlacementOwner->PlayerIndex != ReturnValue && Temp_CardArray_Ref[j]->OwningPlayerID != ReturnValue && !Temp_IgnoreCardArray_Ref_0.Contains(Temp_CardArray_Ref[j]))
				{
					Card_Ref = Temp_CardArray_Ref[j];
				}
				else
				{
					if (Temp_CardArray_Ref[j]->PlacementOwner->PlayerIndex == 0)
					{
						Card_Ref = Temp_CardArray_Ref[j];
						break;
					}
				}
			}
			if (Card_Ref)
			{
				CardGameState = Cast<ACardGameState>(GWorld->GetWorld());
				CardGameState->GetCardPlacementReferences_C(ReturnValue, PlacementArray, Total);
				UPlacement_FunctionLibrary::GetIsValidPlacement(PlacementArray[0], Card_Ref->CardDataTableName, ECardSet::Empty, ValidPlacement);
				if (ValidPlacement)
				{
					GoalPlacement_Ref = PlacementArray[0];
					Temp_IgnoreCardArray_Ref_0.Add(Card_Ref);
					CardGameState->RemoveCardOnBoard_C(Card_Ref, Card_Ref->OwningPlayerID);
					Card_Ref->PlacementOwner->RemoveCardFromPlacement_C(Card_Ref);
					Card_Ref->OwningPlayerID = ReturnValue;
					Card_Ref->OnRep_OwningPlayerID_C();
					UCard_FunctionLibrary::SetCustomCardData(Card_Ref, true, 1);
					UCard_FunctionLibrary::AddCardToBoardPlacement(Card_Ref, GoalPlacement_Ref, ReturnValue);
				}
				else
				{
					if (UPlacement_FunctionLibrary::GetClosestAvailablePlacement(PlacementArray[0], Card_Ref->CardDataTableName, ECardSet::Empty, ReturnPlacement))
					{
						GoalPlacement_Ref = ReturnPlacement;
						Temp_IgnoreCardArray_Ref_0.Add(Card_Ref);
						CardGameState->RemoveCardOnBoard_C(Card_Ref, Card_Ref->OwningPlayerID);
						Card_Ref->PlacementOwner->RemoveCardFromPlacement_C(Card_Ref);
						Card_Ref->OwningPlayerID = ReturnValue;
						Card_Ref->OnRep_OwningPlayerID_C();
						UCard_FunctionLibrary::SetCustomCardData(Card_Ref, true, 1);
						UCard_FunctionLibrary::AddCardToBoardPlacement(Card_Ref, GoalPlacement_Ref, ReturnValue);
					}
				}
			}
		}
	}
}

void UCardAbility_FunctionLibrary::ChangeInHandCardOwner(ACard3D* CallingCard, int32 AbilityIndex)
{
	TArray<FName> CardsInHand_1;
	TArray<FName> CardsInHand_2;

	CallingCard->OwningPlayerID;
	FControllersState ControllersState = UController_FunctionLibrary::GetControllersStateProfile_ServerOnly(CallingCard->OwningPlayerID);
	CardsInHand_1 = ControllersState.CardsInHand;
	int32 Oppenent = UController_FunctionLibrary::GetOpponentID_ServerOnly(CallingCard->OwningPlayerID);
	FControllersState ControllersStateOppenent = UController_FunctionLibrary::GetControllersStateProfile_ServerOnly(Oppenent);
	CardsInHand_2 = ControllersStateOppenent.CardsInHand;

	AController* controller = UController_FunctionLibrary::GetControllerReferenceFromID_ServerOnly(CallingCard->OwningPlayerID, GWorld->GetWorld());

	ICardsInHand_Interface* CardsInHand_Interface = Cast<ICardsInHand_Interface>(controller);
	IController_Interface* Controller_Interface = Cast<IController_Interface>(controller);

	if (CardsInHand_Interface)
	{
		CardsInHand_Interface->Execute_RemoveCardFromHand_C(controller, "None", 0, true);
	}
	for (int32 i =0; i<CardsInHand_2.Num();i++)
	{
		Controller_Interface->Execute_AddCardToPlayersHand_C(controller, CardsInHand_2[i]);
	}

	AController* controller2 = UController_FunctionLibrary::GetControllerReferenceFromID_ServerOnly(Oppenent,GWorld->GetWorld());

	ICardsInHand_Interface* CardsInHand_Interface2 = Cast<ICardsInHand_Interface>(controller2);
	IController_Interface* Controller_Interface2 = Cast<IController_Interface>(controller2);

	if (CardsInHand_Interface2)
	{
		CardsInHand_Interface2->Execute_RemoveCardFromHand_C(controller2, "None", 0, true);
	}
	for (int32 i = 0; i < CardsInHand_1.Num(); i++)
	{
		Controller_Interface2->Execute_AddCardToPlayersHand_C(controller2, CardsInHand_1[i]);
	}
}
