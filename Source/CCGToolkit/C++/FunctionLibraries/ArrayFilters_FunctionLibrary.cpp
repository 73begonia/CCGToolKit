// Fill out your copyright notice in the Description page of Project Settings.


#include "ArrayFilters_FunctionLibrary.h"
#include "Deck_FunctionLibrary.h"
#include "Misc_FunctionLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "CCGToolkit//CardGame/Structs/Structs.h"
#include "Math_Library.h"

void UArrayFilters_FunctionLibrary::FilterCardArrayByName(TArray<FName>FilterArray, EAlphabeticalSorting Method, TArray<FName>& FilteredArray)
{
	TArray<FName> Temp_Array;
	TArray<FName> FinalArray;
	int32 FilterIndex = 0;
	TArray<FString> CharacterArray = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", 
									  "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};

	Temp_Array = FilterArray;

	switch (Method)
	{
	case EAlphabeticalSorting::AToZ:
		FilterIndex = 0;
		break;
	case EAlphabeticalSorting::ZToA:
		FilterIndex = CharacterArray.Num();
		break;
	default:
		break;
	}
	FilterCardArrayByNameWithRecursion(Method, FilteredArray, Temp_Array, FinalArray, FilterIndex, CharacterArray);
}

void UArrayFilters_FunctionLibrary::FilterCardArrayByNameWithRecursion(EAlphabeticalSorting Method, TArray<FName>& FilteredArray, TArray<FName> Temp_Array, TArray<FName> FinalArray, int32 FilterIndex, TArray<FString> CharacterArray)
{
	for (int32 i = 0; i < Temp_Array.Num(); i++)
	{
		if (UKismetStringLibrary::StartsWith(Temp_Array[i].ToString(), CharacterArray[FilterIndex]))
		{
			FinalArray.Add(Temp_Array[i]);
		}
	}
	switch (Method)
	{
	case EAlphabeticalSorting::AToZ:
		FilterIndex++;
		if (FilterIndex >= CharacterArray.Num())
		{
			FilteredArray = FinalArray;
		}
		else
		{
			FilterCardArrayByNameWithRecursion(Method, FilteredArray, Temp_Array, FinalArray, FilterIndex, CharacterArray);
		}
		break;
	case EAlphabeticalSorting::ZToA:
		FilterIndex--;
		if (FilterIndex <= 0)
		{
			FilteredArray = FinalArray;
		}
		else
		{
			FilterCardArrayByNameWithRecursion(Method, FilteredArray, Temp_Array, FinalArray, FilterIndex, CharacterArray);
		}
		break;
	default:
		break;
	}
}

void UArrayFilters_FunctionLibrary::FilterCardArrayByManaCost(TArray<FName> FilterArray, EManaCostSorting Method, int32 ManaMin, int32 ManaMax, TArray<FName>& ReturnFilteredArray)
{
	int32 ValueCheck = 0;
	int32 CardsFiltered = 0;
	TArray<FName> FilteredArray;
	TArray<FName> Temp_Array;
	FSCard CardStruct;

	Temp_Array = FilterArray;
	switch (Method)
	{
	case EManaCostSorting::LowesttoHighest:
		ValueCheck = FMath::Clamp(0, ManaMin, ManaMax);
		break;
	case EManaCostSorting::HighesttoLowest:
		ValueCheck = FMath::Clamp(10, ManaMin, ManaMax);
		break;
	default:
		break;
	}

	FilterCardArrayByManaCostWithRecursion(FilterArray, Method, ManaMin, ManaMax, ReturnFilteredArray, ValueCheck, CardsFiltered, FilteredArray, Temp_Array, CardStruct);
}

void UArrayFilters_FunctionLibrary::FilterCardArrayByManaCostWithRecursion(TArray<FName> FilterArray, EManaCostSorting Method, int32 ManaMin, int32 ManaMax, TArray<FName>& ReturnFilteredArray, int32 ValueCheck, int32 CardsFiltered, TArray<FName> FilteredArray, TArray<FName> Temp_Array, FSCard CardStruct)
{
	for (int32 i = 0; i < Temp_Array.Num(); i++)
	{
		UDeck_FunctionLibrary::GetCardData(Temp_Array[i], ECardSet::Empty, CardStruct);
		if (CardStruct.Placement_Settings.Mana_Cost == ValueCheck)
		{
			if (ValueCheck >= ManaMin && ValueCheck <= ManaMax)
			{
				FilterArray.Add(Temp_Array[i]);
				CardsFiltered++;
			}
		}
	}
	switch (Method)
	{
	case EManaCostSorting::LowesttoHighest:
		ValueCheck++;
		if (Temp_Array.Num() == CardsFiltered)
		{
			ReturnFilteredArray = FilteredArray;
		}
		else
		{
			FilterCardArrayByManaCostWithRecursion(FilterArray, Method, ManaMin, ManaMax, ReturnFilteredArray, ValueCheck, CardsFiltered, FilteredArray, Temp_Array, CardStruct);
		}
		break;
	case EManaCostSorting::HighesttoLowest:
		ValueCheck--;
		if (Temp_Array.Num() == CardsFiltered)
		{
			ReturnFilteredArray = FilteredArray;
		}
		else
		{
			FilterCardArrayByManaCostWithRecursion(FilterArray, Method, ManaMin, ManaMax, ReturnFilteredArray, ValueCheck, CardsFiltered, FilteredArray, Temp_Array, CardStruct);
		}
		break;
	default:
		break;
	}
}

