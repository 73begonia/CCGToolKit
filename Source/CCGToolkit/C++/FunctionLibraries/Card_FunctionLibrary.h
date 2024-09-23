// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CCGToolkit/C++/GamePlay/Card3D.h"
#include "CCGToolkit/C++/Enums/CardGameEnums.h"
#include "Card_FunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UCard_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static ACard3D* SetupCard(ACard3D* CardRef3D, int32 AbilityIndex, FName CardName, ECardSet CardSet, FSCard CardStruct, bool SetCardDataFromStruct);

	static bool AddCardToBoardPlacement(ACard3D* AddCard, ACardPlacement* GoalPlacement, int32 OwningPlayerID);

	static void LoadPreBuiltDeck(FName RowName, TArray<FName>& ReturnDeck);

	static void SetCustomCardData(ACard3D* Card3D, bool AbilityActive, int32 TurnPoints);

	static bool GetCardReferenceFromID(int32 CardID, ACard3D*& ReturnCard);

	static TArray<ACard3D*> FilterCardsWithTurnPoints(TArray<ACard3D*> CardArray, TArray<ECardType> ValidCardTypes);

	static bool ChargeCardManaCost(int32, bool SkipManaCheck, FSCard Card_Struct, EManaAccount AccountToCharge, int32& Cost);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool ValidateCardPlayConditions(FName CardName, AController* CallingController, ECardPlayConditions& ConditionFailure);

	static bool HasAbility(ACard3D* Card, EAbilityType Ability, EAbilityTrigger Trigger);
};
