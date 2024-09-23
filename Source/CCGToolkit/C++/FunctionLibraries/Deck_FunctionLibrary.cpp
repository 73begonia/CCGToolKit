// Fill out your copyright notice in the Description page of Project Settings.


#include "Deck_FunctionLibrary.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "../Interface/Deck_Interface.h"
#include "Math_Library.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/DataTable.h"
#include "../../CardGame/Structs/Structs.h"

TArray<FName> UDeck_FunctionLibrary::GetChosenDeckArray(int32 Index, TArray<UDataTable*> TargetArray)
{
	TArray<FName> OutRowNames;
	UDataTableFunctionLibrary::GetDataTableRowNames(TargetArray[Index], OutRowNames);
	return OutRowNames;
}

void UDeck_FunctionLibrary::GetCardData(FName CardName, ECardSet CardSet, FSCard& CardStruct)
{
	ECardSet Temp_CardSet = ECardSet::Empty;

	static const FString FindTargetName(TEXT("Trying to read row"));

	UDataTable* BasicSet_DataTable = LoadObject<UDataTable>(NULL, TEXT("DataTable'/Game/CCGToolkitC++/CardGame/DataTable/BasicSet_C_DataTable.BasicSet_C_DataTable'"));
	UDataTable* DebugSet_DataTable = LoadObject<UDataTable>(NULL, TEXT("DataTable'/Game/CCGToolkitC++/CardGame/DataTable/DebugSet_C_DataTable.DebugSet_C_DataTable'"));

	if (BasicSet_DataTable && DebugSet_DataTable)
	{
		FSCard* BasicSetRow = BasicSet_DataTable->FindRow<FSCard>(CardName, FindTargetName);
		FSCard* DebugSetRow = DebugSet_DataTable->FindRow<FSCard>(CardName, FindTargetName);

		switch (CardSet)
		{
		case ECardSet::Empty:
		{
			if (BasicSetRow)
			{
				Temp_CardSet = ECardSet::BasicSet;
			}
			if (!BasicSetRow && DebugSetRow)
			{
				Temp_CardSet = ECardSet::DebugCardSet;
			}
			if (!BasicSetRow && !DebugSetRow)
			{
				Temp_CardSet = ECardSet::Empty;
				UE_LOG(Deck_FunctionLibraryLog, Warning, TEXT("Failed to get %s Row"), *CardName.ToString());
				UE_LOG(Deck_FunctionLibraryLog, Warning, TEXT("Note: Prebuilt Decks may not have been generated. Open the Main Menu to generate the decks."));
			}
			break;
		}

		case ECardSet::BasicSet:
		{
			if (BasicSetRow)
			{
				Temp_CardSet = ECardSet::BasicSet;
			}
			if (!BasicSetRow && DebugSetRow)
			{
				Temp_CardSet = ECardSet::DebugCardSet;
			}
			if (!BasicSetRow && !DebugSetRow)
			{
				Temp_CardSet = ECardSet::Empty;
				UE_LOG(Deck_FunctionLibraryLog, Warning, TEXT("Failed to get %s Row"), *CardName.ToString());
				UE_LOG(Deck_FunctionLibraryLog, Warning, TEXT("Note: Prebuilt Decks may not have been generated. Open the Main Menu to generate the decks."));
			}
			break;
		}

		case ECardSet::DebugCardSet:
		{
			if (DebugSetRow)
			{
				Temp_CardSet = ECardSet::DebugCardSet;
			}
			if (!DebugSetRow)
			{
				Temp_CardSet = ECardSet::Empty;
				UE_LOG(Deck_FunctionLibraryLog, Warning, TEXT("Failed to get %s Row"), *CardName.ToString());
				UE_LOG(Deck_FunctionLibraryLog, Warning, TEXT("Note: Prebuilt Decks may not have been generated. Open the Main Menu to generate the decks."));
			}
			break;
		}

		case ECardSet::EmptySet:
		{
			Temp_CardSet = ECardSet::Empty;
			UE_LOG(Deck_FunctionLibraryLog, Warning, TEXT("Failed to get %s Row"), *CardName.ToString());
			UE_LOG(Deck_FunctionLibraryLog, Warning, TEXT("Note: Prebuilt Decks may not have been generated. Open the Main Menu to generate the decks."));
			break;
		}

		default:
			break;
		}

		switch (Temp_CardSet)
		{
		case ECardSet::Empty:
			break;
		case ECardSet::BasicSet:
			if (BasicSetRow)
			{
				CardStruct = *BasicSetRow;
			}
			break;
		case ECardSet::DebugCardSet:
			if (DebugSetRow)
			{
				CardStruct = *DebugSetRow;
			}
			break;
		case ECardSet::EmptySet:
			break;
		default:
			break;
		}
	}
	
}

int32 UDeck_FunctionLibrary::GetRandomCardFromDeck(AController* Controller, FName& Card, ECardSet& CardRace)
{
	TArray<FName> Deck;
	IDeck_Interface* Deck_Interface = Cast<IDeck_Interface>(Controller);
	if (Deck_Interface)
	{
		Deck_Interface->Execute_GetPlayerDeck_C(Controller, Deck);
		Card = Deck[FMath::RandRange(0, Deck.Num() - 1)];
		CardRace = ECardSet::Empty;
		return FMath::RandRange(0, Deck.Num()-1);
	}
	return 0;
}

FName UDeck_FunctionLibrary::GetCardFromDeck(TArray<FName> Deck, int32 Index, bool LastIndex)
{
	int32 ReturnValue;
	if (LastIndex == true)
	{
		ReturnValue = Deck.Num()-1;
	}
	else
	{
		ReturnValue = Index;
	}
	return Deck[ReturnValue];
}

