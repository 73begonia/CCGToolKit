// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CCGToolkit/C++/Enums/CardGameEnums.h"
#include "CCGToolkit/CardGame/Structs/Structs.h"
#include "Engine/DataTable.h"
#include "Deck_FunctionLibrary.generated.h"

/**
 * 
 */
 DECLARE_LOG_CATEGORY_CLASS(Deck_FunctionLibraryLog, Log, All);

UCLASS()
class CCGTOOLKIT_API UDeck_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static TArray<FName> GetChosenDeckArray(int32 Index, TArray<UDataTable*> TargetArray);

	UFUNCTION(BlueprintCallable)
	static void GetCardData(FName CardName,ECardSet CardSet,FSCard& CardStruct);

	static int32 GetRandomCardFromDeck(AController* Controller, FName& Card, ECardSet& CardRace);

	static FName GetCardFromDeck(TArray<FName> Deck, int32 Index, bool LastIndex);

	UFUNCTION(BlueprintCallable)
	static int32 FindCardInArray(FName SearchForCard, TArray<FName> CardArray, TArray<int32>& AtIndexArray);

	UFUNCTION(BlueprintCallable)
	static TArray<FName> GetAllCardsInActiveSet(ECardSet CardSet);	

	static void RemoveCardFromDeck(bool RemoveAll, int32 IndexToRemove, TArray<FName>& Deck);

	static int32 CountCardsInDeck(TArray<FName> Deck);

	static TArray<FName> FilterWeightedCardsInDeck(TArray<FName> CardsInDeck);
};
