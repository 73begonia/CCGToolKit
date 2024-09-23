// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/ChildActorComponent.h"
#include "AutoRotateToPlayer.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "../Interface/Controller_Interface.h"
#include "Graveyard.generated.h"

class UMaterialInstanceDynamic;
class ACard3D;
UCLASS()
class CCGTOOLKIT_API AGraveyard : public AActor, public IController_Interface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGraveyard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Reliable)
	void AddToGraveyard_C(ACard3D* EnteringCard, FName OrByName);

	UFUNCTION(NetMulticast, Unreliable)
	void AddToGraveyardVisual_C();

	UFUNCTION(Server, Reliable)
	void Event_RemoveCardFromGraveyard_C(FName Card, int32 Index);

	void ScaleCardGraveyardStaticMesh_C();

	void HideUnseenCardsInGraveyard_C();

	FVector GetCardInGraveyardLocation_C(int32 add);

	void RecreateTopGraveyardCardActor_C();

	void RemoveCardFromGraveyard_C(FName CardName, int32 Index);

public:
	UMaterialInstanceDynamic* OutlineDecal_MaterialInstance;

	UPROPERTY(EditAnywhere, Category = "Default")
	int32 Player_Index;

	UPROPERTY(Replicated)
	TArray<ACard3D*> ActorsInGraveyard;

	UPROPERTY(Replicated)
	TArray<FName> GraveyardList;

	TArray<FSCard> GraveyardStructList;

	int32 RemoveIndex;

private:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* DefaultSceneComponent;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* Box;

	UPROPERTY(VisibleAnywhere)
	UTextRenderComponent* Graveyard_Text;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* SM_GraveyardMesh;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* CardPlacementPoint;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* GraveyardOutline;

	UPROPERTY(VisibleAnywhere)
	UAutoRotateToPlayer* PlayerRotationActor;
};