int32 UDeck_FunctionLibrary::FindCardInArray(FName SearchForCard, TArray<FName> CardArray, TArray<int32>& AtIndexArray)
{
	TArray<int32> Temp_AtIndexArray;
	int32 NumberFound = 0;
	for (int32 i = 0; i < CardArray.Num(); i++)
	{
		if (CardArray[i] == SearchForCard)
		{
			Temp_AtIndexArray.Add(i);
			NumberFound++;
		}
	}
	AtIndexArray = Temp_AtIndexArray;
	return NumberFound;
}

TArray<FName> UDeck_FunctionLibrary::GetAllCardsInActiveSet(ECardSet CardSet)
{
	TArray<FName> EmpySet;
	TArray<FName> CombinedCardSets;
	bool bAllCardSets = false;
	TArray<FName> BasicSetOutRowNames;
	TArray<FName> DebugSetOutRowNames;

	static const FString FindTargetName(TEXT("Trying to read row"));

	UDataTable* BasicSet_DataTable = LoadObject<UDataTable>(NULL, TEXT("DataTable'/Game/CCGToolkitC++/CardGame/DataTable/BasicSet_C_DataTable.BasicSet_C_DataTable'"));
	UDataTable* DebugSet_DataTable = LoadObject<UDataTable>(NULL, TEXT("DataTable'/Game/CCGToolkitC++/CardGame/DataTable/DebugSet_C_DataTable.DebugSet_C_DataTable'"));

	BasicSetOutRowNames = BasicSet_DataTable->GetRowNames();
	DebugSetOutRowNames = DebugSet_DataTable->GetRowNames();

	switch (CardSet)
	{
	case ECardSet::Empty:

		bAllCardSets = true;
		CombinedCardSets.Append(BasicSetOutRowNames);
		CombinedCardSets.Append(DebugSetOutRowNames);
			return CombinedCardSets;
		break;
	case ECardSet::BasicSet:

		CombinedCardSets.Append(BasicSetOutRowNames);
			return CombinedCardSets;
		break;
	case ECardSet::DebugCardSet:

		CombinedCardSets.Append(DebugSetOutRowNames);
			return CombinedCardSets;
		break;
	case ECardSet::EmptySet:

		CombinedCardSets.Append(EmpySet);
			return CombinedCardSets;
		break;
	default:
		break;
	}

	return CombinedCardSets;
}

void UDeck_FunctionLibrary::RemoveCardFromDeck(bool RemoveAll, int32 IndexToRemove, TArray<FName>& Deck)
{
	if (RemoveAll)
	{
		Deck.Empty();
	}
	else
	{
		Deck.RemoveAt(IndexToRemove);
	}
}

int32 UDeck_FunctionLibrary::CountCardsInDeck(TArray<FName> Deck)
{
	return Deck.Num();
}

TArray<FName> UDeck_FunctionLibrary::FilterWeightedCardsInDeck(TArray<FName> CardsInDeck)
{
	TArray<FName> TempDeck;
	TArray<FName> ReturnPlayerDeck;
	int32 WeightedFilterIndex;
	FSCard CardStruct;
	int32 ReturnValue;

	TempDeck = CardsInDeck;
	ReturnPlayerDeck.SetNum(TempDeck.Num());
	for (int32 i = 0; i < TempDeck.Num(); i++)
	{
		UDeck_FunctionLibrary::GetCardData(TempDeck[i], ECardSet::Empty, CardStruct);
		if (CardStruct.DeckSettings.Weight > 2)
		{
			WeightedFilterIndex = 0;
			if (CardStruct.DeckSettings.Weight+i > ReturnPlayerDeck.Num()-1)
			{
				ReturnValue = i;
			}
			else
			{
				ReturnValue = CardStruct.DeckSettings.Weight+i;
			}
			WeightedFilterIndex = ReturnValue;

			while (ReturnPlayerDeck[WeightedFilterIndex] != "None")
			{
				if (WeightedFilterIndex + 1 > ReturnPlayerDeck.Num() - 1)
				{
					WeightedFilterIndex = 1;
				}
				else
				{
					WeightedFilterIndex++;
				}
			}
			UMath_Library::SetArrayElement(TempDeck[i], ReturnPlayerDeck, WeightedFilterIndex);
		}
	}

	UKismetSystemLibrary::PrintString( GWorld->GetWorld(),"______________________________Weighted Cards_________________________________");
	UKismetSystemLibrary::PrintString(GWorld->GetWorld(), "______________________________Remove Player Deck Cards From Temp_________________________________");
	for (int32 i = 0; i<ReturnPlayerDeck.Num(); i++)
	{
		TempDeck.Remove(ReturnPlayerDeck[i]);
	}
	UKismetSystemLibrary::PrintString(GWorld->GetWorld(), "______________________________Fill In Empty Deck Places_________________________________");
	for (int32 i = 0; i<ReturnPlayerDeck.Num();i++)
	{
		if (ReturnPlayerDeck[i] == "None")
		{
			if (TempDeck.Num()>0)
			{
				UMath_Library::SetArrayElement(TempDeck[0], ReturnPlayerDeck, i);
				TempDeck.RemoveAt(0);
			}
		}
	}
	UKismetSystemLibrary::PrintString(GWorld->GetWorld(), "______________________________These are the Cards_________________________________");
	for (int32 i = 0; i<ReturnPlayerDeck.Num(); i++)
	{
		UKismetSystemLibrary::PrintString(GWorld->GetWorld(), FString::FromInt(i).Append(":").Append(ReturnPlayerDeck[i].ToString()));
	}
	return ReturnPlayerDeck;
}
