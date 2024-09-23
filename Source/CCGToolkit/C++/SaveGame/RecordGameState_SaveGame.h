// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CCGToolkit/CardGame/Structs/Structs.h"
#include "RecordGameState_SaveGame.generated.h"

/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API URecordGameState_SaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	TArray<FSGameStateSnapshot> Snapshot;
};
