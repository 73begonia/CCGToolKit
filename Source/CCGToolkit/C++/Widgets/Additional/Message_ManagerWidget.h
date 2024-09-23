// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Message_ManagerWidget.generated.h"

/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UMessage_ManagerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void LogMessages_C(APlayerController* CallingPlayer, const FString& Message, FLinearColor SpecifiedColor);
};
