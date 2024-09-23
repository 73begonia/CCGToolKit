// Fill out your copyright notice in the Description page of Project Settings.


#include "CardGameState.h"
#include "TimerManager.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Net/UnrealNetwork.h"
#include "FunctionLibraries/Math_Library.h"
#include "FunctionLibraries/Controller_FunctionLibrary.h"
#include "CardGameInstance.h"
#include "TimerManager.h"
#include "Interface/GameState_Interface.h"
#include "Kismet/GameplayStatics.h"
#include "FunctionLibraries/GameSnapshot_FunctionLibrary.h"
#include "FunctionLibraries/Misc_FunctionLibrary.h"
#include "SaveGame/RecordGameState_SaveGame.h"

ACardGameState::ACardGameState()
{
	bReplicates = true;
}

void ACardGameState::BeginPlay()
{
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(DelayTimerHandle, this, &ACardGameState::EventBeginPlay, 0.5f, false);
	}
}

void ACardGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACardGameState, GameTime_Seconds);
	DOREPLIFETIME(ACardGameState, GameTime_Minutes);
	DOREPLIFETIME(ACardGameState, GameTurnState);
	DOREPLIFETIME(ACardGameState, TurnTime_Seconds);
	DOREPLIFETIME(ACardGameState, TurnTime_Minutes);
	DOREPLIFETIME(ACardGameState, PlayerState_Array);
	DOREPLIFETIME(ACardGameState, bGameActive);
	DOREPLIFETIME(ACardGameState, ActiveCards_Player1);
	DOREPLIFETIME(ACardGameState, ActiveCards_Player2);
	DOREPLIFETIME(ACardGameState, CardPlacements_Player1);
	DOREPLIFETIME(ACardGameState, CardPlacements_Player2);
	DOREPLIFETIME(ACardGameState, Graveyards_Player1);
	DOREPLIFETIME(ACardGameState, Graveyards_Player2);
}

void ACardGameState::Server_RequestChangeTurnState_C_Implementation(AController* Controller)
{
	if (RequestChangeTurnState_C(Controller))
	{
		if (bGameActive)
		{
			RotatePlayerTurn_C();
			for (int32 i = 0; i < CardGameMode_Ref->GameControllersArray.Num(); i++)
			{
				IGameState_Interface* GameState_Interface = Cast<IGameState_Interface>(CardGameMode_Ref->GameControllersArray[i]);
				GameState_Interface->Execute_ChangeActivePlayerTurn_C(CardGameMode_Ref->GameControllersArray[i], CardGameMode_Ref->GameControllersArray[i] == PlayerTurn_Array[0]);
				if (CardGameMode_Ref->GameControllersArray[i] == PlayerTurn_Array[0])
				{
					int32 PlayerID = UController_FunctionLibrary::GetControllerID_ServerOnly(CardGameMode_Ref->GameControllersArray[i]);
					BeginPlayerTurn_C(PlayerID);
				}
				else
				{
					int32 PlayerID = UController_FunctionLibrary::GetControllerID_ServerOnly(CardGameMode_Ref->GameControllersArray[i]);
					EndOfPlayerTurn_C(PlayerID);
				}
			}
			RestetTurnTimer_C();
		}
	}
}

void ACardGameState::Server_ForceChangeTurnState_C_Implementation()
{
	if (HasAuthority())
	{
		if (bGameActive)
		{
			RotatePlayerTurn_C();
			for (int32 i = 0; i < CardGameMode_Ref->GameControllersArray.Num(); i++)
			{
				IGameState_Interface* GameState_Interface = Cast<IGameState_Interface>(CardGameMode_Ref->GameControllersArray[i]);
				GameState_Interface->Execute_ChangeActivePlayerTurn_C(CardGameMode_Ref->GameControllersArray[i], CardGameMode_Ref->GameControllersArray[i] == PlayerTurn_Array[0]);
				if (CardGameMode_Ref->GameControllersArray[i] == PlayerTurn_Array[0])
				{
					int32 PlayerID = UController_FunctionLibrary::GetControllerID_ServerOnly(CardGameMode_Ref->GameControllersArray[i]);
					BeginPlayerTurn_C(PlayerID);
				}
				else
				{
					int32 PlayerID = UController_FunctionLibrary::GetControllerID_ServerOnly(CardGameMode_Ref->GameControllersArray[i]);
					EndOfPlayerTurn_C(PlayerID);
				}
			}
			RestetTurnTimer_C();
		}
	}
}

