// Fill out your copyright notice in the Description page of Project Settings.


#include "CardInteraction_FunctionLibrary.h"
#include "Misc_FunctionLibrary.h"
#include "../Enums/CardGameEnums.h"
#include "../../CardGame/Structs/Structs.h"
#include "Controller_FunctionLibrary.h"
#include "CardConditions_FunctionLibrary.h"
#include "../GamePlay/Card3D.h"

bool UCardInteraction_FunctionLibrary::DealDamageToCard(bool SimpleDamage, ACard3D* TalkingCard, ACard3D* RecievingCard, int32 DamageOverride, EInteractionConditions& CallingCardCondition, EInteractionConditions& TalkingCardCondition)
{
	ACard3D* InteractionTalkingCard = TalkingCard;
	ACard3D* InteracitionRecievingCard = RecievingCard;
	EInteractionConditions CallingCardConditionFailure = EInteractionConditions::None;
	EInteractionConditions TalkingCardConditionFailure = EInteractionConditions::None;
	int32 ReturnValue;
	FText CardName;
	int32 Attack, Health;

	if (DamageOverride > 0)
	{
		ReturnValue = DamageOverride;
	}
	else
	{
		UMisc_FunctionLibrary::ReadInteractingCardData(InteractionTalkingCard, CardName, Attack, Health);
		ReturnValue = Attack;
	}

	if (SimpleDamage)
	{
		ICardInteraction_Interface* CardInteraction_Interface = Cast<ICardInteraction_Interface>(InteracitionRecievingCard);
		if (CardInteraction_Interface)
		{
			CardInteraction_Interface->Execute_TakesDamage_C(InteracitionRecievingCard,InteractionTalkingCard, 0, ReturnValue);
			ACardGameState* CardGameState = Cast<ACardGameState>(GWorld->GetGameState());
			if (CardGameState)
			{
				FSBattleHistory BattleHistory;
				BattleHistory.OwningControllerID = InteractionTalkingCard->OwningPlayerID;
				BattleHistory.Interaction_Type = EPlayType::AttackCard;
				BattleHistory.TalkingCard = InteractionTalkingCard->CardDataStruct;
				BattleHistory.RecievingCard = InteracitionRecievingCard->CardDataStruct;
				BattleHistory.OR_RecievingPlayerID = 0;

				CardGameState->RecordBattleHistory_C(BattleHistory);
				CallingCardCondition = CallingCardConditionFailure;
				TalkingCardCondition = TalkingCardConditionFailure;
				return true;
			}
			else
			{
				CallingCardCondition = CallingCardConditionFailure;
				TalkingCardCondition = TalkingCardConditionFailure;
				return false;
			}
		}
	}
	else
	{
		if (UCardInteraction_FunctionLibrary::ValidateCardInteraction(InteractionTalkingCard, InteracitionRecievingCard, CallingCardConditionFailure, TalkingCardConditionFailure))
		{
			if (UCardInteraction_FunctionLibrary::ChargeInteractionPoints(InteractionTalkingCard, 1))
			{
				InteractionTalkingCard->Multicast_Attacking_C(InteracitionRecievingCard);
				ICardInteraction_Interface* CardInteraction_Interface = Cast<ICardInteraction_Interface>(InteracitionRecievingCard);
				if (CardInteraction_Interface)
				{
					CardInteraction_Interface->Execute_TakesDamage_C(InteracitionRecievingCard, InteractionTalkingCard, 0, ReturnValue);
					ACardGameState* CardGameState = Cast<ACardGameState>(GWorld->GetGameState());
					if (CardGameState)
					{
						FSBattleHistory BattleHistory;
						BattleHistory.OwningControllerID = InteractionTalkingCard->OwningPlayerID;
						BattleHistory.Interaction_Type = EPlayType::AttackCard;
						BattleHistory.TalkingCard = InteractionTalkingCard->CardDataStruct;
						BattleHistory.RecievingCard = InteracitionRecievingCard->CardDataStruct;
						BattleHistory.OR_RecievingPlayerID = 0;

						CardGameState->RecordBattleHistory_C(BattleHistory);
						CallingCardCondition = CallingCardConditionFailure;
						TalkingCardCondition = TalkingCardConditionFailure;
						return true;
					}
					else
					{
						CallingCardCondition = CallingCardConditionFailure;
						TalkingCardCondition = TalkingCardConditionFailure;
						return false;
					}
				}
			}
			else
			{
				CallingCardCondition = CallingCardConditionFailure;
				TalkingCardCondition = TalkingCardConditionFailure;
				return false;
			}
		}
		else
		{
			UCardInteraction_FunctionLibrary::ValidateCardInteraction(InteractionTalkingCard, InteracitionRecievingCard, CallingCardConditionFailure, TalkingCardConditionFailure);
			CallingCardCondition = CallingCardConditionFailure;
			TalkingCardCondition = TalkingCardConditionFailure;
			return false;
		}
	}
	return false;
}

