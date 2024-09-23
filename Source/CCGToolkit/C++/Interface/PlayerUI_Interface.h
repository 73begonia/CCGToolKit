// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CCGToolkit/C++/Enums/CardGameEnums.h"
#include "PlayerUI_Interface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerUI_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CCGTOOLKIT_API IPlayerUI_Interface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateUIPlayerStats_C(bool ForceCleanVisualUpdate);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateUITurnState_C(bool TurnActive, EGameTurn TurnState);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool UpdateUIGameTurnTime_C();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool UpdatePlayerStateUI_C(int32 NewParam, int32 NewParam1, int32 NewParam2, int32 NewParam3);
};
