// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/SaveGame.h"
#include "SaveGame_FunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API USaveGame_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static void RemoveSaveGameSlot(FString SaveGame);

	UFUNCTION(BlueprintCallable)
	static bool CreateOrLoadCardGameSave(FString SlotName, TSubclassOf<USaveGame> SaveGameClass, USaveGame*& SaveGame);

	UFUNCTION(BlueprintCallable)
	static bool SaveCustomDeck(FText InText, TArray<FName> CustomDeck, bool bEditable);

	UFUNCTION(BlueprintCallable)
	static bool LoadCustomDeck(FString DeckName, TArray<FName>& CustomDeck, bool& bDeckEditable);

	UFUNCTION(BlueprintCallable)
	static bool GetRandomDeck(FString& RandomDeck);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool GetDeckValid(FString SlotName);

	UFUNCTION(BlueprintCallable)
	static bool GetDeckEditable(FString SlotName);
};
