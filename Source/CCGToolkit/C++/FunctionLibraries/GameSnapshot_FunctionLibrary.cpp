// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSnapshot_FunctionLibrary.h"
#include "../CardGameMode.h"
#include "Controller_FunctionLibrary.h"
#include "../CardGameState.h"
#include "../../CardGame/Structs/Structs.h"
#include "Misc_FunctionLibrary.h"
#include "../../CardGameAI/AIC++/CardGameAIController.h"
#include "../../CardGameAI/AIC++/CardGameAI_Pawn.h"
#include "Math_Library.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EvaluationRules_FunctionLibrary.h"

FSGameStateSnapshot UGameSnapshot_FunctionLibrary::GetGameStateSnapshot()
{
	TArray<FSPlayerStateSnapshot> PlayerState_Array;
	TArray<int32> CardIDsArray;
	TArray<FSCard> CardStructs_Array;
	TArray<ACard3D*> PlayerCards;
	TArray<ACard3D*> OppenedCards;

	FSGameStateSnapshot GameStateSnapshot;

	ACardGameMode* CardGameMode = Cast<ACardGameMode>(GWorld->GetAuthGameMode());
	ACardGameState* CardGameState = Cast<ACardGameState>(GWorld->GetGameState());

	for (int32 i = 0; i < CardGameMode->GameControllersArray.Num(); i++)
	{
		CardStructs_Array.Empty();
		int32 ID = UController_FunctionLibrary::GetControllerID_ServerOnly(CardGameMode->GameControllersArray[i]);
		FControllersState ControllersState = UController_FunctionLibrary::GetControllersStateProfile_ServerOnly(ID);
		CardGameState->GetBoardState_C(ID, PlayerCards, OppenedCards);
		for (int32 j = 0; j< PlayerCards.Num(); j++)
		{
			CardStructs_Array.Add(PlayerCards[i]->CardDataStruct);
			CardIDsArray.Add(PlayerCards[i]->CardID);
		}

		FSPlayerStateSnapshot PlayerStateSnapshotStruct;
		PlayerStateSnapshotStruct.PlayerID = ID;
		PlayerStateSnapshotStruct.Card_Struct_Array = CardStructs_Array;
		PlayerStateSnapshotStruct.Card_IDs_Array = CardIDsArray;
		PlayerStateSnapshotStruct.Cards_In_Hand = ControllersState.CardsInHand;
		PlayerStateSnapshotStruct.Deck = ControllersState.Deck;
		PlayerStateSnapshotStruct.Health = ControllersState.Health;
		PlayerStateSnapshotStruct.Mana = ControllersState.Mana;
		PlayerStateSnapshotStruct.Turn = ControllersState.PlayerTurn;

		PlayerState_Array.Add(PlayerStateSnapshotStruct);
	}
	return GameStateSnapshot;
}

FSPlay UGameSnapshot_FunctionLibrary::GenerateSimulationPlayList(int32 ControllerID)
{
	TArray<FSPlaySub> PlayListArray;
	TArray<ACard3D*> PlayerCards;
	TArray<ACard3D*> OppenedCards;

	ACardGameState* CardGameState = Cast<ACardGameState>(GWorld->GetWorld());
	CardGameState->GetBoardState_C(ControllerID, PlayerCards,	OppenedCards);
	for (int32 i = 0; i<PlayerCards.Num();i++)
	{
		TArray<FSCardInteraction> ReturnInteractionArray = GenerateCardSnapshotSimulation(PlayerCards[i], ControllerID);
		FSPlaySub PlaySub;
		PlaySub.CardPlayStruct = ReturnInteractionArray;
		PlayListArray.Add(PlaySub);
	}
	FSPlay PlayStruct;
	PlayStruct.Card_Interactions = PlayListArray;
	return PlayStruct;
}