void UArrayFilters_FunctionLibrary::FilterIncludedCards(TArray<FName> FilterArray, bool Warrior, bool Mage, bool Spy, bool Obelisk, bool HERO, bool Creature, bool Spell, bool Trap, bool Aura, bool Booster, bool Common, bool Uncommon, bool Rare, bool UltraRare, bool Legendary, bool CardWithAbility, bool UnlockedOnly, bool SpecialOnly, bool IncludeUndeckableCards, TArray<FName>& ReturnIncludedCardArray)
{
	TArray<FName> Temp_IncludedCardArray;
	TArray<FName> IncludedCardArray;

	Temp_IncludedCardArray = FilterArray;

	for (int32 i = 0; i < Temp_IncludedCardArray.Num(); i++)
	{
		if (ValidateCardThroughFilter(Temp_IncludedCardArray[i], ECardSet::Empty, Warrior, Mage, Spy, Obelisk, HERO, Creature, Spell, Trap, Aura, Booster, Common, Uncommon, Rare, UltraRare, Legendary, CardWithAbility, UnlockedOnly, SpecialOnly, IncludeUndeckableCards))
		{
			IncludedCardArray.Add(Temp_IncludedCardArray[i]);
		}
	}
	ReturnIncludedCardArray = IncludedCardArray;
}

void UArrayFilters_FunctionLibrary::GetManaInDeck(TArray<FName> Array, TArray<int32>& ReturnArray, float& Average)
{
	TArray<FName> Deck;
	TArray<int32> ValuesArray = { 99,99,99,99,99,99,99,99,99,99 };
	float TEMP_AverageManaCost = 0;
	FSCard CardStruct;

	if (Array.Num() > 0)
	{
		ValuesArray.Empty();
		ValuesArray.SetNum(10);
		for (int i = 0; i < Array.Num(); i++)
		{
			UDeck_FunctionLibrary::GetCardData(Array[i], ECardSet::Empty, CardStruct);
			UMath_Library::SetArrayElement(ValuesArray[CardStruct.Placement_Settings.Mana_Cost] + 1, ValuesArray, CardStruct.Placement_Settings.Mana_Cost);
		}
		for (int32 i = 0; i <= 9; i++)
		{
			TEMP_AverageManaCost += ValuesArray[i] * i;
		}
		if (Array.Num() > 0)
		{
			ReturnArray = ValuesArray;
			Average = (float(TEMP_AverageManaCost) / float(Array.Num()));
		}
		else
		{
			ReturnArray = ValuesArray;
			Average = 0.0;
		}
	}
	else
	{
		ReturnArray.Init(0, 10);
		Average = 0.0;
	}
}

template<typename T>
inline void UArrayFilters_FunctionLibrary::SetArrayElement(T item, TArray<T> item_array, int32 index)
{
	if (item_array.Num() - 1 < index)
		item_array.SetNum(index);

	item_array.Insert(item, index);
}

void UArrayFilters_FunctionLibrary::CalculateDeckStats(TArray<FName> FilterArray, TArray<int32>& Mana, TArray<FName>& UnlockedCards, int32& CardsWithAbilities, TArray<int32>& Rarity)
{
	TArray<int32> ManaStats = {0};
	TArray<FName> UnlockedCardsArray = {"None"};
	int32 CardsWithAbilities2 = 0;
	TArray<int32> CardRarity = {0};
	FSCard CardStruct;

	if (FilterArray.Num() > 0)
	{
		ManaStats.Empty();
		ManaStats.SetNum(10);
		CardRarity.Empty();
		CardRarity.SetNum(10);
		UnlockedCardsArray.Empty();
		UnlockedCardsArray.SetNum(10);

		for (int32 i = 0; i < FilterArray.Num(); i++)
		{
			UDeck_FunctionLibrary::GetCardData(FilterArray[i], ECardSet::Empty, CardStruct);
			SetArrayElement(ManaStats[CardStruct.Placement_Settings.Mana_Cost] + 1, ManaStats, CardStruct.Placement_Settings.Mana_Cost);
			SetArrayElement(CardRarity[int32(CardStruct.Rarity)] + 1, CardRarity, int32(CardStruct.Rarity));
			if (CardStruct.Abilities[0].Type != EAbilityType::None)
			{
				CardsWithAbilities2++;
			}
			if (CardStruct.DeckSettings.Unlocked)
			{
				UnlockedCardsArray.Add(FilterArray[i]);
			}
		}
	}
}

