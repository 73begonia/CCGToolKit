// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "../CardGamePlayerController.h"
#include "Log_FunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API ULog_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static void AddMessageToLog(ACardGamePlayerController* CallingPlayer, FString Message, FLinearColor SpecifiedColor);

	static FString CloneCardLogMessage(FText CardName, int32 AbilityInt);

	static FString PickupCard_LogMessage(int32 AbilityInt);

	static FString IncreaseAttack_LogMessage(FText CardName, int32 AbilityInt);
};
