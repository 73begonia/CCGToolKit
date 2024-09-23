// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardWidget.generated.h"

/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UCardWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetDefaults();
};
