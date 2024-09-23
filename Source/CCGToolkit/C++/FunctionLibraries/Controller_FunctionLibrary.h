// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Controller_FunctionLibrary.generated.h"

class AController;
class ACardGamePlayerController;
class ACardGamePlayerState;
class ACardGameAIController;
class ACardGameAI_Pawn;
/**
 * 
 */
USTRUCT(BlueprintType)
struct FControllersState
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ControllerID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumCardsInHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CardsInDeck;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ActiveCards;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Mana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ManaMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PlayerTurn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Deck;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> CardsInHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* PlayerState;
};

UCLASS()
class CCGTOOLKIT_API UController_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool GetPlayerControllerReference_ServerOnly(AController* Controller, ACardGamePlayerController*& CardGamePlayerController, ACardGamePlayerState*& CardGamePlayerState);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static int32 GetControllerID_ServerOnly(AController* Controller);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool GetAIControllerReference_ServerOnly(AController* Controller, ACardGameAIController*& CardGameAIController, ACardGameAI_Pawn*& CardGameAIPawn);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static AController* GetControllerReferenceFromID_ServerOnly(int32 ControllerID, UWorld* TheWorld);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FControllersState GetControllersStateProfile_ServerOnly(int32 ControllerID);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static void GetControllerIDForBothPlayers_ServerOnly(AController* Controller, int32& SelfPlayer, int32& Oppenent);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static AActor* GetControllerPlayerState_ServerOnly(int32 ControllerID);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static void GetReplicatedPlayerState(int32 PlayerID, FString& Name, int32& Health, int32& Mana, int32& Mana_Max, int32& CardsInHand, int32& CardsInDeck, int32& PlayerTurn);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static int32 GetOpponentID_ServerOnly(int32 PlayerID);
};

