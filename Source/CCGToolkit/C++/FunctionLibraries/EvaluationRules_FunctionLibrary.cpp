// Fill out your copyright notice in the Description page of Project Settings.


#include "EvaluationRules_FunctionLibrary.h"
#include "../GamePlay/Card3D.h"
#include "Kismet/KismetSystemLibrary.h"

int32 UEvaluationRules_FunctionLibrary::RuleCalculateActiveCardPoints(int32 Add, int32 PtsPerActiveCard, TArray<ACard3D*> CardsActive, bool IncludePlayValue, int32& RuleTotal)
{
	int32 TEMP_CardValueTotal = 0;

	if (!IncludePlayValue)
	{
		RuleTotal = PtsPerActiveCard* CardsActive.Num();
		return Add + CardsActive.Num()* PtsPerActiveCard;
	}
	else
	{
		for (int32 i = 0; i < CardsActive.Num(); i++)
		{
			TEMP_CardValueTotal += CardsActive[i]->CardDataStruct.Card_System_Data.Card_Value;
		}
		RuleTotal = PtsPerActiveCard* CardsActive.Num() + Add + TEMP_CardValueTotal;
		return PtsPerActiveCard * CardsActive.Num() + Add + TEMP_CardValueTotal;
	}
}

int32 UEvaluationRules_FunctionLibrary::RuleCalculateCardsInHandPoints(int32 Add, int32 PtsPerCardnHand, int32 CardsInHand, int32& RuleTotal)
{
	RuleTotal = PtsPerCardnHand* CardsInHand;
	return Add + PtsPerCardnHand* CardsInHand;
}

int32 UEvaluationRules_FunctionLibrary::RuleCalculateHealthPoints(int32 Add, int32 PtsPerHealthPoint, int32 PlayerHealth, int32& RuleTotal)
{
	RuleTotal = PtsPerHealthPoint* PlayerHealth;
	return Add + PtsPerHealthPoint* PlayerHealth;
}

bool UEvaluationRules_FunctionLibrary::RuleCalculateAttackPoints(int32 Add, int32 PtsForDamage, int32 PtsForRemovingCardFromPlay, int32 Attack, int32 Health, int32 CardValue, bool IncludeCardValue, int32& Total, int32& RuleTotal)
{
	Total = Add + ((Health - Attack) <= 0 ? PtsForRemovingCardFromPlay : PtsForDamage) + ((IncludeCardValue) ? CardValue : 0) - ((Health - Attack < 0) ? FMath::Abs(Health - Attack) : 0);
	RuleTotal = ((Health - Attack) <= 0 ? PtsForRemovingCardFromPlay : PtsForDamage) + ((IncludeCardValue) ? CardValue : 0) - ((Health - Attack < 0) ? FMath::Abs(Health - Attack) : 0);
	return Health - Attack <= 0;
}

int32 UEvaluationRules_FunctionLibrary::RuleCalculateAdditionalAbilityPointOffset(ACard3D* TalkingCard, ACard3D* RecievingCard)
{
	int32 ReturnPoints = 0;
	for (int32 i = 0; i < RecievingCard->CardDataStruct.Abilities.Num(); i++)
	{
		switch (RecievingCard->CardDataStruct.Abilities[i].Type)
		{
		case EAbilityType::None:
			break;
		case EAbilityType::DrawCards:
			break;
		case EAbilityType::IncreaseAttack:
			switch (RecievingCard->CardDataStruct.Abilities[i].Trigger)
			{
			case EAbilityTrigger::None:
				break;
			case EAbilityTrigger::OnDrop:
				break;
			case EAbilityTrigger::StartOfTurn:
				break;
			case EAbilityTrigger::EndOfTurn:
				break;
			case EAbilityTrigger::TakesDamage:
				ReturnPoints = ReturnPoints - (UKismetSystemLibrary::MakeLiteralInt(1)* RecievingCard->CardDataStruct.Abilities[i].Ability_Int);
				break;
			case EAbilityTrigger::SentToGraveyard:
				break;
			case EAbilityTrigger::DeathByLifeExpectancy:
				break;
			default:
				break;
			}
			break;
		case EAbilityType::Clone:
			break;
		case EAbilityType::IncreasePlayerHealth:
			break;
		case EAbilityType::RetaliationDamage:
			ReturnPoints = ReturnPoints - (UKismetSystemLibrary::MakeLiteralInt(1)* RecievingCard->CardDataStruct.Abilities[i].Ability_Int);
			break;
		case EAbilityType::DamageAllCardsOnBoard:
			switch (RecievingCard->CardDataStruct.Abilities[i].Trigger)
			{
			case EAbilityTrigger::None:
				break;
			case EAbilityTrigger::OnDrop:
				break;
			case EAbilityTrigger::StartOfTurn:
				ReturnPoints = (ReturnPoints + 5)* RecievingCard->CardDataStruct.Abilities[i].Ability_Int;
				break;
			case EAbilityTrigger::EndOfTurn:
				break;
			case EAbilityTrigger::TakesDamage:
				break;
			case EAbilityTrigger::SentToGraveyard:
				break;
			case EAbilityTrigger::DeathByLifeExpectancy:
				ReturnPoints = (ReturnPoints + 5) * RecievingCard->CardDataStruct.Abilities[i].Ability_Int;
				break;
			default:
				break;
			}
			break;
		case EAbilityType::SpawnRandomCardFromDeck:
			break;
		case EAbilityType::GiveTurnPointsToAllActiveCards:
			break;
		case EAbilityType::IncreaseTurnPoints:
			break;
		case EAbilityType::DiscardCardsInHand:
			break;
		case EAbilityType::PickupFirstCreatureInTheGraveyardToBoard:
			break;
		case EAbilityType::PickupFirstCreatureInTheGraveyardToHand:
			break;
		case EAbilityType::PossessOpponentsCard:
			break;
		case EAbilityType::SwapCardsInHand:
			break;
		default:
			break;
		}
	}
	return ReturnPoints;
}

