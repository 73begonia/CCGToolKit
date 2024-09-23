// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CustomDeckSave.generated.h"

/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UCustomDeckSave : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	TArray<FName> CustomDeck;

	UPROPERTY(BlueprintReadWrite)
	bool bEditable;
};
