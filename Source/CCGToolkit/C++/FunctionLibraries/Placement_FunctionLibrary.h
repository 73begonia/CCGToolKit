// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "../GamePlay/CardPlacement.h"
#include "CCGToolkit/C++/Enums/CardGameEnums.h"
#include "Placement_FunctionLibrary.generated.h"


/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UPlacement_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static bool GetPlayersAvailablePlacementsForCard(int32 PlayerID, FName CardName,TArray<ACardPlacement*>& ReturnAvailablePlacements);

	static bool GetClosestAvailablePlacement(ACardPlacement* Placement, FName CardName, ECardSet CardSet, ACardPlacement*& ReturnPlacement);

	static bool GetIsPlacementAvailable(ACardPlacement* Placement, FName CardName);

	UFUNCTION(BlueprintCallable)
	static bool GetIsValidPlacement(ACardPlacement* Placement, FName CardName, ECardSet CardSet, bool& ValidPlacement);

	static bool GetDoesPlacementSupportCardType(ACardPlacement* Placement, FName CardName);

	static ACardPlacement* FindChosenPlacementleft(ACardPlacement* ChosenPlacement, ACardPlacement* Placement, FName CardName, ECardSet CardSet, bool& LeftValidPath);

	static ACardPlacement* FindChosenPlacementRight(ACardPlacement* ChosenPlacement, ACardPlacement* Placement, FName CardName, ECardSet CardSet, bool& RightValidPath);
};
