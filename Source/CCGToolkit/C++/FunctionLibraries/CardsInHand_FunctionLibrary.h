// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/Controller.h"
#include "../Enums/CardGameEnums.h"
#include "CardsInHand_FunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UCardsInHand_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static int32 CountCardsInHand(TArray<FName> CardsInHand);

	static FName GetCardInHand(TArray<FName> CardsInHand, int32 Index, bool LastIndex);

	static bool HasCardInHand(TArray<FName> CardsInHand, FName Card, int32 Index);

	static TArray<FName> FindPlayableCardsInHand(AController* Controller);

	static TArray<FName> SortCardsByFocus(TArray<FName> CardsArray, TArray<EAIPersonalityFocus> FocusArray, bool IncludeOrderByValue);
};
