// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "../../C++/CardGameMode.h"
#include "GameFramework/Controller.h"
#include "../../C++/Interface/PlayerState_Interface.h"
#include "../../C++/GamePlay/Card3D.h"
#include "../../CardGame/WHOO.h"
#include "../../CardGame/Structs/Structs.h"
#include "CardGameAI_Pawn.generated.h"

UCLASS()
class CCGTOOLKIT_API ACardGameAI_Pawn : public APawn, public IPlayerState_Interface, public IWHOO
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	ACardGameAI_Pawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController);

	WHCoroTask* CoroPossessed();

public:

	UFUNCTION(NetMulticast, Reliable)
	void ForceUIUpdate(int32 InCardsInHand, int32 InCardsInDeck);

	void ForceUIUpdate_Implementation(int32 InCardsInHand, int32 InCardsInDeck);

	void UpDateAIState(int32 InCardsInHand, int32 InCardsINDeck);

	void InitAttributes();

	void InitHealth();

	void InitMana();

	bool AddCardToPlayersHand();

	void CheckGameOverConditions();

	ACard3D* CreatePlayableCard(FTransform SpawnTransfrom);

	void UpdtaeUI();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetManaForTurn_C();
	virtual int32 GetManaForTurn_C_Implementation() override;

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

public:
	
	UPROPERTY(BlueprintReadWrite, Category = "PlayerData")
	FString AI_Name;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerData", ReplicatedUsing = OnRep_Health_C)
	int32 Health;

	UFUNCTION()
	void OnRep_Health_C();

	UPROPERTY(ReplicatedUsing = OnRep_Mana_C)
	int32 Mana;

	UFUNCTION()
	void OnRep_Mana_C();

	UPROPERTY(BlueprintReadWrite, Category = "PlayerData", Replicated)
	int32 ManaMax;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerData", Replicated)
	int32 HealthMax;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_CardsInHand_C)
	int32 CardsInHand;

	UFUNCTION()
	void OnRep_CardsInHand_C();

	UPROPERTY(BlueprintReadWrite, Category = "PlayerData", ReplicatedUsing = OnRep_CardsInDeck_C)
	int32 CardsInDeck;

	UFUNCTION()
	void OnRep_CardsInDeck_C();

	UPROPERTY(BlueprintReadWrite, Category = "PlayerData", Replicated)
	int32 ActiveCards;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerData", ReplicatedUsing = OnRep_PlayerTurn_C)
	int32 PlayerTurn;

	UFUNCTION()
	void OnRep_PlayerTurn_C();

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "System")
	int32 PlayerID;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "System")
	int32 CardGameAIID;

	UPROPERTY(BlueprintReadWrite, Category = "GameMode_Ref")
	ACardGameMode* GameMode_Ref;

	UPROPERTY(BlueprintReadWrite, Category = "GameMode_Ref")
	AController* OwningController_Ref;


};