TArray<FSCardInteraction> UGameSnapshot_FunctionLibrary::GenerateCardSnapshotSimulation(ACard3D* Card, int32 ControllerID)
{
	TArray<FSCardInteraction> InteractionsArray;
	ACard3D* CallingCard_Ref = nullptr;
	int32 ControllerID_Ref = 0;
	TArray<ACard3D*> OpponentCards_Array;

	ACardGameAIController* CardGameAIController;
	int32 SelfPlayer, Oppenent;
	TArray<ACard3D*> PlayerCards;
	TArray<ACard3D*> OppenentCards;
	bool RemovedFromPlay;
	bool PrintPlayListToLog = false;

	int32 AttackCardArrayIndex = 0;

	int32 AttackPlayerArrayIndex = 0;

	if (Card)
	{
		CallingCard_Ref = Card;
		ControllerID_Ref = ControllerID;
		InteractionsArray.Empty();

		AController* Controller = UController_FunctionLibrary::GetControllerReferenceFromID_ServerOnly(ControllerID, GWorld->GetWorld());
		CardGameAIController = Cast<ACardGameAIController>(Controller);

		UMisc_FunctionLibrary::GetPlayerStateReference(Controller, SelfPlayer, Oppenent);
		if (CallingCard_Ref->TurnPoints > 0)
		{
			if (CardGameAIController->ValidInteractionTypes.Contains(CallingCard_Ref->Type))
			{
				ACardGameState* CardGameState = Cast<ACardGameState>(UGameplayStatics::GetGameState(CallingCard_Ref));
				CardGameState->GetBoardState_C(ControllerID_Ref, PlayerCards, OppenentCards);
				OpponentCards_Array = OppenentCards;
				UMath_Library::ShuffleArray(OpponentCards_Array);
				FSCardInteraction ReturnInteraction;


				for (int32 i = 0;i<OpponentCards_Array.Num();i++)
				{		
					bool ValidAttackCard = UGameSnapshot_FunctionLibrary::SimulateAttackCards(ControllerID_Ref, CallingCard_Ref, OpponentCards_Array[i], CardGameAIController->AIPointAllocation_Struct.Calculation.Pts_Card_Damaged, CardGameAIController->AIPointAllocation_Struct.Calculation.Pts_Card_Removed_From_Play, CardGameAIController->AIPointAllocation_Struct.Calculation.bInclude_AI_Points_Per_Card, ReturnInteraction, RemovedFromPlay);

					for (int32 j = 0; j<InteractionsArray.Num(); j++)
					{
						if (ValidAttackCard)
						{
							if (ReturnInteraction.Value > InteractionsArray[j].Value)
							{
								InteractionsArray.Insert(ReturnInteraction, j);
								AttackCardArrayIndex = j;
								break;
							}
						}
					}	
					
					if (ValidAttackCard)
					{
						if (AttackCardArrayIndex >= InteractionsArray.Num() - 1)
						{
							InteractionsArray.Add(ReturnInteraction);
						}
					}
				}
				

				bool ValidAttackPlayer = UGameSnapshot_FunctionLibrary::SimulateAttackPlayer(ControllerID_Ref, CallingCard_Ref, Oppenent, CardGameAIController->AIPointAllocation_Struct.Calculation.Pts_Per_Damage_Point_To_Player, CardGameAIController->AIPointAllocation_Struct.Calculation.Pts_Player_Removed_From_Play, ReturnInteraction, RemovedFromPlay);

				for (int32 i = 0; i<InteractionsArray.Num(); i++)
				{									
					if (ValidAttackPlayer)
					{
						if (ReturnInteraction.Value>InteractionsArray[i].Value)
						{
							AttackPlayerArrayIndex = i;
							InteractionsArray.Insert(ReturnInteraction, i);
							break;
						}
					}			
				}
				if (ValidAttackPlayer)
				{
					if (AttackPlayerArrayIndex >= (InteractionsArray.Num() - 1))
					{
						InteractionsArray.Add(ReturnInteraction);
					}
				}

				for (int32 i = 0; i < PlayerCards.Num(); i++)
				{
					if (PlayerCards[i]->PlacementOwner->PlayerIndex == ControllerID && CallingCard_Ref!=PlayerCards[i])
					{
						if (PlayerCards[i]->CardDataStruct.Card_System_Data.InteractionConditions.Contains(EInteractionConditions::CanRecieveSelfInteractionIfDoesNotRemoveCardFromPlayAIOnly) || PlayerCards[i]->CardDataStruct.Card_System_Data.InteractionConditions.Contains(EInteractionConditions::CanRecieveSelfInteractionAIOnly))
						{
							bool Valid = UGameSnapshot_FunctionLibrary::SimulateAttackOwnedCard(ControllerID_Ref, CallingCard_Ref, PlayerCards[i], CardGameAIController->AIPointAllocation_Struct.Calculation.Pts_Attack_Owned_Card_With_Value, ReturnInteraction);
							int ArrayIndex = 0;
							for (int32 j = 0; j< InteractionsArray.Num(); j++)
							{																
								if (Valid)
								{
									if (ReturnInteraction.Value> InteractionsArray[j].Value)
									{
										ArrayIndex = j;
										InteractionsArray.Insert(ReturnInteraction, j);
										break;
									}
								}
							}
							if (Valid)
							{
								if (ArrayIndex>=InteractionsArray.Num()-1)
								{
									InteractionsArray.Add(ReturnInteraction);
								}
							}
						}
					}
				}

				if (PrintPlayListToLog)
				{
					UKismetSystemLibrary::PrintString(GWorld->GetWorld(), "------------- Card: " + CallingCard_Ref->Name.ToString() + "   Play Options -------------");
					for (int32 i = 0; i < InteractionsArray.Num(); i++)
					{
						ACard3D* Card3D_1 = Cast<ACard3D>(InteractionsArray[i].RecievingActor);
						ACard3D* Card3D_2 = Cast<ACard3D>(InteractionsArray[i].TalkingCard);
						ABoardPlayer* BoardPlayer = Cast<ABoardPlayer>(InteractionsArray[i].RecievingActor);
						if (Card3D_1)
						{
							UKismetSystemLibrary::PrintString(GWorld->GetWorld(), "OwningID: " + FString::FromInt(InteractionsArray[i].OwningControllerID) + "   TalkingCard: " + Card3D_2->Name.ToString() + "    RecievingCard: " + Card3D_1->Name.ToString() + "    Value: " + FString::FromInt(InteractionsArray[i].Value), false, true, FLinearColor::Blue);
						}
						else if (BoardPlayer)
						{
							UKismetSystemLibrary::PrintString(GWorld->GetWorld(), "OwningID: " + FString::FromInt(InteractionsArray[i].OwningControllerID) + "   TalkingCard: " + Card3D_2->Name.ToString() + "    RecievingCard: " + BoardPlayer->PlayerName.ToString() + "    Value: " + FString::FromInt(InteractionsArray[i].Value), false, true, FLinearColor::Blue);
						}
					}
					UKismetSystemLibrary::PrintString(GWorld->GetWorld(), "------------------------ End Options ------------------------");
				}
				return InteractionsArray;
			}
			else if (PrintPlayListToLog)
			{
				UKismetSystemLibrary::PrintString(GWorld->GetWorld(), "------------- Card: " + CallingCard_Ref->Name.ToString() + "   Play Options -------------");
				for (int32 i = 0; i < InteractionsArray.Num(); i++)
				{
					ACard3D* Card3D_1 = Cast<ACard3D>(InteractionsArray[i].RecievingActor);
					ACard3D* Card3D_2 = Cast<ACard3D>(InteractionsArray[i].TalkingCard);
					ABoardPlayer* BoardPlayer = Cast<ABoardPlayer>(InteractionsArray[i].RecievingActor);
					if (Card3D_1)
					{
						UKismetSystemLibrary::PrintString(GWorld->GetWorld(), "OwningID: " + FString::FromInt(InteractionsArray[i].OwningControllerID) + "   TalkingCard: " + Card3D_2->Name.ToString() + "    RecievingCard: " + Card3D_1->Name.ToString() + "    Value: " + FString::FromInt(InteractionsArray[i].Value), false, true, FLinearColor::Blue);
					}
					else if (BoardPlayer)
					{
						UKismetSystemLibrary::PrintString(GWorld->GetWorld(), "OwningID: " + FString::FromInt(InteractionsArray[i].OwningControllerID) + "   TalkingCard: " + Card3D_2->Name.ToString() + "    RecievingCard: " + BoardPlayer->PlayerName.ToString() + "    Value: " + FString::FromInt(InteractionsArray[i].Value), false, true, FLinearColor::Blue);
					}
				}
				UKismetSystemLibrary::PrintString(GWorld->GetWorld(), "------------------------ End Options ------------------------");
			}
			return InteractionsArray;
		}
		else if (PrintPlayListToLog)
		{
			UKismetSystemLibrary::PrintString(GWorld->GetWorld(), "------------- Card: " + CallingCard_Ref->Name.ToString() + "   Play Options -------------");
			for (int32 i = 0; i < InteractionsArray.Num(); i++)
			{
				ACard3D* Card3D_1 = Cast<ACard3D>(InteractionsArray[i].RecievingActor);
				ACard3D* Card3D_2 = Cast<ACard3D>(InteractionsArray[i].TalkingCard);
				ABoardPlayer* BoardPlayer = Cast<ABoardPlayer>(InteractionsArray[i].RecievingActor);
				if (Card3D_1)
				{
					UKismetSystemLibrary::PrintString(GWorld->GetWorld(), "OwningID: " + FString::FromInt(InteractionsArray[i].OwningControllerID) + "   TalkingCard: " + Card3D_2->Name.ToString() + "    RecievingCard: " + Card3D_1->Name.ToString() + "    Value: " + FString::FromInt(InteractionsArray[i].Value), false, true, FLinearColor::Blue);
				}
				else if (BoardPlayer)
				{
					UKismetSystemLibrary::PrintString(GWorld->GetWorld(), "OwningID: " + FString::FromInt(InteractionsArray[i].OwningControllerID) + "   TalkingCard: " + Card3D_2->Name.ToString() + "    RecievingCard: " + BoardPlayer->PlayerName.ToString() + "    Value: " + FString::FromInt(InteractionsArray[i].Value), false, true, FLinearColor::Blue);
				}
			}
			UKismetSystemLibrary::PrintString(GWorld->GetWorld(), "------------------------ End Options ------------------------");
		}
		return InteractionsArray;
	}
	else if (PrintPlayListToLog)
	{
		UKismetSystemLibrary::PrintString(GWorld->GetWorld(), "------------- Card: " + CallingCard_Ref->Name.ToString() + "   Play Options -------------");
		for (int32 i = 0; i < InteractionsArray.Num(); i++)
		{
			ACard3D* Card3D_1 = Cast<ACard3D>(InteractionsArray[i].RecievingActor);
			ACard3D* Card3D_2 = Cast<ACard3D>(InteractionsArray[i].TalkingCard);
			ABoardPlayer* BoardPlayer = Cast<ABoardPlayer>(InteractionsArray[i].RecievingActor);
			if (Card3D_1)
			{
				UKismetSystemLibrary::PrintString(GWorld->GetWorld(), "OwningID: " + FString::FromInt(InteractionsArray[i].OwningControllerID) + "   TalkingCard: " + Card3D_2->Name.ToString() + "    RecievingCard: " + Card3D_1->Name.ToString() + "    Value: " + FString::FromInt(InteractionsArray[i].Value), false, true, FLinearColor::Blue);
			}
			else if (BoardPlayer)
			{
				UKismetSystemLibrary::PrintString(GWorld->GetWorld(), "OwningID: " + FString::FromInt(InteractionsArray[i].OwningControllerID) + "   TalkingCard: " + Card3D_2->Name.ToString() + "    RecievingCard: " + BoardPlayer->PlayerName.ToString() + "    Value: " + FString::FromInt(InteractionsArray[i].Value), false, true, FLinearColor::Blue);
			}
		}
		UKismetSystemLibrary::PrintString(GWorld->GetWorld(), "------------------------ End Options ------------------------");
	}
	return InteractionsArray;
}

