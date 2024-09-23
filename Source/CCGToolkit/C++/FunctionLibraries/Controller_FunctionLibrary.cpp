// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller_FunctionLibrary.h"
#include "GameFramework/Controller.h"
#include "../CardGamePlayerController.h"
#include "../CardGamePlayerState.h"
#include "../../CardGameAI/AIC++/CardGameAIController.h"
#include "../../CardGameAI/AIC++/CardGameAI_Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"

bool UController_FunctionLibrary::GetPlayerControllerReference_ServerOnly(AController* Controller, ACardGamePlayerController*& CardGamePlayerController, ACardGamePlayerState*& CardGamePlayerState)
{
	CardGamePlayerController = Cast<ACardGamePlayerController>(Controller);
	if (CardGamePlayerController)
	{
		CardGamePlayerState = Cast<ACardGamePlayerState>(CardGamePlayerController->PlayerState);
	}
	if (CardGamePlayerController)
	{
		if (CardGamePlayerState)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

int32 UController_FunctionLibrary::GetControllerID_ServerOnly(AController* Controller)
{
	ACardGamePlayerController* CardGamePlayerController = Cast<ACardGamePlayerController>(Controller);
	if (CardGamePlayerController)
	{
		ACardGamePlayerState* CardGamePlayerState = Cast<ACardGamePlayerState>(CardGamePlayerController->PlayerState);
		if (CardGamePlayerState)
		{
			return CardGamePlayerState->CardGamePlayerId;
		}
		return -1;
	}

	if (!CardGamePlayerController)
	{
		ACardGameAIController* CardGameAIController = Cast<ACardGameAIController>(Controller);
		if (CardGameAIController)
		{
			ACardGameAI_Pawn* CardGameAI_Pawn = Cast<ACardGameAI_Pawn>(CardGameAIController->GetPawn());
			if (CardGameAI_Pawn)
			{
				return CardGameAI_Pawn->PlayerID;
			}
			return -1;
		}
		else
		{
			UKismetSystemLibrary::PrintString(GEngine->GetWorld(), "Failed to get Controller");
			return -1;
		}
	}
	else
	{
		UKismetSystemLibrary::PrintString(GEngine->GetWorld(), "Failed to get Controller");
		return -1;
	}
}

bool UController_FunctionLibrary::GetAIControllerReference_ServerOnly(AController* Controller, ACardGameAIController*& CardGameAIController, ACardGameAI_Pawn*& CardGameAIPawn)
{
	if (Controller)
	{
		CardGameAIController = Cast<ACardGameAIController>(Controller);
		if (CardGameAIController)
		{
			CardGameAIPawn = Cast<ACardGameAI_Pawn>(CardGameAIController->GetPawn());
		}
		if (CardGameAIController)
		{
			if (CardGameAIPawn)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	return false;
}

AController* UController_FunctionLibrary::GetControllerReferenceFromID_ServerOnly(int32 ControllerID, UWorld* TheWorld)
{
	ACardGameMode* GameModeRef = Cast<ACardGameMode>(UGameplayStatics::GetGameMode(TheWorld));
	if (GameModeRef != nullptr)
	{
		return GameModeRef->GameControllersArray[ControllerID - 1];
	}
	else
	{
		return nullptr;
	}
}

FControllersState UController_FunctionLibrary::GetControllersStateProfile_ServerOnly(int32 ControllerID)
{
	ACardGamePlayerController* CardGamePlayerController;

	ACardGamePlayerState* CardGamePlayerState = nullptr;

	ACardGameAIController* CardGameAIController;

	ACardGameAI_Pawn* CardGameAIPawn;

	FControllersState ControllersState;

	if (GetPlayerControllerReference_ServerOnly(GetControllerReferenceFromID_ServerOnly(ControllerID, GWorld->GetWorld()), CardGamePlayerController, CardGamePlayerState))
	{
		ControllersState.Health = CardGamePlayerState->Health;
		ControllersState.NumCardsInHand = CardGamePlayerState->CardsInHand;
		ControllersState.CardsInDeck = CardGamePlayerState->CardsInDeck;
		ControllersState.ActiveCards = CardGamePlayerState->ActiveCards;
		ControllersState.Mana = CardGamePlayerState->Mana;
		ControllersState.ManaMax = CardGamePlayerState->Mana_Max;
		ControllersState.PlayerTurn = CardGamePlayerState->PlayerTurn;
		ControllersState.Deck = CardGamePlayerController->PlayerDeck;
		ControllersState.CardsInHand = CardGamePlayerController->CardsInHand;
		ControllersState.PlayerState = CardGamePlayerState;
		return ControllersState;
	}
	else
	{
		if (GetAIControllerReference_ServerOnly(GetControllerReferenceFromID_ServerOnly(ControllerID, GWorld->GetWorld()), CardGameAIController, CardGameAIPawn))
		{
			ControllersState.Health = CardGameAIPawn->Health;
			ControllersState.NumCardsInHand = CardGameAIPawn->CardsInHand;
			ControllersState.CardsInDeck = CardGameAIPawn->CardsInDeck;
			ControllersState.ActiveCards = CardGameAIPawn->ActiveCards;
			ControllersState.Mana = CardGameAIPawn->Mana;
			ControllersState.ManaMax = CardGameAIPawn->ManaMax;
			ControllersState.PlayerTurn = CardGameAIPawn->PlayerTurn;
			ControllersState.Deck = CardGameAIController->AIDeck;
			ControllersState.CardsInHand = CardGameAIController->AICardsInHand;
			ControllersState.PlayerState = CardGameAIPawn;
			return ControllersState;
		}
	}
	return ControllersState;
}

void UController_FunctionLibrary::GetControllerIDForBothPlayers_ServerOnly(AController* Controller, int32& SelfPlayer, int32& Oppenent)
{
	ACardGameMode* CardGameMode = Cast<ACardGameMode>(UGameplayStatics::GetGameMode(GWorld->GetWorld()));
	if (CardGameMode->GameControllersArray.Num() > 1)
	{
		if (GetControllerID_ServerOnly(Controller) == 1)
		{
			SelfPlayer = 1;
			Oppenent = 2;
		}
		else
		{
			SelfPlayer = 2;
			Oppenent = 1;
		}
	}
	else
	{
		SelfPlayer = 1;
		Oppenent = 1;
	}
}

AActor* UController_FunctionLibrary::GetControllerPlayerState_ServerOnly(int32 ControllerID)
{
	AController* controller = nullptr;
	ACardGamePlayerController* CardGamePlayerController;
	ACardGamePlayerState* CardGamePlayState = nullptr;
	ACardGameAIController* CardGameAIController;
	ACardGameAI_Pawn* CardGameAI_Pawn;
	controller = GetControllerReferenceFromID_ServerOnly(ControllerID, GWorld->GetWorld());
	if (GetPlayerControllerReference_ServerOnly(controller, CardGamePlayerController, CardGamePlayState))
	{
		return CardGamePlayState;
	}
	else
	{
		if (GetAIControllerReference_ServerOnly(controller, CardGameAIController, CardGameAI_Pawn))
		{
			return CardGameAI_Pawn;
		}
		else
		{
			return nullptr;
		}
	}
}

void UController_FunctionLibrary::GetReplicatedPlayerState(int32 PlayerID, FString& Name, int32& Health, int32& Mana, int32& Mana_Max, int32& CardsInHand, int32& CardsInDeck, int32& PlayerTurn)
{
	ACardGameState* CardGameState = Cast<ACardGameState>(UGameplayStatics::GetGameState(GWorld));
	if (CardGameState)
	{
		if (CardGameState->PlayerState_Array.Num() >= PlayerID)
		{
			ACardGamePlayerState* CardGamePlayerState = Cast<ACardGamePlayerState>(CardGameState->PlayerState_Array[PlayerID - 1]);
			ACardGameAI_Pawn* CardGameAI_Pawn = Cast<ACardGameAI_Pawn>(CardGameState->PlayerState_Array[PlayerID - 1]);
			if (CardGamePlayerState)
			{
				Name = CardGamePlayerState->GetPlayerName();
				Health = CardGamePlayerState->Health;
				Mana = CardGamePlayerState->Mana;
				Mana_Max = CardGamePlayerState->Mana_Max;
				CardsInHand = CardGamePlayerState->CardsInHand;
				CardsInDeck = CardGamePlayerState->CardsInDeck;
				PlayerTurn = CardGamePlayerState->PlayerTurn;
			}
			else if (CardGameAI_Pawn)
			{
				Name = CardGameAI_Pawn->AI_Name;
				Health = CardGameAI_Pawn->Health;
				Mana = CardGameAI_Pawn->Mana;
				Mana_Max = CardGameAI_Pawn->ManaMax;
				CardsInHand = CardGameAI_Pawn->CardsInHand;
				CardsInDeck = CardGameAI_Pawn->CardsInDeck;
				PlayerTurn = CardGameAI_Pawn->PlayerTurn;
			}
			else
			{
				Name = "NO_OPPONENT";
				Health = 0;
				Mana = 0;
				Mana_Max = 0;
				CardsInHand = 0;
				CardsInDeck = 0;
				PlayerTurn = 0;
			}
		}
		else
		{
			Name = "NO_OPPONENT";
			Health = 0;
			Mana = 0;
			Mana_Max = 0;
			CardsInHand = 0;
			CardsInDeck = 0;
			PlayerTurn = 0;
		}
	}
	else
	{
		Name = "NO_OPPONENT";
		Health = 0;
		Mana = 0;
		Mana_Max = 0;
		CardsInHand = 0;
		CardsInDeck = 0;
		PlayerTurn = 0;
	}
}

int32 UController_FunctionLibrary::GetOpponentID_ServerOnly(int32 PlayerID)
{
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GWorld->GetWorld());
	ACardGameMode* CardGameMode = Cast<ACardGameMode>(GameMode);
	if (CardGameMode->GameControllersArray.Num() > 1)
	{
		if (PlayerID == 1)
		{
			return 2;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return 1;
	}
}

