// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StackStateMachineComponent.h"
#include "UIState.h"
#include "UIManagerComponent.generated.h"


DECLARE_LOG_CATEGORY_CLASS(LogUIManager, Log, All);

UENUM(BlueprintType)
enum EUIStateCastResult {Success, Failed};
/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UUIManagerComponent : public UStackStateMachineComponent
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, Category = "UIManager")
	void InitUIStates();

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "UIManager", meta = (DisplayName = "GetUI"))
	UUIState* GetUI(FName UIName);

	UFUNCTION(BlueprintCallable, Category = "UIManager", meta = (DisplayName = "PushUIState"))
	void PushUIState(FName UIName);

	UFUNCTION(BlueprintCallable, Category = "UIManager", meta = (ExpandEnumAsExecs = "IsValid", DeterminesOutputType = "StateClass", DynamicOutputParam = "OutState"))
	void GetCurrentUIState(TSubclassOf<UUIState> StateClass, TEnumAsByte<EUIStateCastResult>& IsValid, UUIState*& OutState);

	UPROPERTY(EditDefaultsOnly, Category = "UIManager")
	TMap<FName, TSubclassOf<UUIState>> UIStates;

	void BeginPlay() override;
private:
	UPROPERTY()
	TMap<FName, UUIState*> UIInstances;
};