bool UGameSnapshot_FunctionLibrary::SimulateAttackCards(int32 CallingPlayerID, ACard3D* CallingCard, ACard3D* RecievingCard, int32 PtsForDamage, int32 PtsForRemovingCardFromPlay, bool IncludeCardValue, FSCardInteraction& ReturnInteraction, bool& InRemovedFromPlay)
{
	int32 Total;
	int32 RuleTotal;
	FControllersState ControllersState = UController_FunctionLibrary::GetControllersStateProfile_ServerOnly(CallingPlayerID);
		
	int32 Points = UEvaluationRules_FunctionLibrary::RuleCalculateAdditionalAbilityPointOffset(CallingCard, RecievingCard);
	bool RemovefromPlay = UEvaluationRules_FunctionLibrary::RuleCalculateAttackPoints(Points, PtsForDamage, PtsForRemovingCardFromPlay, CallingCard->CardDataStruct.Attack.Damage, RecievingCard->CardDataStruct.Health.Health, RecievingCard->CardDataStruct.Card_System_Data.Card_Value, IncludeCardValue, Total, RuleTotal);

	if (CallingCard->CardDataStruct.Attack.CanAttackCards && RecievingCard->CardDataStruct.Attack.Mana_Cost <= ControllersState.Mana)
	{
		ReturnInteraction.OwningControllerID = CallingPlayerID;
		ReturnInteraction.Interaction_Type = EPlayType::AttackCard;
		ReturnInteraction.TalkingCard = CallingCard;
		ReturnInteraction.RecievingActor = RecievingCard;
		ReturnInteraction.Value = Total;
		InRemovedFromPlay = RemovefromPlay;
		return true;
	}
	return false;
}

