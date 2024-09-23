// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/Controller.h"
#include "../Enums/CardGameEnums.h"
#include "../GamePlay/Card3D.h"
#include "CardConditions_FunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UCardConditions_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static bool ConditionIfCardTypeInPlay(AController* Controller, int32 MinCreaturesOnBoard, bool bOpponent, ECardType CardType, bool AI_Only);

	static bool ConditionIfPlayerLowHealth(int32 PlayerID);

	static bool ConditionIfPlayerHighHealth(int32 PlayerID);

	static bool ConditionIfStartOfPlay_AIOnly(int32 PlayerID);

	static bool ConditionIfEndOfPlay_AIOnly(int32 PlayerID);

	static bool ConditionIfPlacedOnOwnedPlacement(int32 PlayerID, ACard3D* RecievingCard, bool Oppenent);

	static bool ConditionIfRecievingCardType(ACard3D* RecievingCard, ECardType RecievingCardType);

	static bool ConditionIsNotOwningPlayer(int32 PlayerID, ACard3D* RecievingCard);

	static bool ConditionIfPlayerHasCardsInGraveyard(AController* Controller, bool AIOnly, int32 MinCardsInGraveyard);

	static bool ConditionIfLowPlayableCardsInHand_AIOnly(int32 PlayerID, int32 LowValue);
};
