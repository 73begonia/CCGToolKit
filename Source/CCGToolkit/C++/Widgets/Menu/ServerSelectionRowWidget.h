// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../Interface/Widget_Interface.h"
#include "ServerSelectionRowWidget.generated.h"

/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UServerSelectionRowWidget : public UUserWidget, public IWidget_Interface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ReturnServerSelection(const FBlueprintSessionResult SessionResult);

	virtual void ReturnServerSelection_Implementation(const FBlueprintSessionResult SessionResult) override;
};
