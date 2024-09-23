// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CardGameSave.generated.h"

/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UCardGameSave : public USaveGame
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadWrite)
	TArray<FString> DeckList;
};