void ACardGameState::Server_NotifyEndGameState_C_Implementation(EEndGameResults Player1Result, EEndGameResults Player2Result)
{
	if (HasAuthority())
	{
		bGameActive = false;
		GetWorldTimerManager().ClearTimer(TurnTimer_Ref);
		GetWorldTimerManager().ClearTimer(GameTimer_Ref);
		for (int32 i = 0; i < CardGameMode_Ref->GameControllersArray.Num(); i++)
		{
			if (i == 0)
			{
				IGameState_Interface* GameState_Interface = Cast<IGameState_Interface>(CardGameMode_Ref->GameControllersArray[i]);
				GameState_Interface->Execute_MatchEnd_C(CardGameMode_Ref->GameControllersArray[i], Player1Result);
			}
			if (i == 1)
			{
				IGameState_Interface* GameState_Interface = Cast<IGameState_Interface>(CardGameMode_Ref->GameControllersArray[i]);
				GameState_Interface->Execute_MatchEnd_C(CardGameMode_Ref->GameControllersArray[i], Player2Result);
			}
			FTimerDelegate TimerDel;
			CardGameInstance = Cast<UCardGameInstance>(UGameplayStatics::GetGameInstance(this));
			TimerDel.BindUFunction(CardGameInstance, "ShowMainMenu_C");
			GetWorldTimerManager().SetTimer(NotifyEndGameStateTimerHandle, TimerDel, 5.f, false);
		}
	}
}

void ACardGameState::GameTimer_C()
{
	if (GameTime_Seconds >= 59)
	{
		GameTime_Minutes++;
		GameTime_Seconds = 0;
	}
	else
	{
		GameTime_Seconds++;
		if (GameTime_Minutes >= 30)
		{
			Server_NotifyEndGameState_C(EEndGameResults::Victory, EEndGameResults::Victory);
		}
	}
}

void ACardGameState::TurnTimer_C()
{
	if (TurnTime_Seconds <= 0)
	{
		TurnTime_Minutes--;
		TurnTime_Seconds = 59;
		if (TurnTime_Minutes == 0 && TurnTime_Seconds <= 0)
		{
			Server_ForceChangeTurnState_C();
		}
	}
	else
	{
		TurnTime_Seconds--;
		if (TurnTime_Minutes == 0 && TurnTime_Seconds <= 0)
		{
			Server_ForceChangeTurnState_C();
		}
	}
}

void ACardGameState::RestetTurnTimer_C()
{
	TurnTime_Seconds = TurnDuration_Seconds;
	TurnTime_Minutes = TurnDuration_Minutes;
}

bool ACardGameState::CheckPlayerTurnState_C(EGameTurn GameTurnState_P)
{
	return (this->GameTurnState == GameTurnState_P);
}

bool ACardGameState::RequestChangeTurnState_C(AController* Controller)
{
	return (PlayerTurn_Array[0] == Controller);
}

void ACardGameState::AddCardToBoard_C(ACard3D* CardReference, int32 PlayerID)
{
	if (HasAuthority())
	{
		if (PlayerID ==1)
		{
			ActiveCards_Player1.AddUnique(CardReference);
			CardReferenceArray.Add(CardReference);
			CardReference->CardID = CardReferenceArray.Add(CardReference);
		}
		if (PlayerID ==2)
		{
			ActiveCards_Player2.AddUnique(CardReference);
			CardReferenceArray.Add(CardReference);
			CardReference->CardID = CardReferenceArray.Add(CardReference);
		}
	}
}

void ACardGameState::RemoveCardOnBoard_C(ACard3D* CardReference, int32 PlayerID)
{
	if (HasAuthority())
	{
		if (PlayerID == 1)
		{
			if (ActiveCards_Player1.Find(CardReference) >= 0)
			{
				ActiveCards_Player1.Remove(CardReference);
			}
			else
			{
				UKismetSystemLibrary::PrintString(this, "Player Does Not Own Requested Card");
			}
		}
		if (PlayerID == 2)
		{
			if (ActiveCards_Player2.Find(CardReference) >= 0)
			{
				ActiveCards_Player2.Remove(CardReference);
			}
			else
			{
				UKismetSystemLibrary::PrintString(this, "Player Does Not Own Requested Card");
			}
		}
	}
}

void ACardGameState::GetBoardState_C(int32 PlayerID, TArray<ACard3D*>& PlayerCards, TArray<ACard3D*>& OppenedCards)
{
	if (PlayerID == 1)
	{
		PlayerCards = ActiveCards_Player1;
		OppenedCards = ActiveCards_Player2;
	}
	else if(PlayerID == 2)
	{
		PlayerCards = ActiveCards_Player2;
		OppenedCards = ActiveCards_Player1;
	}
}

