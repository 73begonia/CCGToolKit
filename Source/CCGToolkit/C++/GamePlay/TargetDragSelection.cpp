// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetDragSelection.h"

// Sets default values
ATargetDragSelection::ATargetDragSelection()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));
	SetRootComponent(DefaultSceneComponent);

	NarrowCapsule = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NarrowCapsule"));
	NarrowCapsule->SetupAttachment(RootComponent);

	ToLoacation = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ToLoacation"));
	ToLoacation->SetupAttachment(RootComponent);

	Target = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Target"));
	Target->SetupAttachment(ToLoacation);

}

// Called when the game starts or when spawned
void ATargetDragSelection::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATargetDragSelection::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATargetDragSelection::SetSelectionProperties_Implementation(FVector Location, bool ValidTarget)
{
	
}

