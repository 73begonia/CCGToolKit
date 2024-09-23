// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Enums/CardGameEnums.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "PlacementTiles/PlacementTile_Parent.h"
#include "CardPlacement.generated.h"

class ACard3D;

USTRUCT(BlueprintType)
struct FPlacementState
{
	GENERATED_USTRUCT_BODY()
public:

	int32 InPlayerIndex;
	bool InIsMultiCardPlacement;
	bool InIsFull;
	int32 InCardsInPlacement;
	int32 InMaxCardsInPlacement;
	bool InSupportAllCardTypes;
	TArray<ECardType> ValidCardTypesArray;

};

UCLASS()
class CCGTOOLKIT_API ACardPlacement : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACardPlacement();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Reliable)
	void Event_RemoveCardFromPlacement_C(ACard3D* CardToClear);

	UFUNCTION(Server, Reliable)
	void AddCardToPlacement_C(ACard3D* CardToAdd);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
	void GetCardInPlacementLocation_C(int32 CardIndex, int32 MaxCards, int32 CardsInPlacement, FVector& ReturnVector, FRotator& ReturnRotation);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RemoveCardFromPlacement_C(ACard3D* CardToClear);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void CompileSupportedCardTypes_C();

	UFUNCTION(BlueprintCallable)
	FPlacementState GetPlacementState_C();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	bool GetClosestAvailablePlacement_C(ACardPlacement* Placement, FName CardName, ECardSet CardSet, ACardPlacement*& ReturnPlacement);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void CreateVisualBoardTiles_C();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
	bool CalculateDynamicSpacingLocation_C(int32 Index, int32 InMaxCardsInPlacement, int32 InCardInPlacement, bool CenterOut, float& InReturnValue);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
	float CalculateSpacingLocation_C(int32 Index, int32 InMaxCardsInPlacement, int32 InCardInPlacement, float Spacing);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void CalculateDynamicSpacingPosition_C(int32 InCardInPlacement, int32 InMaxCardsInPlacement, float& ReturnValue, float& ReturnValue2, float& ReturnValue3);

public:
	UPROPERTY(Replicated, BlueprintReadWrite)
	TArray<ACard3D*> Card_Array;
	
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	int32 PlayerIndex;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MaxCardsInPlacement;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	bool bMultiPlacement;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bScaleMulticardPlacement = true;

	UPROPERTY(Replicated, BlueprintReadWrite)
	bool bIsFull;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float PlacementWidth;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CardSpacing;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CardWidth;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bDynamicSpacing;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EDirection FillDirection;

	UPROPERTY(BlueprintReadWrite)
	bool bCenterOut;

	UPROPERTY(BlueprintReadWrite)
	bool bInvertPlacementDirection;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	AGraveyard* Graveyard_Ref;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bDisplayDemoCards;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 DemoCards;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bSupportsAllCardTypes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bHero;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bCreature;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bSpell;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bTrap;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAura;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bBooster;

	UPROPERTY(BlueprintReadWrite)
	TArray<ECardType> ValidCardTypes_Array;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Row_X;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Row_Y;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ACardPlacement* PlacementLeft;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ACardPlacement* PlacementRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APlacementTile_Parent> SingleTIle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APlacementTile_Parent> LeftEndTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APlacementTile_Parent> CenterTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APlacementTile_Parent> RightEndTile;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* PlacementCollision;
};
