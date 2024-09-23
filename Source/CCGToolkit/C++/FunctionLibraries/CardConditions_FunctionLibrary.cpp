// Fill out your copyright notice in the Description page of Project Settings.


#include "CardConditions_FunctionLibrary.h"
#include "Controller_FunctionLibrary.h"
#include "Misc_FunctionLibrary.h"
#include "../../CardGameAI/AIC++/CardGameAIController.h"
#include "../../CardGameAI/AIC++/CardGameAI_Pawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../GamePlay/Graveyard.h"

bool UCardConditions_FunctionLibrary::ConditionIfCardTypeInPlay(AController* Controller, int32 MinCreaturesOnBoard, bool bOpponent, ECardType CardType, bool AI_Only)
{
	int32 CreaturesOnBoard = 0;
	TArray<ACard3D*> CardArray;
	ACardGameAIController* CardGameAIController;
	ACardGameAI_Pawn* CardGameAI_Pawn;
	int32 SelfPlayer, Oppenent;
	int32 ReturnValue = 0;
	TArray<ACard3D*> PlayerCards;
	TArray<ACard3D*> OppenentCards;

	if (AI_Only)
	{
		if (UController_FunctionLibrary::GetAIControllerReference_ServerOnly(Controller, CardGameAIController, CardGameAI_Pawn))
		{
			UMisc_FunctionLibrary::GetPlayerStateReference(Controller, SelfPlayer, Oppenent);
			if (bOpponent == false)
			{
				ReturnValue = SelfPlayer;
			}
			else if(bOpponent == true)
			{
				ReturnValue = Oppenent;
			}
			ACardGameState* CardGameState = Cast<ACardGameState>(GWorld->GetGameState());
			CardGameState->GetBoardState_C(ReturnValue, PlayerCards, OppenentCards);
			CardArray = PlayerCards;
			for (int32 i = 0; i< CardArray.Num(); i++)
			{
				if (CardArray[i]->CardDataStruct.Type == CardType)
				{
					CreaturesOnBoard++;
				}
			}
			return CreaturesOnBoard >= MinCreaturesOnBoard;
		}
		else
		{
			return false;
		}
	}
	else
	{
		UMisc_FunctionLibrary::GetPlayerStateReference(Controller, SelfPlayer, Oppenent);
		if (bOpponent == false)
		{
			ReturnValue = SelfPlayer;
		}
		else if (bOpponent == true)
		{
			ReturnValue = Oppenent;
		}
		ACardGameState* CardGameState = Cast<ACardGameState>(GWorld->GetGameState());
		CardGameState->GetBoardState_C(ReturnValue, PlayerCards, OppenentCards);
		CardArray = PlayerCards;
		for (int32 i = 0; i < CardArray.Num(); i++)
		{
			if (CardArray[i]->CardDataStruct.Type == CardType)
			{
				CreaturesOnBoard++;
			}
		}
		return CreaturesOnBoard >= MinCreaturesOnBoard;
	}
}

bool UCardConditions_FunctionLibrary::ConditionIfPlayerLowHealth(int32 PlayerID)
{
	ACardGameMode* CardGameMode = Cast<ACardGameMode>(GWorld->GetAuthGameMode());
	FControllersState ControllersState = UController_FunctionLibrary::GetControllersStateProfile_ServerOnly(PlayerID);
	return (float(ControllersState.Health) / float(CardGameMode->PlayerStartingHealth) <= 0.35);
}

bool UCardConditions_FunctionLibrary::ConditionIfPlayerHighHealth(int32 PlayerID)
{
	ACardGameMode* CardGameMode = Cast<ACardGameMode>(GWorld->GetAuthGameMode());
	FControllersState ControllersState = UController_FunctionLibrary::GetControllersStateProfile_ServerOnly(PlayerID);
	return (float(ControllersState.Health) / float(CardGameMode->PlayerStartingHealth) >= 0.7);
}

