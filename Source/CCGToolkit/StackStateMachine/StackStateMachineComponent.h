// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../C++/Interface/StackState_Interface.h"
#include "../C++/Enums/CardGameEnums.h"
#include "StackStateMachineComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStackStatePushPopSignature, TScriptInterface<IStackState_Interface>, State);

DECLARE_LOG_CATEGORY_CLASS(LogStackStateMachine, Log, All);

UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class CCGTOOLKIT_API UStackStateMachineComponent : public UActorComponent, public IStackState_Interface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStackStateMachineComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "StackStateMachine")
	void PushState(TScriptInterface<IStackState_Interface> NewState);

	UFUNCTION(BlueprintCallable, Category = "StackStateMachine")
	void PopState();

	UFUNCTION(BlueprintCallable, Category = "StackStateMachine")
	void PopStates(int32 Count);

	UFUNCTION(BlueprintCallable, Category = "StackStateMachine")
	void PopAllStates();

	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void EnterState(EStackAction StackAction) override;
	virtual void ExitState(EStackAction StackAction) override;
	virtual void UpdateState(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "StackStateMachine", meta = (DisplayName = "OnEnterState"))
	void ReceiveEnterState(EStackAction StackAction);

	UFUNCTION(BlueprintImplementableEvent, Category = "StackStateMachine", meta = (DisplayName = "OnExitState"))
	void ReceiveExitState(EStackAction StackAction);

	UFUNCTION(BlueprintImplementableEvent, Category = "StackStateMachine", meta = (DisplayName = "OnUpdateState"))
	void ReceiveUpdateState(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent, Category = "StackStateMachine", meta = (DisplayName = "StatePushed"))
	void ReceiveStatePushed(const TScriptInterface<IStackState_Interface>& PushedState);

	UFUNCTION(BlueprintImplementableEvent, Category = "StackStateMachine", meta = (DisplayName = "StatePopped"))
	void ReceiveStatePopped(const TScriptInterface<IStackState_Interface>& PoppedState);

	UPROPERTY(BlueprintAssignable, Category = "StackStateMachine")
	FOnStateEnterSignature OnStateEnter;

	UPROPERTY(BlueprintAssignable, Category = "StackStateMachine")
	FOnStateExitSignature OnStateExit;

	UPROPERTY(BlueprintAssignable, Category = "StackStateMachine")
	FOnStackStatePushPopSignature OnStatePushed;

	UPROPERTY(BlueprintAssignable,Category = "StackStateMachine")
	FOnStackStatePushPopSignature OnStatePopped;

	UFUNCTION(BlueprintPure, Category = "StackStateMachine")
	TScriptInterface<IStackState_Interface> GetCurrentState() { return States[StateCount() - 1]; }

	UFUNCTION(BlueprintPure, Category = "StackStateMachine")
	TScriptInterface<IStackState_Interface> GetUnderState() { return States[StateCount() - 2]; }

	UFUNCTION(BlueprintPure, Category = "StackStateMachine")
	TScriptInterface<IStackState_Interface> GetState(int32 Index) { return States[Index]; }

	UFUNCTION(BlueprintPure, Category = "StackStateMachine")
	int32 StateCount() const { return States.Num(); }

	UFUNCTION(BlueprintPure, Category = "StackStateMachine")
	FORCEINLINE float GetCurrentStateTime() const { return CurrentStateTime; }

protected:
	virtual void StatePushed(TScriptInterface<IStackState_Interface> PushedState);
	virtual void StatePopped(TScriptInterface<IStackState_Interface> PoppedState);

	TArray<TScriptInterface<IStackState_Interface>> States;

	UPROPERTY(VisibleAnywhere)
	float CurrentStateTime = 0;
};
