// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "../Interface/CardsInHand_Interface.h"
#include "../Enums/CardGameEnums.h"
#include "CardAbility_FunctionLibrary.generated.h"

class ACard3D;
/**
 * ;
 */
UCLASS()
class CCGTOOLKIT_API UCardAbility_FunctionLibrary : public UBlueprintFunctionLibrary ,public ICardsInHand_Interface
{
	GENERATED_BODY()
	
public:
	
	static void DrawCards(ACard3D* CallingCard, int32 AbilityIndex);

	static void IncreaseAttack(ACard3D* CallingCard, int32 AbilityIndex);

	static void CloneCard(ACard3D* CallingCard, int32 AbilityIndex, bool RandomPlacement);

	static void IncreasePlayerHealth(ACard3D* CallingCard, int32 AbilityIndex);

	static void DealRetaliationDamage(ACard3D* CallingCard, int32 AbilityIndex);

	static void DamageAllCardsOnBoard(ACard3D* CallingCard, int32 AbilityIndex);

	static void SpawnRandomCardFromDeck(ACard3D* CallingCard, int32 AbilityIndex);

	static void GiveTurnPointstoAllActiveCards(ACard3D* CallingCard, int32 AbilityIndex);

	static void	IncreaseTurnPoints(ACard3D* CallingCard, int32 AbilityIndex);

	static void DiscardRandomCardsFromHand(ACard3D* CallingCard, int32 AbilityIndex);

	static void PickupCardsFromGraveyard(ACard3D* CallingCard, int32 AbilityIndex, bool AddDirectlyToBoard, ECardType CardType);

	static void ChangePlayedCardOwner(ACard3D* CallingCard, int32 AbilityIndex);

	static void ChangeInHandCardOwner(ACard3D* CallingCard, int32 AbilityIndex);
};