void ACardGameState::CompilePlacementsPerPlayer_C()
{
	TArray<AActor*> OutActors;
	UClass* CardPlacementClass = LoadClass<ACardPlacement>(NULL, TEXT("Blueprint'/Game/CCGToolkitC++/Blueprints/GamePlay/BP_CardPlacement.BP_CardPlacement_C'"));
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), CardPlacementClass, OutActors);
	for (int32 i = 0; i<OutActors.Num(); i++)
	{
		ACardPlacement* ArrayElement = Cast<ACardPlacement>(OutActors[i]);
		if (ArrayElement->PlayerIndex == 0)
		{
			CardPlacements_Player1.Add(ArrayElement);
			TotalCardPlacementPositions_Player1 += ArrayElement->MaxCardsInPlacement;

			CardPlacements_Player2.Add(ArrayElement);
			TotalCardPlacementPositions_Player2 += ArrayElement->MaxCardsInPlacement;
		}
		else
		{
			if (ArrayElement->PlayerIndex == 1)
			{
				CardPlacements_Player1.Add(ArrayElement);
				TotalCardPlacementPositions_Player1 += ArrayElement->MaxCardsInPlacement;
			}
			else if(ArrayElement->PlayerIndex == 2)
			{
				CardPlacements_Player2.Add(ArrayElement);
				TotalCardPlacementPositions_Player2 += ArrayElement->MaxCardsInPlacement;
			}
		}
	}
}

void ACardGameState::GetCardPlacementReferences_C(int32 PlayerID, TArray<ACardPlacement*>& PlacementArray, int32& Total)
{
	if (PlayerID == 1)
	{
		for (int32 i = 0; i < CardPlacements_Player1.Num(); i++)
		{
			PlacementArray.Add(CardPlacements_Player1[i]);
		}
		Total = TotalCardPlacementPositions_Player1;
	}
	if (PlayerID == 2)
	{
		for (int32 i = 0; i < CardPlacements_Player2.Num(); i++)
		{
			PlacementArray.Add(CardPlacements_Player2[i]);
		}
		Total = TotalCardPlacementPositions_Player2; 
	}
}

void ACardGameState::GetGraveyardReferencesPerPlayer_C()
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGraveyard::StaticClass(), OutActors);
	for (int32 i = 0; i< OutActors.Num(); i++)
	{
		AGraveyard* Graveyard = Cast<AGraveyard>(OutActors[i]);
		switch (Graveyard->Player_Index)
		{
		case 0:
			Graveyards_Player1 = Graveyards_Player2 = Graveyard;
			break;
		case 1:
			Graveyards_Player1 = Graveyard;
			break;
		case 2:
			Graveyards_Player2 = Graveyard;
			break;
		default:
			break;
		}
	}
	
}

AGraveyard* ACardGameState::GetGraveyardReference_C(int32 PlayerID)
{
	if (PlayerID == 1)
	{
		return Graveyards_Player1;
	}
	if (PlayerID == 2)
	{
		return Graveyards_Player2;
	}
	else
	{
		return nullptr;
	}
}

void ACardGameState::RecordGameStateSnapshot_C()
{
	UClass* RecordGameState_SaveGameClass = LoadClass<URecordGameState_SaveGame>(NULL, TEXT("Blueprint'/Game/CCGToolkitC++/Blueprints/SaveGame/RecordGameState_SaveGame_BP.RecordGameState_SaveGame_BP_C'"));
	GameSnapshot.Add(UGameSnapshot_FunctionLibrary::GetGameStateSnapshot());
	UMisc_FunctionLibrary::CustomPrint("Snapshots Recorded: ", FString::FromInt(GameSnapshot.Num()), "", "", FLinearColor::Green, 10.0, false);
	URecordGameState_SaveGame* RecordGameState_SaveGame = Cast<URecordGameState_SaveGame>(UGameplayStatics::CreateSaveGameObject(RecordGameState_SaveGameClass));
	RecordGameState_SaveGame->Snapshot = GameSnapshot;
	UGameplayStatics::SaveGameToSlot(RecordGameState_SaveGame, "HappySnaps", 0);
}

void ACardGameState::RecordBattleHistory_C(FSBattleHistory BattleHistoryStruct)
{
	if (EnableBattleHistory)
	{
		BattleHistory.Add(BattleHistoryStruct);
		//printHistoryResult...
	}
}

void ACardGameState::RotatePlayerTurn_C()
{
	/*PlayerTurn_Array.Insert(PlayerTurn_Array[0], PlayerTurn_Array.Num());
	PlayerTurn_Array.RemoveAt(0);*/
	AController* TempPlayerTurn_Array = PlayerTurn_Array[0];
	PlayerTurn_Array[0] = PlayerTurn_Array[1];
	PlayerTurn_Array[1] = TempPlayerTurn_Array;
}