bool UCardInteraction_FunctionLibrary::DealDamageToPlayer(bool SimpleDamage, ACard3D* TalkingCard, ABoardPlayer* BoardPlayer, int32 DamageOverride, EInteractionConditions& InteractionConditionFailure)
{
	ACard3D* InteractionTalkingCard;
	ABoardPlayer* InteractionRecievingPlayer;
	EInteractionConditions ConditionFailure = EInteractionConditions::None;
	int32 ReturnValue = 0;
	FText CardName;
	int32 Attack, Health;
	FSCard RecievingCard;

	InteractionTalkingCard = TalkingCard;
	InteractionRecievingPlayer = BoardPlayer;

	if (DamageOverride > 0)
	{
		ReturnValue = DamageOverride;
	}
	else
	{
		UMisc_FunctionLibrary::ReadInteractingCardData(InteractionTalkingCard, CardName, Attack, Health);
		ReturnValue = Attack;
	}

	if (SimpleDamage)
	{		
		AActor* ReplicatedPlayerState = UController_FunctionLibrary::GetControllerPlayerState_ServerOnly(InteractionRecievingPlayer->PlayerIndex);
		IPlayerState_Interface* PlayerState_Interface = Cast<IPlayerState_Interface>(ReplicatedPlayerState);
		if (PlayerState_Interface)
		{
			PlayerState_Interface->Execute_DecreasePlayerHealth_C(ReplicatedPlayerState, ReturnValue);
			ACardGameState* CardGameState = Cast<ACardGameState>(GWorld->GetGameState());
			if (CardGameState)
			{
				FSBattleHistory BattleHistory;
				BattleHistory.OwningControllerID = InteractionTalkingCard->OwningPlayerID;
				BattleHistory.Interaction_Type = EPlayType::AttackPlayer;
				BattleHistory.TalkingCard = InteractionTalkingCard->CardDataStruct;
				BattleHistory.OR_RecievingPlayerID = InteractionRecievingPlayer->PlayerIndex;

				CardGameState->RecordBattleHistory_C(BattleHistory);
				InteractionConditionFailure = ConditionFailure;
				return true;
			}
			else
			{
				InteractionConditionFailure = ConditionFailure;
				return false;
			}
		}
	}
	else
	{
		if (UCardInteraction_FunctionLibrary::ValidatePlayerInteraction(InteractionTalkingCard, InteractionRecievingPlayer, ConditionFailure))
		{
			if (UCardInteraction_FunctionLibrary::ChargeInteractionPoints(InteractionTalkingCard, 1))
			{
				InteractionTalkingCard->Multicast_Attacking_C(InteractionRecievingPlayer);
				AActor* ReplicatedPlayerState = UController_FunctionLibrary::GetControllerPlayerState_ServerOnly(InteractionRecievingPlayer->PlayerIndex);
				IPlayerState_Interface* PlayerState_Interface = Cast<IPlayerState_Interface>(ReplicatedPlayerState);
				if (PlayerState_Interface)
				{
					PlayerState_Interface->Execute_DecreasePlayerHealth_C(ReplicatedPlayerState, ReturnValue);
					ACardGameState* CardGameState = Cast<ACardGameState>(GWorld->GetGameState());
					if (CardGameState)
					{
						FSBattleHistory BattleHistory;
						BattleHistory.OwningControllerID = InteractionTalkingCard->OwningPlayerID;
						BattleHistory.Interaction_Type = EPlayType::AttackPlayer;
						BattleHistory.TalkingCard = InteractionTalkingCard->CardDataStruct;
						BattleHistory.OR_RecievingPlayerID = InteractionRecievingPlayer->PlayerIndex;

						CardGameState->RecordBattleHistory_C(BattleHistory);
						InteractionConditionFailure = ConditionFailure;
						return true;
					}
					else
					{
						InteractionConditionFailure = ConditionFailure;
						return false;
					}
				}
			}
			else
			{
				InteractionConditionFailure = ConditionFailure;
				return false;
			}
		}
		else
		{
			UCardInteraction_FunctionLibrary::ValidatePlayerInteraction(InteractionTalkingCard, InteractionRecievingPlayer, ConditionFailure);
			InteractionConditionFailure = ConditionFailure;
			return false;
		}
	}
	return false;
}

bool UCardInteraction_FunctionLibrary::ChargeInteractionPoints(ACard3D* CardToCharge, int32 Cost)
{
	if (CardToCharge->TurnPoints >= 1)
	{
		CardToCharge->TurnPoints -= Cost;
		return true;
	}
	else
	{
		return false;
	}
}

bool UCardInteraction_FunctionLibrary::IsSameOwningPlayer(ACard3D* TalkingCard, ACard3D* RecievingCard)
{
	return TalkingCard->OwningPlayerID == RecievingCard->OwningPlayerID;
}

