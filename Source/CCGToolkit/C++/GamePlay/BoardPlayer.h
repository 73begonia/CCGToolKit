// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Enums/CardGameEnums.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "AutoRotateToPlayer.h"
#include "Components/ChildActorComponent.h"
#include "Components/BillboardComponent.h"
#include "../CardGameCamera.h"
#include "BoardPlayer.generated.h"

class AController;

UCLASS()
class CCGTOOLKIT_API ABoardPlayer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoardPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent)
	void ScaleHealthIcon(bool Decrease);

	UFUNCTION(BlueprintNativeEvent)
	void UpdatePlayerStats();

	UFUNCTION()
	void OnRep_PlayerHealth_C();

	UFUNCTION(Server, Reliable)
	void Update_Health_C(int32 Health);
	void Update_Health_C_Implementation(int32 Health);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_BoardPlayerEffect_C(EBoardPlayerEffects Effect);

	void Multicast_BoardPlayerEffect_C_Implementation(EBoardPlayerEffects Effect);

public:
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Player")
	int32 PlayerIndex;

	UPROPERTY(BlueprintReadOnly, Replicated, EditAnywhere, Category = "Player")
	FText PlayerName;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerHealth_C, BlueprintReadWrite)
	int32 PlayerHealth;

	AController* OwningController;

	UPROPERTY(BlueprintReadWrite)
	FLinearColor HealthIncreaseColour;

	UPROPERTY(BlueprintReadWrite)
	FLinearColor HealthDecreaseColour;

	UPROPERTY(BlueprintReadWrite)
	FLinearColor Temp_HealthTextColour;

	UPROPERTY(BlueprintReadWrite)
	bool bScaleingHealthIcon;

	UPROPERTY(BlueprintReadWrite)
	FVector Temp_HealthIconRelativeScale;

	FTimerHandle MulticastBoardDelayTimerHandle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Player")
	ACardGameCamera* CardGameCamera_Ref;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* DefaultSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Cube;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* Box;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UTextRenderComponent* PlayerText;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBillboardComponent* Billboard;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UArrowComponent* Arrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Heart_Icon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UTextRenderComponent* HealthText;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UChildActorComponent* PlayerTile;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMesh;

private:
	UPROPERTY(VisibleAnywhere)
	UAutoRotateToPlayer* AutoRotateToPlayer;
};
