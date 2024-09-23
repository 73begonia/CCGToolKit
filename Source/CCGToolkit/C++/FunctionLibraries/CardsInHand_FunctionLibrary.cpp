// Fill out your copyright notice in the Description page of Project Settings.


#include "CardsInHand_FunctionLibrary.h"
#include "Controller_FunctionLibrary.h"
#include "Deck_FunctionLibrary.h"
#include "Math_Library.h"
#include "../../CardGame/Structs/Structs.h"

int32 UCardsInHand_FunctionLibrary::CountCardsInHand(TArray<FName> CardsInHand)
{
	return CardsInHand.Num();
}

FName UCardsInHand_FunctionLibrary::GetCardInHand(TArray<FName> CardsInHand, int32 Index, bool LastIndex)
{
	int32 ReturnValue;
	if (LastIndex == true)
	{
		ReturnValue = CardsInHand.Num() - 1;
	}
	else
	{
		ReturnValue = Index;
	}

	return CardsInHand[ReturnValue];
}

bool UCardsInHand_FunctionLibrary::HasCardInHand(TArray<FName> CardsInHand, FName Card, int32 Index)
{
	return (CardsInHand[Index] == Card);
}

TArray<FName> UCardsInHand_FunctionLibrary::FindPlayableCardsInHand(AController* Controller)
{
	TArray<FName> TEMP_PlayableCardsInHand;
	int32 SelfPlayer, Oppenent;
	FSCard CardStruct;

	UController_FunctionLibrary::GetControllerIDForBothPlayers_ServerOnly(Controller, SelfPlayer, Oppenent);
	FControllersState ControllersState = UController_FunctionLibrary::GetControllersStateProfile_ServerOnly(SelfPlayer);
	for (int32 i = 0;i< ControllersState.CardsInHand.Num();i++)
	{
		UDeck_FunctionLibrary::GetCardData(ControllersState.CardsInHand[i],ECardSet::Empty, CardStruct);
		if (CardStruct.Placement_Settings.Mana_Cost<= ControllersState.Mana)
		{
			TEMP_PlayableCardsInHand.Add(ControllersState.CardsInHand[i]);
		}
	}
	return TEMP_PlayableCardsInHand;
}

TArray<FName> UCardsInHand_FunctionLibrary::SortCardsByFocus(TArray<FName> CardsArray, TArray<EAIPersonalityFocus> FocusArray, bool IncludeOrderByValue)
{
	TArray<FName> ReturnCardArray;
	TArray<FName> TEMP_CardArray;
	TArray<FName> TEMP_FocusCardArray;
	FSCard CardStruct;
	FSCard CardStruct2;
	int32 GoalArrayIndex = 0;

	TEMP_CardArray = CardsArray;
	UMath_Library::ShuffleArray(TEMP_CardArray);
	for (int32 i = 0; i < FocusArray.Num(); i++)
	{
		TEMP_FocusCardArray.Empty();
		for (int32 j = 0; j < TEMP_CardArray.Num(); j++)
		{
			UDeck_FunctionLibrary::GetCardData(TEMP_CardArray[i], ECardSet::Empty, CardStruct);
			if (CardStruct.Card_System_Data.Card_Purpose.Find(FocusArray[i])>=0)
			{
				int32 CardValue = CardStruct.Card_System_Data.Card_Value;
				if (IncludeOrderByValue)
				{
					for (int32 k = 0; k < TEMP_FocusCardArray.Num(); k++)
					{
						UDeck_FunctionLibrary::GetCardData(TEMP_FocusCardArray[k], ECardSet::Empty, CardStruct2);
						if (CardValue > CardStruct2.Card_System_Data.Card_Value)
						{
							TEMP_FocusCardArray.Insert(TEMP_CardArray[j], k);
							GoalArrayIndex = k;
							break;
						}
					}
					if (TEMP_FocusCardArray.Num() - 1 <= GoalArrayIndex)
					{
						TEMP_CardArray.Add(TEMP_CardArray[j]);
					}
				}
				else
				{
					ReturnCardArray.Add(TEMP_CardArray[j]);
				}
			}
		}
		ReturnCardArray.Append(TEMP_FocusCardArray);
	}
	return ReturnCardArray;
}
