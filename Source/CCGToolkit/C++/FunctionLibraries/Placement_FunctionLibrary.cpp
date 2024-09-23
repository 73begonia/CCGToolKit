// Fill out your copyright notice in the Description page of Project Settings.


#include "Placement_FunctionLibrary.h"
#include "../CardGameState.h"
#include "Deck_FunctionLibrary.h"
#include "../GamePlay/CardPlacement.h"

bool UPlacement_FunctionLibrary::GetPlayersAvailablePlacementsForCard(int32 PlayerID, FName CardName, TArray<ACardPlacement*>& ReturnAvailablePlacements)
{
	TArray<ACardPlacement*> AvailablePlacements;
	ACardGameState* CardGameState = Cast<ACardGameState>(GWorld->GetGameState());
	if (CardGameState)
	{
		switch (PlayerID)
		{
		case 1:
			for (int32 i =0; i<CardGameState->CardPlacements_Player1.Num();i++)
			{
				if (UPlacement_FunctionLibrary::GetIsPlacementAvailable(CardGameState->CardPlacements_Player1[i], CardName))
				{
					AvailablePlacements.Add(CardGameState->CardPlacements_Player1[i]);
				}
			}
			ReturnAvailablePlacements = AvailablePlacements;
			return AvailablePlacements.Num() > 0;
			break;
		case 2:
			for (int32 i =0; i<CardGameState->CardPlacements_Player2.Num();i++)
			{
				if (UPlacement_FunctionLibrary::GetIsPlacementAvailable(CardGameState->CardPlacements_Player2[i], CardName))
				{
					AvailablePlacements.Add(CardGameState->CardPlacements_Player2[i]);
				}
			}
			ReturnAvailablePlacements = AvailablePlacements;
			return AvailablePlacements.Num() > 0;
			break;
		default:
			break;
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool UPlacement_FunctionLibrary::GetClosestAvailablePlacement(ACardPlacement* Placement, FName CardName, ECardSet CardSet, ACardPlacement*& ReturnPlacement)
{
	ACardPlacement* ChosenPlacement = nullptr;
	bool LeftValidPath = true;
	bool RightValidPath = true;
	bool FlipFlop = true;

	for (int32 j = 0; j <= 12; j++)
	{	
		if (FMath::RandBool())
		{
			if (FlipFlop)
			{
				if (LeftValidPath)
				{
					ChosenPlacement = FindChosenPlacementleft(ChosenPlacement, Placement, CardName, CardSet, LeftValidPath);
					if (ChosenPlacement)
						break;
				}
				else
				{
					if (RightValidPath)
					{
						ChosenPlacement = FindChosenPlacementRight(ChosenPlacement, Placement, CardName, CardSet, RightValidPath);
						if (ChosenPlacement)
							break;
					}
					else
					{
						if (LeftValidPath)
						{
							ChosenPlacement = FindChosenPlacementleft(ChosenPlacement, Placement, CardName, CardSet, LeftValidPath);
							if (ChosenPlacement)
								break;
						}
						else
						{
							ReturnPlacement = ChosenPlacement;
							return UKismetSystemLibrary::IsValid(ChosenPlacement);
						}
					}
				}
				FlipFlop = false;
			}
			else
			{
				if (RightValidPath)
				{
					ChosenPlacement = FindChosenPlacementRight(ChosenPlacement, Placement, CardName, CardSet, RightValidPath);
					if (ChosenPlacement)
						break;
				}
				else
				{
					if (LeftValidPath)
					{
						ChosenPlacement = FindChosenPlacementleft(ChosenPlacement, Placement, CardName, CardSet, LeftValidPath);
						if (ChosenPlacement)
							break;
					}
					else
					{
						ReturnPlacement = ChosenPlacement;
						return ::IsValid(ChosenPlacement);
					}
				}
				FlipFlop = true;
			}
		}
		else
		{
			if (FlipFlop)
			{
				if (RightValidPath)
				{
					ChosenPlacement = FindChosenPlacementRight(ChosenPlacement, Placement, CardName, CardSet, RightValidPath);
					if (ChosenPlacement)
						break;
				}
				else
				{
					if (LeftValidPath)
					{
						ChosenPlacement = FindChosenPlacementleft(ChosenPlacement, Placement, CardName, CardSet, LeftValidPath);
						if (ChosenPlacement)
							break;
					}
					else
					{
						ReturnPlacement = ChosenPlacement;
						return ::IsValid(ChosenPlacement);
					}
				}
				FlipFlop = true;
			}
			else
			{
				if (LeftValidPath)
				{
					ChosenPlacement = FindChosenPlacementleft(ChosenPlacement, Placement, CardName, CardSet, LeftValidPath);
					if (ChosenPlacement)
						break;
				}
				else
				{
					if (RightValidPath)
					{
						ChosenPlacement = FindChosenPlacementRight(ChosenPlacement, Placement, CardName, CardSet, RightValidPath);
						if (ChosenPlacement)
							break;
					}
					else
					{
						if (LeftValidPath)
						{
							ChosenPlacement = FindChosenPlacementleft(ChosenPlacement, Placement, CardName, CardSet, LeftValidPath);
							if (ChosenPlacement)
								break;
						}
						else
						{
							ReturnPlacement = ChosenPlacement;
							return ::IsValid(ChosenPlacement);
						}
					}
				}
				FlipFlop = false;
			}
		}
	}

	ReturnPlacement = ChosenPlacement;
	return ::IsValid(ChosenPlacement);
}

bool UPlacement_FunctionLibrary::GetIsPlacementAvailable(ACardPlacement* Placement, FName CardName)
{
	bool Valid = UPlacement_FunctionLibrary::GetDoesPlacementSupportCardType(Placement, CardName);
	return Valid && !Placement->bIsFull;
}

bool UPlacement_FunctionLibrary::GetIsValidPlacement(ACardPlacement* Placement, FName CardName, ECardSet CardSet, bool& ValidPlacement)
{	
	FSCard CardStruct;
	FPlacementState PlacementState;
	if (Placement)
	{
		UDeck_FunctionLibrary::GetCardData(CardName, CardSet, CardStruct);
		PlacementState = Placement->GetPlacementState_C();
		if (PlacementState.ValidCardTypesArray.Find(CardStruct.Type) >= 0)
		{
			ValidPlacement = !PlacementState.InIsFull && PlacementState.ValidCardTypesArray.Find(CardStruct.Type) >= 0;
			return UKismetSystemLibrary::IsValid(Placement);
		}
		else
		{
			ValidPlacement = false;
			return false;
		}
	}
	else
	{
		ValidPlacement = false;
		return false;
	}
}

bool UPlacement_FunctionLibrary::GetDoesPlacementSupportCardType(ACardPlacement* Placement, FName CardName)
{
	FSCard CardStruct;
	UDeck_FunctionLibrary::GetCardData(CardName, ECardSet::Empty, CardStruct);
	return Placement->ValidCardTypes_Array.Contains(CardStruct.Type);
}

ACardPlacement* UPlacement_FunctionLibrary::FindChosenPlacementleft(ACardPlacement* ChosenPlacement, ACardPlacement* Placement, FName CardName, ECardSet CardSet, bool& LeftValidPath)
{
	int32 i = 0;
	if (i == 0)
	{
		bool ValidPlacement;
		if (true)
		{
			if (UPlacement_FunctionLibrary::GetIsValidPlacement(Placement, CardName, CardSet, ValidPlacement))
			{
				if (ValidPlacement)
				{
					ChosenPlacement = Placement;
					return ChosenPlacement;
				}
				else
				{
					i++;
				}
			}
			else
			{
				LeftValidPath = false;
				i++;
			}
		}	
	}
	if (i == 1)
	{
		bool ValidPlacement;
		if (Placement)
		{
			if (UPlacement_FunctionLibrary::GetIsValidPlacement(Placement->PlacementLeft, CardName, CardSet, ValidPlacement))
			{
				if (ValidPlacement)
				{
					ChosenPlacement = Placement->PlacementLeft;
					return ChosenPlacement;
				}
				else
				{
					i++;
				}
			}
			else
			{
				LeftValidPath = false;
				i++;
			}
		}
		else
		{
			LeftValidPath = false;
			return ChosenPlacement;
		}
	}
	if (i == 2)
	{
		bool ValidPlacement;
		if (Placement->PlacementLeft)
		{
			if (UPlacement_FunctionLibrary::GetIsValidPlacement(Placement->PlacementLeft->PlacementLeft, CardName, CardSet, ValidPlacement))
			{
				if (ValidPlacement)
				{
					ChosenPlacement = Placement->PlacementLeft->PlacementLeft;
					return ChosenPlacement;
				}
				else
				{
					i++;
				}
			}
			else
			{
				LeftValidPath = false;
				i++;
			}
		}
		else
		{
			LeftValidPath = false;
			return ChosenPlacement;
		}
	}
	if (i == 3)
	{
		bool ValidPlacement;
		if (Placement->PlacementLeft->PlacementLeft)
		{
			if (UPlacement_FunctionLibrary::GetIsValidPlacement(Placement->PlacementLeft->PlacementLeft->PlacementLeft, CardName, CardSet, ValidPlacement))
			{
				if (ValidPlacement)
				{
					ChosenPlacement = Placement->PlacementLeft->PlacementLeft->PlacementLeft;
					return ChosenPlacement;
				}
				else
				{
					i++;
				}
			}
			else
			{
				LeftValidPath = false;
				i++;
			}
		}
		else
		{
			LeftValidPath = false;
			return ChosenPlacement;
		}
	}
	if (i == 4)
	{
		bool ValidPlacement;
		if (Placement->PlacementLeft->PlacementLeft->PlacementLeft)
		{
			if (UPlacement_FunctionLibrary::GetIsValidPlacement(Placement->PlacementLeft->PlacementLeft->PlacementLeft->PlacementLeft, CardName, CardSet, ValidPlacement))
			{
				if (ValidPlacement)
				{
					ChosenPlacement = Placement->PlacementLeft->PlacementLeft->PlacementLeft->PlacementLeft;
					return ChosenPlacement;
				}
				else
				{
					i++;
				}
			}
			else
			{
				LeftValidPath = false;
				i++;
			}
		}
		else
		{
			LeftValidPath = false;
			return ChosenPlacement;
		}
	}
	if (i == 5)
	{
		bool ValidPlacement;
		if (Placement->PlacementLeft->PlacementLeft->PlacementLeft->PlacementLeft)
		{
			if (UPlacement_FunctionLibrary::GetIsValidPlacement(Placement->PlacementLeft->PlacementLeft->PlacementLeft->PlacementLeft->PlacementLeft, CardName, CardSet, ValidPlacement))
			{
				if (ValidPlacement)
				{
					ChosenPlacement = Placement->PlacementLeft->PlacementLeft->PlacementLeft->PlacementLeft->PlacementLeft;
					return ChosenPlacement;
				}
			}
			else
			{
				LeftValidPath = false;
			}
		}
		else
		{
			LeftValidPath = false;
			return ChosenPlacement;
		}
	}

	LeftValidPath = false;
	return ChosenPlacement;
}

ACardPlacement* UPlacement_FunctionLibrary::FindChosenPlacementRight(ACardPlacement* ChosenPlacement, ACardPlacement* Placement, FName CardName, ECardSet CardSet, bool& RightValidPath)
{
	int32 i = 0;
	if (i == 0)
	{
		bool ValidPlacement;
		if (UPlacement_FunctionLibrary::GetIsValidPlacement(Placement, CardName, CardSet, ValidPlacement))
		{
			if (ValidPlacement)
			{
				ChosenPlacement = Placement;
				return ChosenPlacement;
			}
			else
			{
				i++;
			}
		}
		else
		{
			RightValidPath = false;
			i++;
		}
	}
	if (i == 1)
	{
		bool ValidPlacement;
		if (Placement)
		{
			if (UPlacement_FunctionLibrary::GetIsValidPlacement(Placement->PlacementRight, CardName, CardSet, ValidPlacement))
			{
				if (ValidPlacement)
				{
					ChosenPlacement = Placement->PlacementRight;
					return ChosenPlacement;
				}
				else
				{
					i++;
				}
			}
			else
			{
				RightValidPath = false;
				i++;
			}
		}
		else
		{
			RightValidPath = false;
			return ChosenPlacement;
		}
	}
	if (i == 2)
	{
		bool ValidPlacement;
		if (Placement->PlacementRight)
		{
			if (UPlacement_FunctionLibrary::GetIsValidPlacement(Placement->PlacementRight->PlacementRight, CardName, CardSet, ValidPlacement))
			{
				if (ValidPlacement)
				{
					ChosenPlacement = Placement->PlacementRight->PlacementRight;
					return ChosenPlacement;
				}
				else
				{
					i++;
				}
			}
			else
			{
				RightValidPath = false;
				i++;
			}
		}
		else
		{
			RightValidPath = false;
			return ChosenPlacement;
		}
	}
	if (i == 3)
	{
		bool ValidPlacement;
		if (Placement->PlacementRight->PlacementRight)
		{
			if (UPlacement_FunctionLibrary::GetIsValidPlacement(Placement->PlacementRight->PlacementRight->PlacementRight, CardName, CardSet, ValidPlacement))
			{
				if (ValidPlacement)
				{
					ChosenPlacement = Placement->PlacementRight->PlacementRight->PlacementRight;
					return ChosenPlacement;
				}
				else
				{
					i++;
				}
			}
			else
			{
				RightValidPath = false;
				i++;
			}
		}
		else
		{
			RightValidPath = false;
			return ChosenPlacement;
		}
	}
	if (i == 4)
	{
		bool ValidPlacement;
		if (Placement->PlacementRight->PlacementRight->PlacementRight)
		{
			if (UPlacement_FunctionLibrary::GetIsValidPlacement(Placement->PlacementRight->PlacementRight->PlacementRight->PlacementRight, CardName, CardSet, ValidPlacement))
			{
				if (ValidPlacement)
				{
					ChosenPlacement = Placement->PlacementRight->PlacementRight->PlacementRight->PlacementRight;
					return ChosenPlacement;
				}
				else
				{
					i++;
				}
			}
			else
			{
				RightValidPath = false;
				i++;
			}
		}
		else
		{
			RightValidPath = false;
			return ChosenPlacement;
		}
	}
	if (i == 5)
	{
		bool ValidPlacement;
		if (Placement->PlacementRight->PlacementRight->PlacementRight->PlacementRight)
		{
			if (UPlacement_FunctionLibrary::GetIsValidPlacement(Placement->PlacementRight->PlacementRight->PlacementRight->PlacementRight->PlacementRight, CardName, CardSet, ValidPlacement) && ValidPlacement)
			{
				ChosenPlacement = Placement->PlacementRight->PlacementRight->PlacementRight->PlacementRight->PlacementRight;
				return ChosenPlacement;
			}
			else
			{
				RightValidPath = false;
			}
		}
		else
		{
			RightValidPath = false;
			return ChosenPlacement;
		}
	}

	RightValidPath = false;
	return ChosenPlacement;
}
