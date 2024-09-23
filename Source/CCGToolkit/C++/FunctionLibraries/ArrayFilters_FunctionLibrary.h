// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CCGToolkit/C++/Enums/CardGameEnums.h"
#include "../../CardGame/Structs/Structs.h"
#include "ArrayFilters_FunctionLibrary.generated.h"

UCLASS()
class CCGTOOLKIT_API UArrayFilters_FunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void FilterCardArrayByName(TArray<FName>FilterArray, EAlphabeticalSorting Method, TArray<FName>& FilteredArray);

	static void FilterCardArrayByNameWithRecursion(EAlphabeticalSorting Method, TArray<FName>& FilteredArray, TArray<FName> Temp_Array, TArray<FName> FinalArray, int32 FilterIndex, TArray<FString> CharacterArray);

	UFUNCTION(BlueprintCallable)
	static void FilterCardArrayByManaCost(TArray<FName> FilterArray, EManaCostSorting Method, int32 ManaMin, int32 ManaMax, TArray<FName>& ReturnFilteredArray);

	static void FilterCardArrayByManaCostWithRecursion(TArray<FName> FilterArray, EManaCostSorting Method, int32 ManaMin, int32 ManaMax, TArray<FName>& ReturnFilteredArray, int32 ValueCheck, int32 CardsFiltered, TArray<FName> FilteredArray, TArray<FName> Temp_Array, FSCard CardStruct);

	UFUNCTION(BlueprintCallable)
	static void FilterIncludedCards(TArray<FName> FilterArray, bool Warrior, bool Mage, bool Spy, bool Obelisk, bool HERO, bool Creature, bool Spell, bool Trap, bool Aura, bool Booster, bool Common,
		bool Uncommon, bool Rare, bool UltraRare, bool Legendary, bool CardWithAbility, bool UnlockedOnly, bool SpecialOnly, bool IncludeUndeckableCards, TArray<FName>& ReturnIncludedCardArray);

	UFUNCTION(BlueprintCallable)
	static void GetManaInDeck(TArray<FName> Array, TArray<int32>& ReturnArray, float& Average);

	template <typename T>
	static void SetArrayElement(T item, TArray<T> item_array, int32 index);

	UFUNCTION(BlueprintCallable)
	static void CalculateDeckStats(TArray<FName> FilterArray, TArray<int32>& Mana, TArray<FName>& UnlockedCards, int32& CardsWithAbilities, TArray<int32>& Rarity);

	UFUNCTION(BlueprintCallable)
	static void FilterCardTypes(TArray<FName> FilterArray, ECardType FilterFor, TArray<FName>& FilteredArray);

	UFUNCTION(BlueprintCallable)
	static void FilterCardClass(TArray<FName> FilterArray, ECardClass FilterFor, TArray<FName>& FilteredArray);

	UFUNCTION(BlueprintCallable)
	static void FilterCardRarity(TArray<FName> FilterArray, ECardRarity FilterFor, TArray<FName>& FilteredArray);

	UFUNCTION(BlueprintCallable)
	static bool ValidateCardThroughFilter(FName CardName, ECardSet CardSet, bool Warrior, bool Mage, bool Spy, bool Obelisk, bool HERO, bool Creature, bool Spell, bool Trap, bool Aura,
		bool Booster, bool Common, bool Uncommon, bool Rare, bool UltraRare, bool Legendary, bool CardWithAbility, bool UnlockedOnly, bool SpecialOnly, bool IncludeUndeckableCards);

};