bool UGameSnapshot_FunctionLibrary::SimulateAttackPlayer(int32 CallingPlayerID, ACard3D* CallingCard, int32 RecievingPlayerID, int32 PtsForPlayerDamage, int32 PtsForRemovingPlayer, FSCardInteraction& ReturnInteraction, bool& RemovesPlayer)
{
	FControllersState CallingPlayerControllersState = UController_FunctionLibrary::GetControllersStateProfile_ServerOnly(CallingPlayerID);
	FControllersState RecievingPlayerControllersState = UController_FunctionLibrary::GetControllersStateProfile_ServerOnly(RecievingPlayerID);
	AController* Controller = UController_FunctionLibrary::GetControllerReferenceFromID_ServerOnly(CallingPlayerID, GWorld);
	ACardGameAIController* CardGameAIController = Cast<ACardGameAIController>(Controller);
	int32 Total;
	int32 RuleTotal;
	if (CallingCard->CardDataStruct.Attack.CanAttackPlayer)
	{
		ACardGameMode* CardGameMode = Cast<ACardGameMode>(UGameplayStatics::GetGameMode(GWorld));
		if (CallingCard->CardDataStruct.Attack.Mana_Cost <= CallingPlayerControllersState.Mana)
		{
			UEvaluationRules_FunctionLibrary::RuleCalculateDamagetoPlayerPoints((CardGameAIController->PriorityFocusList_Array.Find(EAIPersonalityFocus::Damage_Opponent_Player) <= 1) ? 1 : 0, PtsForPlayerDamage, PtsForRemovingPlayer, CallingCard->CardDataStruct.Attack.Damage, RecievingPlayerControllersState.Health, Total, RuleTotal);
			ReturnInteraction.OwningControllerID = CallingPlayerID;
			ReturnInteraction.Interaction_Type = EPlayType::AttackPlayer;
			ReturnInteraction.TalkingCard = CallingCard;
			ReturnInteraction.RecievingActor = CardGameMode->BoardPlayersArray[RecievingPlayerID - 1];
			ReturnInteraction.Value = Total;
			RemovesPlayer = false;
			return true;
		}
	}
	return false;
}

bool UGameSnapshot_FunctionLibrary::SimulateAttackOwnedCard(int32 CallingPlayerID, ACard3D* CallingCard, ACard3D* RecievingCard, int32 AttackOwnCardDefaultValue, FSCardInteraction& ReturnInteraction)
{
	FControllersState ControllersState = UController_FunctionLibrary::GetControllersStateProfile_ServerOnly(CallingPlayerID);
	if (CallingCard->CardDataStruct.Attack.CanAttackCards)
	{
		if (RecievingCard->CardDataStruct.Attack.Mana_Cost <= ControllersState.Mana)
		{
			int32 Points;
			if (UEvaluationRules_FunctionLibrary::RuleCalculateOwnedCardAbilityPointOffset(CallingCard, RecievingCard, Points))
			{
				ReturnInteraction.OwningControllerID = CallingPlayerID;
				ReturnInteraction.Interaction_Type = EPlayType::AttackCard;
				ReturnInteraction.TalkingCard = CallingCard;
				ReturnInteraction.RecievingActor = RecievingCard;
				ReturnInteraction.Value = Points + AttackOwnCardDefaultValue;
				return true;
			}

		}
	}
	return false;
}
