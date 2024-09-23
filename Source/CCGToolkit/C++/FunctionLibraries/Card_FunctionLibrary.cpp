// Fill out your copyright notice in the Description page of Project Settings.


#include "Card_FunctionLibrary.h"
#include "../CardGameState.h"
#include "../../CardGame/Structs/Structs.h"
#include "Controller_FunctionLibrary.h"
#include "../Interface/PlayerState_Interface.h"
#include "Deck_FunctionLibrary.h"
#include "CardConditions_FunctionLibrary.h"

ACard3D* UCard_FunctionLibrary::SetupCard(ACard3D* CardRef3D, int32 OwningPlayerID, FName CardName, ECardSet CardSet, FSCard CardStruct, bool SetCardDataFromStruct)
{
	CardRef3D->OwningPlayerID = OwningPlayerID;
	CardRef3D->OnRep_OwningPlayerID_C();

	CardRef3D->SetReplicates(true);

	CardRef3D->Server_SetCardData_C(CardName, CardSet, CardStruct, SetCardDataFromStruct);

	return CardRef3D;
}

bool UCard_FunctionLibrary::AddCardToBoardPlacement(ACard3D* AddCard, ACardPlacement* GoalPlacement, int32 OwningPlayerID)
{
	GoalPlacement->AddCardToPlacement_C(AddCard);
	AddCard->PlacementOwner = GoalPlacement;

	ACardGameState* CardGameState = Cast<ACardGameState>(GWorld->GetGameState());
	if (CardGameState != nullptr)
	{
		CardGameState->AddCardToBoard_C(AddCard, OwningPlayerID);
		return true;
	}
	else
	{
		return false;
	}
}

void UCard_FunctionLibrary::LoadPreBuiltDeck(FName RowName, TArray<FName>& ReturnDeck)
{
	FString ContextString;
	UDataTable* PreBuildDecksTableInfo;
	PreBuildDecksTableInfo = LoadObject<UDataTable>(NULL, TEXT("DataTable'/Game/CCGToolkitC++/CardGame/DataTable/PreBuildDecks_C_DataTable.PreBuildDecks_C_DataTable'"));
	if (PreBuildDecksTableInfo)
	{
		FSPrebuiltDecks* pRow = PreBuildDecksTableInfo->FindRow<FSPrebuiltDecks>(RowName, ContextString);
		if (pRow)
		{
			for (int32 i = 0; i < pRow->CardsInDeck.Num(); i++)
			{
				for (int32 j = 1; j <= pRow->CardsInDeck[i].Quantity; j++)
				{
					ReturnDeck.Add(pRow->CardsInDeck[i].Card_Table_Name);
				}
			}
		}
		else
		{
			ReturnDeck = { "None" };
		}
	}
	else
	{
		ReturnDeck = {"None"};
	}
}

void UCard_FunctionLibrary::SetCustomCardData(ACard3D* Card3D, bool AbilityActive, int32 TurnPoints)
{
	Card3D->IsAbilityActive = AbilityActive;
	if (TurnPoints > 0)
	{
		Card3D->TurnPoints = 1;
	}
}

bool UCard_FunctionLibrary::GetCardReferenceFromID(int32 CardID, ACard3D*& ReturnCard)
{
	ACardGameState* CardGameState = Cast<ACardGameState>(GWorld->GetGameState());
	if (!CardGameState)
	{
		ReturnCard = nullptr;
		return false;
	}
	if (CardGameState->CardReferenceArray[CardID])
	{
		ReturnCard = CardGameState->CardReferenceArray[CardID];
		return true;
	}
	else
	{
		ReturnCard = CardGameState->CardReferenceArray[CardID];
		return false;
	}
}

TArray<ACard3D*> UCard_FunctionLibrary::FilterCardsWithTurnPoints(TArray<ACard3D*> CardArray, TArray<ECardType> ValidCardTypes)
{
	TArray<ACard3D*> TEMP_ReturnCardArray;
	for (int32 i = 0; i < CardArray.Num(); i++)
	{
		if (CardArray[i]->TurnPoints > 0 && ValidCardTypes.Contains(CardArray[i]->Type))
		{
			TEMP_ReturnCardArray.Add(CardArray[i]);
		}
	}
	return TEMP_ReturnCardArray;
}

