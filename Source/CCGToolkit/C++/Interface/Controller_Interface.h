// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../GamePlay/Card3D.h"
#include "Math/TransformNonVectorized.h"
#include "Controller_Interface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UController_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CCGTOOLKIT_API IController_Interface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	ACard3D* CreatePlayableCard_C(FTransform SpawnTransform);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool AddCardToPlayersHand_C(FName CardName);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool BeginPlayerTurn_C();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool EndPlayerTurn_C();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool UpdateUI_C();

};
