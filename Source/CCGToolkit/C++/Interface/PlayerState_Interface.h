// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerState_Interface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerState_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CCGTOOLKIT_API IPlayerState_Interface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 IncreasePlayerHealth_C(int32 IncreaseBy);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 DecreasePlayerHealth_C(int32 DecreaseBy);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 IncreasePlayerMana_C(int32 IncreaseBy);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 DecreasePlayerMana_C(int32 DecreaseBy);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetManaForTurn_C();
};
