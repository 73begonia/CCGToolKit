// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../Enums/CardGameEnums.h"
#include "CardManagerWidget.generated.h"

class UCardWidget;
/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UCardManagerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	EViewState SetCardViewState_C(EViewState InCardViewState, bool Force);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	UCardWidget* AddCardWidget_C(FName CardName, ECardSet FromCardSet);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	int32 RemoveCardWidgetfromHand_C(UCardWidget* CardToRemove, int32 OR_Index, bool AllCardsInHand);

public:
	UPROPERTY(BlueprintReadWrite)
	EViewState CardViewState = EViewState::Default;

};
