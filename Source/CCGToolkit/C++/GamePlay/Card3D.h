// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../CardGame/Structs/Structs.h"
#include "../Interface/CardInteraction_Interface.h"
#include "../Enums/CardGameEnums.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/SceneComponent.h"
#include "../../CardGame/WHOO.h"
#include "Card3D.generated.h"

class ACardPlacement;
class ACardGameState;
class ACardGamePlayerController;
class UMaterial;
class UMaterialInstance;
class UMaterialInstanceDynamic;
class UCardPreviewWidget;


UCLASS()
class CCGTOOLKIT_API ACard3D : public AActor, public ICardInteraction_Interface, public IWHOO
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACard3D();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void ReceiveActorBeginCursorOver();

	virtual void ReceiveActorEndCursorOver();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaceCardOnBoard_C(FVector Destination, FRotator Rotation);

	UFUNCTION(BlueprintNativeEvent)
	void CloseGate_C();

	UFUNCTION(BlueprintNativeEvent)
	void OpenGate_C();

	void SwitchPreviewMode();

	bool RunCardAbility(int32 AbilityIndex);

	void SetCardData(FName CardTableName, ECardSet InCardSet, FSCard CardStruct, bool SetCardDataFromStruct);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool InterpSelfToLocation(FVector& ReturnLocation);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool InterpSelfToRotation(FRotator& ReturnRotation);

	void SetCardVisuals();

	WHCoroTask* CoroMoveToGravyard();

	WHCoroTask* CoreMoveCardtoGravyard();

	WHCoroTask* CoroAbilityColourChange();

	WHCoroTask* CoroRemoveCardActor();

	WHCoroTask* DamageColourChange2();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GenerateDescription(const FString& DescriptionText);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateVisualStats_C();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_MoveToGraveyard_C();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Attacking_C(AActor* Attacking);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ArrivedAtDestination_C();

	UFUNCTION(Server, Reliable)
	void Server_ActivateAbility_C(int32 AbilityIndex);

	UFUNCTION(Server, Reliable)
	void Server_SetCardData_C(FName CardTableName, ECardSet CardRace, FSCard CardStruct, bool SetCardDataFromStruct);

	UFUNCTION(NetMulticast, Reliable)
	void Mutlicast_SetCardVisuals_C();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_DamageVisual_C();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnEffect_C(ECardEffects CardEffect);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateCardVisual_C(bool IsActive);

	void SetClientSideData_C(FName CardTableName, ECardSet CardRace);

	UFUNCTION(NetMulticast, Reliable)
	void RemoveCardActor_C();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_AbilityVisual_C();

	UFUNCTION(NetMulticast, Reliable)
	void SetCardHiddenState_C(bool bNewHidden);
	
	void SpawnEffect_C(ECardEffects CardEffect);

	UFUNCTION(NetMulticast, Reliable)
	void ForceMoveCardDirectlyToGraveyard_C(AGraveyard* Graveyard);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool TakesDamage_C(ACard3D* DealingCard, int32 DamageAttack, int32 DamageHealth);

	virtual bool TakesDamage_C_Implementation(ACard3D* DealingCard, int32 DamageAttack, int32 DamageHealth);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool OnActivePlayerTurn_C();

	virtual bool OnActivePlayerTurn_C_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool OnEndActivePlayerTurn_C();

	virtual bool OnEndActivePlayerTurn_C_Implementation();

	void Event_Deselected_C();

	void Event_Selected_C(int32 SelectingPlayerID);

	void DisableMobileCardPreview_C();

	void EnableMobileCardPreview_C();

	void SetTurnStateVisual(bool InIsActive);

	void ChangeTurnStateVisual(ETurnState InCardGameState);

	UFUNCTION(BlueprintCallable)
	bool CheckAblilityUseState(EAbilityTrigger CheckAbilityTrigger, ECardUseState CheckCardUseState, int32 AbilityIndex, bool SearchAllAbilities);

	bool RunActiveAbility(EAbilityTrigger InTrigger);

	void MoveCardToGravyard();

	void PlayMovementAnimation(E3DCardMovementState InCardMovementState, FVector ManualGoalDestination, FRotator ManualGoalRotation, AActor* ORGoalActor, float ZOffset, float InErrorTolerance, float InInterpSpeed);

	void RemoveMouseOverPreview();

	void SaveCardStateToStruct();

