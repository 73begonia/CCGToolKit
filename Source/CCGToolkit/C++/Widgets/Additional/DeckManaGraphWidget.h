// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VerticleManaBarWidget.h"
#include "DeckManaGraphWidget.generated.h"

/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UDeckManaGraphWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetupGraphPrerequisites();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateGraphData(const TArray<int32>& Array, float Average);
};
