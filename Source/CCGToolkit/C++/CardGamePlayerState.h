// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GamePlay/Card3D.h"
#include "Interface/PlayerState_Interface.h"
#include "Widgets/GamePlay/CardWidget.h"
#include "CardGamePlayerController.h"
#include "CardGameMode.h"
#include "CardGamePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API ACardGamePlayerState : public APlayerState , public IPlayerState_Interface
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_NotifyPlayerStateChange_C();

	void Server_NotifyPlayerStateChange_C_Implementation();

	UFUNCTION(NetMulticast, Unreliable)
	void ForceUpdateUI_C();

	void ForceUpdateUI_C_Implementation();

	void InitAttributes_C();

	void InitMana_C();

	void InitHealth_C();

	void UpdatePlayersCardStates_C(int32 CardsInHand, int32 CardsInDeck, int32 ActiveCards);

	void Set_Mana_C(int32 Mana);

	ACard3D* CreatePlayableCard_C(FTransform SpawnTransform);

	bool AddCardToPlayersHand_C(FName CardName);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 IncreasePlayerHealth_C(int32 IncreaseBy);

	virtual int32 IncreasePlayerHealth_C_Implementation(int32 IncreaseBy) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 DecreasePlayerHealth_C(int32 DecreaseBy);

	virtual int32 DecreasePlayerHealth_C_Implementation(int32 DecreaseBy) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 IncreasePlayerMana_C(int32 IncreaseBy);

	virtual int32 IncreasePlayerMana_C_Implementation(int32 IncreaseBy) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 DecreasePlayerMana_C(int32 DecreaseBy);

	virtual int32 DecreasePlayerMana_C_Implementation(int32 DecreaseBy) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetManaForTurn_C();

	virtual int32 GetManaForTurn_C_Implementation() override;

	void UpdateUI_C();

protected:
	ACardGamePlayerState();

public:
	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 CardGamePlayerId = 0;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PlayerTurn_C)
	int32 PlayerTurn = 0;

	UFUNCTION()
	void OnRep_PlayerTurn_C();

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana_C)
	int32 Mana;

	UFUNCTION()
	void OnRep_Mana_C();

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana_Max_C)
	int32 Mana_Max;

	UFUNCTION()
	void OnRep_Mana_Max_C();

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PlayerHealth_C)
	int32 Health;

	UFUNCTION()
	void OnRep_PlayerHealth_C();

	UPROPERTY()
	int32 Health_Max;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CardsInHand_C)
	int32 CardsInHand;

	UFUNCTION()
	void OnRep_CardsInHand_C();

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CardsInDeck_C)
	int32 CardsInDeck;

	UFUNCTION()
	void OnRep_CardsInDeck_C();

	UPROPERTY(ReplicatedUsing = OnRep_ActiveCards_C)
	int32 ActiveCards;

	UFUNCTION()
	void OnRep_ActiveCards_C();

	UPROPERTY(ReplicatedUsing = OnRep_CardsInHand_Array_C)
	TArray<UCardWidget*> CardsInHand_Array;

	UFUNCTION()
	void OnRep_CardsInHand_Array_C();

	UPROPERTY(Replicated)
	ACardGamePlayerController* OwningPlayerController;

	ACardGameMode* GameMode_Ref;

	int32 MaxCardsInDeck = 0;

	bool ForceUIUpdateOnReplication;

};