bool UCardConditions_FunctionLibrary::ConditionIfStartOfPlay_AIOnly(int32 PlayerID)
{
	ACardGameAIController* CardGameAIController;
	ACardGameAI_Pawn* CardGameAI_Pawn;
	AController* controller = UController_FunctionLibrary::GetControllerReferenceFromID_ServerOnly(PlayerID, GWorld->GetWorld());
	if (UController_FunctionLibrary::GetAIControllerReference_ServerOnly(controller, CardGameAIController, CardGameAI_Pawn))
	{
		UBlackboardComponent* Target = CardGameAIController->GetBlackboardComponent();
		return Target->GetValueAsInt("Loop")<= 2;
	}
	else
	{
		return true;
	}

}

bool UCardConditions_FunctionLibrary::ConditionIfEndOfPlay_AIOnly(int32 PlayerID)
{
	ACardGameAIController* CardGameAIController;
	ACardGameAI_Pawn* CardGameAI_Pawn;
	AController* controller = UController_FunctionLibrary::GetControllerReferenceFromID_ServerOnly(PlayerID, GWorld->GetWorld());
	if (UController_FunctionLibrary::GetAIControllerReference_ServerOnly(controller, CardGameAIController, CardGameAI_Pawn))
	{
		bool Valid = CardGameAIController->GetBlackboardComponent()->GetValueAsBool("bFinalLoop");

		return Valid;
	}
	else
	{
		return true;
	}
}

bool UCardConditions_FunctionLibrary::ConditionIfPlacedOnOwnedPlacement(int32 PlayerID, ACard3D* RecievingCard, bool Oppenent)
{
	bool ReturnValue = false;
	if (RecievingCard)
	{
		if (Oppenent == true)
		{
			ReturnValue = PlayerID!=RecievingCard->PlacementOwner->PlayerIndex;
		}
		if (Oppenent == false)
		{
			ReturnValue = PlayerID != RecievingCard->PlacementOwner->PlayerIndex;
		}
		return ReturnValue||RecievingCard->PlacementOwner->PlayerIndex==0;
	}
	else
	{
		return true;
	}
}

bool UCardConditions_FunctionLibrary::ConditionIfRecievingCardType(ACard3D* RecievingCard, ECardType RecievingCardType)
{
	return RecievingCard->Type == RecievingCardType;
}

bool UCardConditions_FunctionLibrary::ConditionIsNotOwningPlayer(int32 PlayerID, ACard3D* RecievingCard)
{
	if (RecievingCard)
	{
		return PlayerID!=RecievingCard->OwningPlayerID;
	}
	else
	{
		return false;
	}
}

bool UCardConditions_FunctionLibrary::ConditionIfPlayerHasCardsInGraveyard(AController* Controller, bool AIOnly, int32 MinCardsInGraveyard)
{
	ACardGameAIController* CardGameAIController;
	ACardGameAI_Pawn* CardGameAI_Pawn;
	AGraveyard* Graveyard = nullptr;
	ACardGameState* CardGameState = Cast<ACardGameState>(GWorld->GetGameState());
	if (AIOnly)
	{
		if (UController_FunctionLibrary::GetAIControllerReference_ServerOnly(Controller, CardGameAIController, CardGameAI_Pawn))
		{
			Graveyard = CardGameState->GetGraveyardReference_C(UController_FunctionLibrary::GetControllerID_ServerOnly(Controller));
			return Graveyard->GraveyardList.Num()>= MinCardsInGraveyard;
		}
		else
		{
			return false;
		}
	}
	else
	{
		Graveyard = CardGameState->GetGraveyardReference_C(UController_FunctionLibrary::GetControllerID_ServerOnly(Controller));
		return Graveyard->GraveyardList.Num() >= MinCardsInGraveyard;
	}
}

bool UCardConditions_FunctionLibrary::ConditionIfLowPlayableCardsInHand_AIOnly(int32 PlayerID, int32 LowValue)
{
	ACardGameAIController* CardGameAIController;
	ACardGameAI_Pawn* CardGameAI_Pawn;
	AController* controller = UController_FunctionLibrary::GetControllerReferenceFromID_ServerOnly(PlayerID, GWorld->GetWorld());
	if (UController_FunctionLibrary::GetAIControllerReference_ServerOnly(controller, CardGameAIController, CardGameAI_Pawn))
	{
		return CardGameAIController->PlayableCardsInHandList.Num()<= LowValue;
	}
	else
	{
		return true;
	}
}
