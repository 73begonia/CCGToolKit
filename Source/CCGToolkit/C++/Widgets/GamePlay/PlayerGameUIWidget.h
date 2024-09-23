// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Interface/PlayerUI_Interface.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "../Additional/Message_ManagerWidget.h"
#include "../Additional/NotificationManagerWidget.h"
#include "CardManagerWidget.h"
#include "../../CardGameState.h"
#include "../../CardGamePlayerState.h"
#include "../../Enums/CardGameEnums.h"
#include "PlayerGameUIWidget.generated.h"


class ACardGamePlayerState;
class UCardManagerWidget;
class UNotificationManagerWidget;
/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UPlayerGameUIWidget : public UUserWidget , public IPlayerUI_Interface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateUIPlayerStats_C(bool ForceCleanVisualUpdate);

	virtual void UpdateUIPlayerStats_C_Implementation(bool ForceCleanVisualUpdate);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateUITurnState_C(bool TurnActive, EGameTurn TurnState);

	virtual void UpdateUITurnState_C_Implementation(bool TurnActive, EGameTurn TurnState);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateGameAndTurnTimers_C();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void CheckTurnBeginState_C();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ToogleGameUI_C();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnClickedTurnButton_C();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnClickedToogleMessageManager_C();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnClickedSpawnAIOpponent_C();

public:

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	UCardManagerWidget* CardManagerWidget;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UNotificationManagerWidget* BP_NotificationManagerWidget;

	UPROPERTY(BlueprintReadOnly)
	UMessage_ManagerWidget* Message_ManagerWidget;
};