bool UCardInteraction_FunctionLibrary::ValidateCardInteraction(ACard3D* TalkingCard, ACard3D* RecievingCard, EInteractionConditions& CallingCardConditionFailure, EInteractionConditions& TalkingCardConditionFailure)
{
	EInteractionConditions CardConditionFailure1;
	EInteractionConditions CardConditionFailure2;

	bool Valid1 = UCardInteraction_FunctionLibrary::ValidateCardInteractionConditions(TalkingCard, RecievingCard, false, CardConditionFailure1);
	bool Valid2 = UCardInteraction_FunctionLibrary::ValidateCardInteractionConditions(TalkingCard, RecievingCard, true, CardConditionFailure2);
	CallingCardConditionFailure = CardConditionFailure1;
	CallingCardConditionFailure = CardConditionFailure2;
	return ((Valid1 && Valid2) && (RecievingCard->Health>0 && !RecievingCard->bInGraveyard) && (TalkingCard->TurnPoints>0 && TalkingCard->Attack>0 && !TalkingCard->bInGraveyard && TalkingCard->CardDataStruct.Attack.CanAttackCards));
}

bool UCardInteraction_FunctionLibrary::ValidatePlayerInteraction(ACard3D* TalkingCard, ABoardPlayer* RecievingBoardPlayer, EInteractionConditions& ConditionFailure)
{
	EInteractionConditions ReturnConditionFailure;
	bool Valid = UCardInteraction_FunctionLibrary::ValidateCardInteractionConditions(TalkingCard, nullptr, false, ReturnConditionFailure);
	ConditionFailure = ReturnConditionFailure;

	FControllersState ControllersState = UController_FunctionLibrary::GetControllersStateProfile_ServerOnly(RecievingBoardPlayer->PlayerIndex);

	return (Valid && ControllersState.Health>0 && (TalkingCard->TurnPoints>0 && TalkingCard->Attack>0 && !TalkingCard->bInGraveyard && TalkingCard->CardDataStruct.Attack.CanAttackPlayer));
}

bool UCardInteraction_FunctionLibrary::ValidateCardInteractionConditions(ACard3D* TalkingCard, ACard3D* RecievingCard, bool RecievingCardConditions, EInteractionConditions& ConditionFailure)
{
	ACard3D* RecievingCard_Ref = nullptr;
	ACard3D* TalkingCard_Ref;
	bool Valid;
	EInteractionConditions InteractionCondition = EInteractionConditions::None;
	TArray<EInteractionConditions> ReturnValue = {};

	TalkingCard_Ref = TalkingCard;
	if (RecievingCard)
	{
		RecievingCard_Ref = RecievingCard;
	}
	Valid = true;

	if (RecievingCardConditions == true)
	{
		ReturnValue = RecievingCard_Ref->CardDataStruct.Card_System_Data.InteractionConditions;
	}
	else
	{
		ReturnValue = TalkingCard_Ref->CardDataStruct.Card_System_Data.InteractionConditions;
	}

	for (int32 i = 0; i < ReturnValue.Num(); i++)
	{
		InteractionCondition = ReturnValue[i];
		switch (ReturnValue[i])
		{
		case EInteractionConditions::None:
			break;
		case EInteractionConditions::CardCanOnlyRecieveInteractionOnOwnedPlacement:
			if (!UCardConditions_FunctionLibrary::ConditionIfPlacedOnOwnedPlacement(TalkingCard_Ref->OwningPlayerID, RecievingCard_Ref, false))
			{
				Valid = false;
			}
			break;
		case EInteractionConditions::CardCanOnlyRecieveInteractionOnAnOppenentsPlacement:
			if (!UCardConditions_FunctionLibrary::ConditionIfPlacedOnOwnedPlacement(TalkingCard_Ref->OwningPlayerID, RecievingCard_Ref, true))
			{
				Valid = false;
			}
			break;
		case EInteractionConditions::CanOnlyAttackCreatures:
			if (!UCardConditions_FunctionLibrary::ConditionIfRecievingCardType(RecievingCard_Ref, ECardType::Creature))
			{
				Valid = false;
			}
			break;
		case EInteractionConditions::CannotAttackIfPlayerOwnsCard:
			if (!UCardConditions_FunctionLibrary::ConditionIsNotOwningPlayer(TalkingCard_Ref->OwningPlayerID, RecievingCard_Ref))
			{
				Valid = false;
			}
			break;
		case EInteractionConditions::CanRecieveSelfInteractionAIOnly:
			break;
		case EInteractionConditions::CanRecieveSelfInteractionIfDoesNotRemoveCardFromPlayAIOnly:
			break;
		default:
			break;
		}
		break;
	}
	if (Valid)
	{
		InteractionCondition = EInteractionConditions::None;
	}
	ConditionFailure = InteractionCondition;
	return Valid;
}
