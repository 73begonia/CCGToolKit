// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "../../CardGame/Structs/Structs.h"
#include "../GamePlay/Card3D.h"
#include "GameSnapshot_FunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UGameSnapshot_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static FSGameStateSnapshot GetGameStateSnapshot();

	static FSPlay GenerateSimulationPlayList(int32 ControllerID);

	UFUNCTION(BlueprintCallable)
	static TArray<FSCardInteraction> GenerateCardSnapshotSimulation(ACard3D* Card, int32 ControllerID);

	static bool SimulateAttackCards(int32 CallingPlayerID, ACard3D* CallingCard, ACard3D* RecievingCard, int32 PtsForDamage, int32 PtsForRemovingCardFromPlay, bool IncludeCardValue, FSCardInteraction& ReturnInteraction, bool& RemovedFromPlay);

	static bool SimulateAttackPlayer(int32 CallingPlayerID, ACard3D* CallingCard, int32 RecievingPlayerID, int32 PtsForPlayerDamage, int32 PtsForRemovingPlayer, FSCardInteraction& ReturnInteraction, bool& RemovesPlayer);

	static bool SimulateAttackOwnedCard(int32 CallingPlayerID, ACard3D* CallingCard, ACard3D* RecievingCard, int32 AttackOwnCardDefaultValue, FSCardInteraction& ReturnInteraction);
};
