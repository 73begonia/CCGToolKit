// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../Interface/PlayerUI_Interface.h"
#include "CardWidget.h"
#include "../../Enums/CardGameEnums.h"
#include "OpponentUIWidget.generated.h"

class UOpponentProfileUIWidget;
/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UOpponentUIWidget : public UUserWidget , public IPlayerUI_Interface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void RemoveWidgetFromHand(int32 Index, bool bLastIndex, bool bRemoveAll);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateCards();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateUIPlayerStats_C(bool ForceCleanVisualUpdate);

	virtual void UpdateUIPlayerStats_C_Implementation(bool ForceCleanVisualUpdate) override;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	EViewState SetCardViewState_C(EViewState CardViewState, bool Force);

};