bool UCard_FunctionLibrary::ChargeCardManaCost(int32 playerID, bool SkipManaCheck, FSCard Card_Struct, EManaAccount AccountToCharge, int32& Cost)
{
	switch (AccountToCharge)
	{
	case EManaAccount::None:
		return 0;
		break;
	case EManaAccount::Placement:
		Cost = Card_Struct.Placement_Settings.Mana_Cost;
		break;
	case EManaAccount::Attack:
		Cost = Card_Struct.Attack.Mana_Cost;
		break;
	default:
		Cost = 0;
		break;
	}

	if (SkipManaCheck)
	{
		return true;
	}
	else
	{
		FControllersState ControllersState = UController_FunctionLibrary::GetControllersStateProfile_ServerOnly(playerID);
		if (ControllersState.Mana >= Cost)
		{
			IPlayerState_Interface* TheInterface = Cast<IPlayerState_Interface>(ControllersState.PlayerState);
			if (TheInterface)
			{
				TheInterface->Execute_DecreasePlayerMana_C(ControllersState.PlayerState, Cost);
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
		return false;
		}
	}
}

bool UCard_FunctionLibrary::ValidateCardPlayConditions(FName CardName, AController* CallingController, ECardPlayConditions& ConditionFailure)
{
	bool Valid;
	FSCard CardStruct;
	AController* Controller_Ref;
	ECardPlayConditions CardPlayConditon;
	Controller_Ref = CallingController;

	CardPlayConditon = ECardPlayConditions::None;

	UDeck_FunctionLibrary::GetCardData(CardName, ECardSet::Empty, CardStruct);
	int32 ID = UController_FunctionLibrary::GetControllerID_ServerOnly(Controller_Ref);

	for (int i = 0; i < CardStruct.Card_System_Data.PlayConditions.Num(); i++)
	{
		CardPlayConditon = CardStruct.Card_System_Data.PlayConditions[i];
		switch (CardPlayConditon)
		{
		case ECardPlayConditions::None:
			break;
		case ECardPlayConditions::PlayAtStartOfTurnAIOnly:
			if (!UCardConditions_FunctionLibrary::ConditionIfStartOfPlay_AIOnly(ID))
			{
				Valid = false;
			}
			break;
		case ECardPlayConditions::PlayAtEndOfTurnAIOnly:
			if (!UCardConditions_FunctionLibrary::ConditionIfEndOfPlay_AIOnly(ID))
			{
				Valid = false;
			}
			break;
		case ECardPlayConditions::PlayWhenLowHealthSelf:
			if (!UCardConditions_FunctionLibrary::ConditionIfPlayerLowHealth(ID))
			{
				Valid = false;
			}
			break;
		case ECardPlayConditions::PlayWhenHighHealthSelf:
			if (!UCardConditions_FunctionLibrary::ConditionIfPlayerHighHealth(ID))
			{
				Valid = false;
			}
			break;
		case ECardPlayConditions::PlayWhenCreaturesAreOnTheBoardSelf:
			if (!UCardConditions_FunctionLibrary::ConditionIfCardTypeInPlay(Controller_Ref, 2, false, ECardType::Creature, false))
			{
				Valid = false;
			}
			break;
		case ECardPlayConditions::PlayWhenCreaturesAreOnTheBoardSelfAIOnly:
			if (!UCardConditions_FunctionLibrary::ConditionIfCardTypeInPlay(Controller_Ref, 2, false, ECardType::Creature, true))
			{
				Valid = false;
			}
			break;
		case ECardPlayConditions::PlayWhenCreaturesAreOnTheBoardOpponent:
			if (!UCardConditions_FunctionLibrary::ConditionIfCardTypeInPlay(Controller_Ref, 2, true, ECardType::Creature, false))
			{
				Valid = false;
			}
			break;
		case ECardPlayConditions::PlayWhenCreaturesAreOnTheBoardOpponentAIOnly:
			if (!UCardConditions_FunctionLibrary::ConditionIfCardTypeInPlay(Controller_Ref, 2, true, ECardType::Creature, true))
			{
				Valid = false;
			}
			break;
		case ECardPlayConditions::PlayWhenThereAreCardsInTheGraveyard:
			if (!UCardConditions_FunctionLibrary::ConditionIfPlayerHasCardsInGraveyard(Controller_Ref, false, 5))
			{
				Valid = false;
			}
			break;
		case ECardPlayConditions::PlayWhenThereAreCardsInTheGraveyardAIOnly:
			if (!UCardConditions_FunctionLibrary::ConditionIfPlayerHasCardsInGraveyard(Controller_Ref, true, 5))
			{
				Valid = false;
			}
			break;
		case ECardPlayConditions::PlayWhenLowPlayableCardInHandAIOnly:
			if (!UCardConditions_FunctionLibrary::ConditionIfLowPlayableCardsInHand_AIOnly(ID, 2))
			{
				Valid = false;
			}
			break;
		default:
			break;
		}
		break;
	}
	Valid = true;
	ConditionFailure = CardPlayConditon;
	return Valid;
}

bool UCard_FunctionLibrary::HasAbility(ACard3D* Card, EAbilityType Ability, EAbilityTrigger Trigger)
{
	for (int32 i = 0; i < Card->CardDataStruct.Abilities.Num(); i++)
	{
		if (Card->CardDataStruct.Abilities[i].Type == Ability)
		{
			if (Card->CardDataStruct.Abilities[i].Trigger == Trigger || Trigger != EAbilityTrigger::None)
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
			return false;
		}
	}
	return false;
}