bool UEvaluationRules_FunctionLibrary::RuleCalculateDamagetoPlayerPoints(int32 Add, int32 PtsForPlayerDamage, int32 PtsForRemovingPlayer, int32 CardAttack, int32 PlayerHealth, int32& Total, int32& RuleTotal)
{
	Total = Add + ((PlayerHealth - CardAttack <= 0)? PtsForRemovingPlayer : PtsForPlayerDamage* CardAttack);
	RuleTotal = ((PlayerHealth - CardAttack <= 0) ? PtsForRemovingPlayer : PtsForPlayerDamage * CardAttack);
	return PlayerHealth - CardAttack <= 0;
}

bool UEvaluationRules_FunctionLibrary::RuleCalculateOwnedCardAbilityPointOffset(ACard3D* TalkingCard, ACard3D* RecievingCard, int32& Points)
{
	int32 ReturnPoints = 0;
	for (int32 i = 0; i < RecievingCard->CardDataStruct.Abilities.Num(); i++)
	{
		switch (RecievingCard->CardDataStruct.Abilities[i].Type)
		{
		case EAbilityType::None:
			break;
		case EAbilityType::DrawCards:
			break;
		case EAbilityType::IncreaseAttack:
			switch (RecievingCard->CardDataStruct.Abilities[i].Trigger)
			{
			case EAbilityTrigger::None:
				break;
			case EAbilityTrigger::OnDrop:
				break;
			case EAbilityTrigger::StartOfTurn:
				break;
			case EAbilityTrigger::EndOfTurn:
				break;
			case EAbilityTrigger::TakesDamage:
				ReturnPoints += (RecievingCard->CardDataStruct.Health.Health - TalkingCard->CardDataStruct.Attack.Damage <= 0)? -99 : 5;
				break;
			case EAbilityTrigger::SentToGraveyard:
				break;
			case EAbilityTrigger::DeathByLifeExpectancy:
				break;
			default:
				break;
			}
			break;
		case EAbilityType::Clone:
			break;
		case EAbilityType::IncreasePlayerHealth:
			break;
		case EAbilityType::RetaliationDamage:
			ReturnPoints -= UKismetSystemLibrary::MakeLiteralInt(10)* RecievingCard->CardDataStruct.Abilities[i].Ability_Int;
			break;
		case EAbilityType::DamageAllCardsOnBoard:
			switch (RecievingCard->CardDataStruct.Abilities[i].Trigger)
			{
			case EAbilityTrigger::None:
				break;
			case EAbilityTrigger::OnDrop:
				break;
			case EAbilityTrigger::StartOfTurn:
				ReturnPoints = (ReturnPoints + 5)* RecievingCard->CardDataStruct.Abilities[i].Ability_Int;
				break;
			case EAbilityTrigger::EndOfTurn:
				break;
			case EAbilityTrigger::TakesDamage:
				break;
			case EAbilityTrigger::SentToGraveyard:
				break;
			case EAbilityTrigger::DeathByLifeExpectancy:
				ReturnPoints = (ReturnPoints + 5) * RecievingCard->CardDataStruct.Abilities[i].Ability_Int;
				break;
			default:
				break;
			}
			break;
		case EAbilityType::SpawnRandomCardFromDeck:
			break;
		case EAbilityType::GiveTurnPointsToAllActiveCards:
			break;
		case EAbilityType::IncreaseTurnPoints:
			break;
		case EAbilityType::DiscardCardsInHand:
			break;
		case EAbilityType::PickupFirstCreatureInTheGraveyardToBoard:
			break;
		case EAbilityType::PickupFirstCreatureInTheGraveyardToHand:
			break;
		case EAbilityType::PossessOpponentsCard:
			break;
		case EAbilityType::SwapCardsInHand:
			break;
		default:
			break;
		}
	}
	Points = ReturnPoints;
	return ReturnPoints >= 0;
}