void UArrayFilters_FunctionLibrary::FilterCardTypes(TArray<FName> FilterArray, ECardType FilterFor, TArray<FName>& FilteredArray)
{
	TArray<FName> TempArray;
	FSCard CardStruct;
	for (int i = 0; i < FilterArray.Num(); i++)
	{
		UDeck_FunctionLibrary::GetCardData(FilterArray[i], ECardSet::Empty, CardStruct);
		if (CardStruct.Type == FilterFor)
		{
			TempArray.Add(FilterArray[i]);
		}
		FilteredArray = TempArray;
	}
}

void UArrayFilters_FunctionLibrary::FilterCardClass(TArray<FName> FilterArray, ECardClass FilterFor, TArray<FName>& FilteredArray)
{
	TArray<FName> TempArray;
	FSCard CardStruct;

	for (int i = 0; i < FilterArray.Num(); i++)
	{
		UDeck_FunctionLibrary::GetCardData(FilterArray[i], ECardSet::Empty, CardStruct);
		if (CardStruct.Class == FilterFor)
		{
			TempArray.Add(FilterArray[i]);
		}
		FilteredArray = TempArray;
	}
}

void UArrayFilters_FunctionLibrary::FilterCardRarity(TArray<FName> FilterArray, ECardRarity FilterFor, TArray<FName>& FilteredArray)
{
	TArray<FName> TempArray;
	FSCard CardStruct;
	for (int i = 0; i < FilterArray.Num(); i++)
	{
		UDeck_FunctionLibrary::GetCardData(FilterArray[i], ECardSet::Empty, CardStruct);
		if (CardStruct.Rarity == FilterFor)
		{
			TempArray.Add(FilterArray[i]);
		}
		FilteredArray = TempArray;
	}
}

bool UArrayFilters_FunctionLibrary::ValidateCardThroughFilter(FName CardName, ECardSet CardSet, bool Warrior, bool Mage, bool Spy, bool Obelisk, bool HERO, bool Creature, bool Spell, bool Trap, bool Aura, bool Booster, bool Common, bool Uncommon, bool Rare, bool UltraRare, bool Legendary, bool CardWithAbility, bool UnlockedOnly, bool SpecialOnly, bool IncludeUndeckableCards)
{
	FSCard CardStruct;
	bool FilterCardClass = true, FilterCardType = true, FilterCardRarity = true;

	UDeck_FunctionLibrary::GetCardData(CardName, CardSet, CardStruct);

	switch (CardStruct.Class)
	{
	case ECardClass::Warrior:
		FilterCardClass = Warrior;
		break;
	case ECardClass::Mage:
		FilterCardClass = Mage;
	case ECardClass::Spy:
		FilterCardClass = Spy;
		break;
	case ECardClass::Obelisk:
		FilterCardClass = Obelisk;
		break;
	default:
		FilterCardType = true;
		break;
	}

	switch (CardStruct.Type)
	{
	case ECardType::HERO:
		FilterCardType = HERO;
		break;
	case ECardType::Creature:
		FilterCardType = Creature;
		break;
	case ECardType::Spell:
		FilterCardType = Spell;
		break;
	case ECardType::Trap:
		FilterCardType = Trap;
		break;
	case ECardType::Aura:
		FilterCardType = Aura;
		break;
	case ECardType::Booster:
		FilterCardType = Booster;
		break;
	default:
		FilterCardType = true;
		break;
	}

	switch (CardStruct.Rarity)
	{
	case ECardRarity::Common:
		FilterCardRarity = Common;
		break;
	case ECardRarity::Uncommon:
		FilterCardRarity = Uncommon;
		break;
	case ECardRarity::Rare:
		FilterCardRarity = Rare;
		break;
	case ECardRarity::UltraRare:
		FilterCardRarity = UltraRare;
		break;
	case ECardRarity::Legendary:
		FilterCardRarity = Legendary;
		break;
	default:
		break;
	}

	if (FilterCardClass && FilterCardRarity && FilterCardType)
	{
		if (CardWithAbility)
		{
			if (!(CardStruct.Abilities.Num() > 1 || CardStruct.Abilities[0].Type != EAbilityType::None))
			{
				return false;
			}
		}
		if (UnlockedOnly)
		{
			if (!CardStruct.DeckSettings.Unlocked)
			{
				return false;
			}
		}
		if (!IncludeUndeckableCards)
		{
			if (!CardStruct.DeckSettings.AddToPlayerDeck)
			{
				return false;
			}
		}
		if (SpecialOnly)
		{
			if (!CardStruct.Special)
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

