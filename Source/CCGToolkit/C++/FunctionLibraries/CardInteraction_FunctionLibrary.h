// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "../GamePlay/Card3D.h"
#include "../GamePlay/BoardPlayer.h"
#include "../Enums/CardGameEnums.h"
#include "CardInteraction_FunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UCardInteraction_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static bool DealDamageToCard(bool SimpleDamage, ACard3D* TalkingCard, ACard3D* RecievingCard, int32 DamageOverride, EInteractionConditions& CallingCardCondition, EInteractionConditions& TalkingCardCondition);

	static bool DealDamageToPlayer(bool SimpleDamage, ACard3D* TalkingCard, ABoardPlayer* BoardPlayer, int32 DamageOverride, EInteractionConditions& InteractionConditionFailure);

	static bool ChargeInteractionPoints(ACard3D* CardToCharge, int32 Cost);

	static bool IsSameOwningPlayer(ACard3D* TalkingCard, ACard3D* RecievingCard);

	static bool ValidateCardInteraction(ACard3D* TalkingCard, ACard3D* RecievingCard, EInteractionConditions& CallingCardConditionFailure, EInteractionConditions& TalkingCardConditionFailure);

	static bool ValidatePlayerInteraction(ACard3D* TalkingCard, ABoardPlayer* RecievingBoardPlayer, EInteractionConditions& ConditionFailure);

	static bool ValidateCardInteractionConditions(ACard3D* TalkingCard, ACard3D* RecievingCard, bool RecievingCardConditions, EInteractionConditions& ConditionFailure);
};
