// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../C++/Interface/StackState_Interface.h"
#include "../C++/Enums/CardGameEnums.h"
#include "UIState.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class CCGTOOLKIT_API UUIState : public UUserWidget, public IStackState_Interface
{
	GENERATED_BODY()
	
public:
	virtual void EnterState(EStackAction StackAction) override;
	virtual void ExitState(EStackAction StackAction) override;
	virtual void UpdateState(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, Category = "StackState", meta = (DisplayName = "OnEnterState"))
	void ReceiveEnterState(EStackAction StackAction);

	UFUNCTION(BlueprintNativeEvent, Category = "StackState", meta = (DisplayName = "OnExitState"))
	void ReceiveExitState(EStackAction StackAction);

	UFUNCTION(BlueprintNativeEvent, Category = "StackState", meta = (DisplayName = "OnUpdateState"))
	void ReceiveUpdateState(float DeltaTime);
};
