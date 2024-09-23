// Fill out your copyright notice in the Description page of Project Settings.


#include "PlacementTile_Parent.h"

// Sets default values
APlacementTile_Parent::APlacementTile_Parent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(RootComponent);

	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	Decal->SetupAttachment(RootComponent);

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(RootComponent);

	TileOutline = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileOutLine"));
	TileOutline->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APlacementTile_Parent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlacementTile_Parent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

