// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCGToolkit/C++/Enums/CardGameEnums.h"
#include "DeckStatsWidget.generated.h"

/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UDeckStatsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void LoadDeck();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetGraphData();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetStatValues();

};