void ACardGameState::EndOfPlayerTurn_C(int32 PlayerID)
{
	TArray<ACard3D*> PlayerCards;
	TArray<ACard3D*> OppenedCards;
	if (HasAuthority())
	{
		GetBoardState_C(PlayerID, PlayerCards, OppenedCards);
		for (int32 i = 0; i< PlayerCards.Num(); i++)
		{
			ICardInteraction_Interface* CardInteraction_Interface = Cast<ICardInteraction_Interface>(PlayerCards[i]);
			if (CardInteraction_Interface)
			{
				CardInteraction_Interface->Execute_OnEndActivePlayerTurn_C(PlayerCards[i]);
			}
		}
	}

}

void ACardGameState::BeginPlayerTurn_C(int32 PlayerID)
{
	TArray<ACard3D*> PlayerCards;
	TArray<ACard3D*> OppenedCards;
	if (HasAuthority())
	{
		GetBoardState_C(PlayerID, PlayerCards, OppenedCards);
		for (int32 i = 0; i < PlayerCards.Num(); i++)
		{
			ICardInteraction_Interface* CardInteraction_Interface = Cast<ICardInteraction_Interface>(PlayerCards[i]);
			if (CardInteraction_Interface)
			{
				CardInteraction_Interface->Execute_OnActivePlayerTurn_C(PlayerCards[i]);
			}
		}
	}
}

bool ACardGameState::ValidatePlayerTurnChange_C(ACardGamePlayerController* PlayerController)
{
	AController* Player1 = nullptr;
	AController* Player2 = nullptr;
	CardGameMode_Ref->GetPlayerControllers(Player1, Player2);
	if (PlayerController == Player1 && GameTurnState == EGameTurn::TurnActive)
	{
		return true;
	}
	else
	{
		if (PlayerController == Player2 && GameTurnState == EGameTurn::TurnActive)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

void ACardGameState::EventBeginPlay()
{
	ACardGameMode* CardGameMode = Cast<ACardGameMode>(UGameplayStatics::GetGameMode(this));
	if (CardGameMode)
	{
		CardGameMode_Ref = CardGameMode;
	}
	else
	{
		UKismetSystemLibrary::PrintString(this, "Game Mode Cast Failed");
	}
	CompilePlacementsPerPlayer_C();
	GetGraveyardReferencesPerPlayer_C();
}

void ACardGameState::OnGameStart_C_Implementation()
{
	if (HasAuthority())
	{
		bGameActive = true;
		PlayerState_Array = CardGameMode_Ref->PlayerStateArray;

		GetWorldTimerManager().SetTimer(TurnTimer_Ref, this, &ACardGameState::TurnTimer_C, GameSeconds, true);
		GetWorldTimerManager().SetTimer(GameTimer_Ref, this, &ACardGameState::GameTimer_C, GameSeconds, true);
		
		for (int32 i = 0; i < CardGameMode_Ref->GameControllersArray.Num(); i++)
		{
			PlayerTurn_Array.AddUnique(CardGameMode_Ref->GameControllersArray[i]);
		}

		UMath_Library::ShuffleArray(PlayerTurn_Array);

		for (int32 i = 0; i < PlayerTurn_Array.Num(); i++)
		{
			IGameState_Interface* GameState_Interface = Cast<IGameState_Interface>(PlayerTurn_Array[i]);
			GameState_Interface->Execute_MatchBegin_C(PlayerTurn_Array[i]);
		}
		for (int32 i = 0; i < CardGameMode_Ref->GameControllersArray.Num(); i++)
		{
			IGameState_Interface* GameState_Interface = Cast<IGameState_Interface>(CardGameMode_Ref->GameControllersArray[i]);
			GameState_Interface->Execute_ChangeActivePlayerTurn_C(CardGameMode_Ref->GameControllersArray[i], CardGameMode_Ref->GameControllersArray[i] == PlayerTurn_Array[0]);
			if (CardGameMode_Ref->GameControllersArray[i] == PlayerTurn_Array[0])
			{
				int32 PlayerID = UController_FunctionLibrary::GetControllerID_ServerOnly(CardGameMode_Ref->GameControllersArray[i]);
				BeginPlayerTurn_C(PlayerID);
			}
			else
			{
				int32 PlayerID = UController_FunctionLibrary::GetControllerID_ServerOnly(CardGameMode_Ref->GameControllersArray[i]);
				EndOfPlayerTurn_C(PlayerID);
			}
		}
		RestetTurnTimer_C();
	}
}

