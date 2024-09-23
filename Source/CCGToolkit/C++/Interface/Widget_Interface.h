// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../Enums/CardGameEnums.h"
#include "Components/Widget.h"
#include "FindSessionsCallbackProxy.h"
#include "Widget_Interface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWidget_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CCGTOOLKIT_API IWidget_Interface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ReturnOption(ECardRowOptions Option, const FString& CardSet, UWidget* CallingWidget);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ReturnSearch(const FText& SearchText);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ReturnCardSet(ECardSet CardSet);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ReturnPreview(bool PreviewEnabled);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ReturnServerSelection(const FBlueprintSessionResult SessionResult);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ReturnDeckSelection(const FString& DeckSelection);
};