public:

	UPROPERTY(BlueprintReadWrite, Category = "Visual")
	UMaterial* OuterMaterial;

	UPROPERTY(BlueprintReadWrite, Category = "Visual")
	UMaterial* Shadow_Mat_Ref;

	UMaterial* Card3DNothing;

	UMaterial* Card_Shadow_Glow_Mat;

	UMaterial* Frame_Material;

	UMaterial* Image_Material;

	UMaterial* Back_material;

	UMaterialInstanceDynamic* CardGreenGlow_Mat;

	UMaterialInstanceDynamic* CardRedGlow_Mat;

	UPROPERTY(BlueprintReadWrite, Category = "Visual", Replicated)
	UMaterialInstance* CardFrameMaterial;

	UPROPERTY(BlueprintReadWrite, Category = "Visual")
	UMaterialInstanceDynamic* CardFrameMaterial_Instance;

	UPROPERTY(BlueprintReadWrite, Category = "Visual", Replicated)
	UMaterialInstance* CardImageMaterial;

	UPROPERTY(BlueprintReadWrite, Category = "Visual")
	UMaterialInstanceDynamic* CardImageMaterial_Instance;

	UPROPERTY(BlueprintReadWrite, Category = "Visual")
	UCardPreviewWidget* ViewCardPreview;

	UPROPERTY(BlueprintReadWrite, Category = "Visual", Replicated)
	UMaterialInstanceDynamic* CardBackMaterial;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Abilities")
	TArray<EAbilityTrigger> AbilityTriggers;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Abilities")
	TArray<EAbilityType> AbilityTypes;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Abilities")
	TArray<FSAbility> Ability_Struct_Refs;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Abilities")
	bool IsAbilityActive = true;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Misc Variables")
	FVector HomeDestination;

	UPROPERTY(BlueprintReadWrite, Category = "Misc Variables")
	FVector GoalDestination;

	UPROPERTY(BlueprintReadWrite, Category = "Misc Variables")
	FRotator GoalRotation;

	UPROPERTY(BlueprintReadWrite, Category = "Misc Variables")
	FRotator HomeRotation;

	UPROPERTY(BlueprintReadWrite, Category = "System")
	float CardDeltaTime;

	UPROPERTY(BlueprintReadWrite, Category = "System")
	float InterpSpeed = 15.0;

	UPROPERTY(BlueprintReadWrite, Category = "System")
	float ErrorTolerance = 0.001;

	UPROPERTY(BlueprintReadWrite, Category = "System")
	float CardPreviewScale = 0.7;

	UPROPERTY(BlueprintReadWrite, Category = "System")
	int32 MaxDescriptionCharacterLineLength = 43;

	UPROPERTY(BlueprintReadWrite, Category = "System")
	EPlatform Platform = EPlatform::Windows;

	UPROPERTY(BlueprintReadWrite, Category = "System")
	E3DCardMovementState CardMovementState = E3DCardMovementState::Placing;

	UPROPERTY(BlueprintReadWrite, Category = "System")
	ETurnState CardGameState = ETurnState::TurnActive;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "System")
	ACardPlacement* PlacementOwner;

	UPROPERTY(BlueprintReadWrite, Category = "System")
	ACard3D* DamageDealingCard_Ref;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "System")
	ACardGamePlayerController* CardOwningPlayer;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "System")
	bool bIsPlaced;

	UPROPERTY(BlueprintReadWrite, Category = "System")
	bool bIsSelected;

	UPROPERTY(BlueprintReadWrite, Category = "System")
	bool bIsAttacking = true;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "System")
	bool bInGraveyard;

	UPROPERTY(BlueprintReadWrite, Category = "System")
	bool bIsPlatformMobile;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "System")
	bool bEnableLifeExpectancy;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_bCardActive_C, Category = "System")
	bool bCardActive;

	UFUNCTION()
	void OnRep_bCardActive_C();

	UPROPERTY(BlueprintReadWrite, Category = "System")
	ACardGameState* GameState_Ref;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_OwningPlayerID_C, Category = "System")
	int32 OwningPlayerID = 0;

	UFUNCTION()
	void OnRep_OwningPlayerID_C();

	UPROPERTY(BlueprintReadWrite, Category = "System")
	int32 CardID = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Preview")
	bool PreviewEnabled;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Card Data")
	FSCard CardDataStruct;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Card Data")
	FName CardDataTableName = "none";

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Card Data")
	FText Name;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_Attack_C, Category = "Card Data")
	int32 Attack;

	UFUNCTION()
	void OnRep_Attack_C();

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_Health_C, Category = "Card Data")
	int32 Health;

	UFUNCTION()
	void OnRep_Health_C();

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_Mana_Cost_Placement_C, Category = "Card Data")
	int32 Mana_Cost_Placement;

	UFUNCTION()
	void OnRep_Mana_Cost_Placement_C();

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Card Data")
	int32 TurnPoints = 1;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Card Data")
	int32 LifeExpectancy;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_TurnsAlive_C, Category = "Card Data")
	int32 TurnsAlive;

	UFUNCTION()
	void OnRep_TurnsAlive_C();

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_Type_C, Category = "Card Data")
	ECardType Type = ECardType::Creature;

	UFUNCTION()
	void OnRep_Type_C();

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Card Data")
	ECardSet CardSet = ECardSet::Empty;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Card Data")
	FString Description;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Card Data")
	bool CanAttackPlayer;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Card Data")
	bool CanAttackCards;

	bool bOpenGate = false;

	FTimerHandle SwitchPreviewHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* Box;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* DefaultSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* CardBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ManaGem_Plane;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Heart_Plane;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Attack_Plane;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTextRenderComponent* ManaCost_Text;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTextRenderComponent* Health_Text;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTextRenderComponent* Attack_Text;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTextRenderComponent* Description_Text;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTextRenderComponent* CardType_Text;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTextRenderComponent* Name_Text;
};
