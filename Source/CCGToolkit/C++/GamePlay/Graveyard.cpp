// Fill out your copyright notice in the Description page of Project Settings.


#include "Graveyard.h"
#include "Card3D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"
#include "../FunctionLibraries/Deck_FunctionLibrary.h"
#include "../FunctionLibraries/Controller_FunctionLibrary.h"
#include "../FunctionLibraries/Card_FunctionLibrary.h"

// Sets default values
AGraveyard::AGraveyard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));
	SetRootComponent(DefaultSceneComponent);

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(RootComponent);
	Graveyard_Text = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Graveyard_Text"));
	Graveyard_Text->SetupAttachment(RootComponent);

	SM_GraveyardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_GraveyardMesh"));
	SM_GraveyardMesh->SetupAttachment(RootComponent);

	CardPlacementPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("CardPlacementPoint"));
	CardPlacementPoint->SetupAttachment(SM_GraveyardMesh);

	GraveyardOutline = CreateDefaultSubobject<UChildActorComponent>(TEXT("GraveyardOutline"));
	GraveyardOutline->SetupAttachment(RootComponent);

	PlayerRotationActor = CreateDefaultSubobject<UAutoRotateToPlayer>(TEXT("PlayerRotationActor"));
}

// Called when the game starts or when spawned
void AGraveyard::BeginPlay()
{
	Super::BeginPlay();
	
	SM_GraveyardMesh->SetHiddenInGame(true, false);
}

// Called every frame
void AGraveyard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGraveyard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGraveyard, ActorsInGraveyard);
	DOREPLIFETIME(AGraveyard, GraveyardList);
}

void AGraveyard::ScaleCardGraveyardStaticMesh_C()
{
	if (GraveyardList.Num() > 0)
	{
		SM_GraveyardMesh->SetHiddenInGame(false, false);
	}
	else
	{
		SM_GraveyardMesh->SetHiddenInGame(true, false);
	}
	FVector ReturnValue(0,0,0);
	ReturnValue.Z = float(GraveyardList.Num() * 1.5 - 66);
	SM_GraveyardMesh->SetRelativeLocation(ReturnValue);
}

void AGraveyard::HideUnseenCardsInGraveyard_C()
{
	if (ActorsInGraveyard.Num() > 1)
	{
		for (int32 i = 0; i < ActorsInGraveyard.Num(); i++)
		{
			if (ActorsInGraveyard[i])
			{
				if (i != ActorsInGraveyard.Num()-1)
				{
					ActorsInGraveyard[i]->RemoveCardActor_C();
					ActorsInGraveyard.RemoveAt(i);
				}
			}
		}
	}
}

FVector AGraveyard::GetCardInGraveyardLocation_C(int32 add)
{
	FVector ReturnValue;
	ReturnValue.X = GetActorLocation().X;
	ReturnValue.Y = GetActorLocation().Y;
	ReturnValue.Z = GetActorLocation().Z + float(GraveyardList.Num()+add)* 1.5 +10;
	return ReturnValue;
}

void AGraveyard::RecreateTopGraveyardCardActor_C()
{
	if (HasAuthority())
	{
		if (GraveyardList.Num() > 0 && ActorsInGraveyard.Num() == 0)
		{
			AController* GoalController = UController_FunctionLibrary::GetControllerReferenceFromID_ServerOnly(Player_Index, GetWorld());
			IController_Interface* Controller_Interface = Cast<IController_Interface>(GoalController);
			FTransform SpawnTransform;
			SpawnTransform = FTransform(GetCardInGraveyardLocation_C(1));
			ACard3D* CardCreated = Controller_Interface->Execute_CreatePlayableCard_C(GoalController, SpawnTransform);
			ACard3D* Card_Ref = UCard_FunctionLibrary::SetupCard(CardCreated, Player_Index, GraveyardList[GraveyardList.Num()-1], ECardSet::Empty, GraveyardStructList[GraveyardList.Num()-1], true);
			ActorsInGraveyard.AddUnique(Card_Ref);
			CardCreated->ForceMoveCardDirectlyToGraveyard_C(this);
		}
	}
}

void AGraveyard::RemoveCardFromGraveyard_C(FName CardName, int32 Index)
{
	RemoveIndex = Index;
	if (GraveyardList.Num() > 0 && GraveyardList[RemoveIndex] == CardName)
	{
		GraveyardStructList.RemoveAt(RemoveIndex);
		if (ActorsInGraveyard[0])
		{
			ActorsInGraveyard[0]->SetCardHiddenState_C(true);
			ActorsInGraveyard[0]->RemoveCardActor_C();
			ActorsInGraveyard.RemoveAt(0);
			GraveyardList.RemoveAt(RemoveIndex);
;		}
	}
}

void AGraveyard::Event_RemoveCardFromGraveyard_C_Implementation(FName Card, int32 Index)
{
	if (HasAuthority())
	{
		RemoveCardFromGraveyard_C(Card, Index);
		RecreateTopGraveyardCardActor_C();
	}
}

void AGraveyard::AddToGraveyardVisual_C_Implementation()
{
	ScaleCardGraveyardStaticMesh_C();
}

void AGraveyard::AddToGraveyard_C_Implementation(ACard3D* EnteringCard, FName OrByName)
{
	if (HasAuthority())
	{
		if (EnteringCard)
		{
			ActorsInGraveyard.AddUnique(EnteringCard);
			GraveyardList.Add(EnteringCard->CardDataTableName);
			GraveyardStructList.Add(EnteringCard->CardDataStruct);
			HideUnseenCardsInGraveyard_C();
		}
		else
		{
			FSCard CardStruct;
			UDeck_FunctionLibrary::GetCardData(OrByName, ECardSet::Empty, CardStruct);
			GraveyardList.Add(OrByName);
			GraveyardStructList.Add(CardStruct);
			HideUnseenCardsInGraveyard_C();
		}
	}
}

