// Fill out your copyright notice in the Description page of Project Settings.


#include "CardPlacement.h"
#include "Net/UnrealNetwork.h"
#include "Card3D.h"
#include "PlacementTiles/PlacementTile_Parent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ACardPlacement::ACardPlacement()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlayerIndex = 0;
	MaxCardsInPlacement = 1;
	bMultiPlacement = false;
	bScaleMulticardPlacement = true;
	bIsFull = false;
	PlacementWidth = 1.0;
	CardSpacing = 330;
	CardWidth = 330;
	bDynamicSpacing = false;
	FillDirection = EDirection::Left;
	bCenterOut = true;
	bInvertPlacementDirection = false;
	bDisplayDemoCards = false;
	DemoCards = 1;
	bSupportsAllCardTypes = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	PlacementCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("PlacementCollision"));
	PlacementCollision->SetupAttachment(RootComponent);

	/*switch (FillDirection)
	{
	case EDirection::Left:
		bCenterOut = false;
		bInvertPlacementDirection = false;
		break;
	case EDirection::CenterLeft:
		bCenterOut = true;
		bInvertPlacementDirection = false;
		break;
	case EDirection::CenterRight:
		bCenterOut = true;
		bInvertPlacementDirection = true;
		break;
	case EDirection::Right:
		bCenterOut = false;
		bInvertPlacementDirection = true;
		break;
	default:
		break;
	}

	PlacementWidth = UKismetMathLibrary::Clamp(UKismetMathLibrary::GridSnap_Float(PlacementWidth, 1), 1, 20);

	bMultiPlacement = MaxCardsInPlacement > 1;
	CreateVisualBoardTiles_C();

	float ReturnValue1;
	float ReturnValue2;

	if (bScaleMulticardPlacement)
	{
		ReturnValue1 = CardWidth* UKismetMathLibrary::Clamp((PlacementWidth - 1), 1, PlacementWidth)/2;
		if (PlacementWidth - 1 > 0)
		{
			ReturnValue2 = 160;
		}
		else
		{
			ReturnValue2 = 0;
		}
	}
	else
	{	
		 ReturnValue1 = 160;
		 ReturnValue2 = 0;
	}
	FVector WorldScale;
	WorldScale.X = 220;
	WorldScale.Y = ReturnValue1 + ReturnValue2;
	WorldScale.Z = 5;
	PlacementCollision->SetWorldLocation(WorldScale);

	if (bDisplayDemoCards)
	{
		for (int32 i = 0; i < DemoCards; i++)
		{
			FVector ReturnVector;
			FRotator ReturnRotator;
			GetCardInPlacementLocation_C(UKismetMathLibrary::FTrunc(float(i) - 0.5), MaxCardsInPlacement, DemoCards, ReturnVector, ReturnRotator);

			FTransform NewTransform;
			NewTransform.SetLocation(ReturnVector);
			NewTransform.SetRotation(FQuat(ReturnRotator));
			NewTransform.SetScale3D(FVector(1,1,1));

			static ConstructorHelpers::FObjectFinder<UStaticMesh> CardModeObj(TEXT("StaticMesh'/Game/CCGToolkit/Meshes/Card_Model.Card_Model'"));
			if (CardModeObj.Object)
			{
				UStaticMeshComponent* CardModel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardModel"));
				CardModel->SetStaticMesh(CardModeObj.Object);

				CardModel->SetHiddenInGame(true, false);
				CardModel->SetWorldTransform(NewTransform, false, false);
				CardModel->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
			
		}
	}
	
	if (bSupportsAllCardTypes)
	{
		bHero = true;
		bCenterOut = true;
		bSpell = true;
		bTrap = true;
		bAura = true;
		bBooster = true;
	}

	CompileSupportedCardTypes_C();*/
}

// Called when the game starts or when spawned
void ACardPlacement::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACardPlacement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACardPlacement::CalculateDynamicSpacingPosition_C_Implementation(int32 InCardInPlacement, int32 InMaxCardsInPlacement, float& ReturnValue, float& ReturnValue2, float& ReturnValue3)
{
	ReturnValue = UKismetMathLibrary::Clamp((float(InMaxCardsInPlacement)* CardSpacing - CardSpacing)/(float(InCardInPlacement) - 1), -5000, CardWidth);
	ReturnValue2 = float(InMaxCardsInPlacement)* CardSpacing/2 - CardSpacing/2;
	ReturnValue3 = float(InMaxCardsInPlacement)* CardSpacing/CardWidth;
}

float ACardPlacement::CalculateSpacingLocation_C_Implementation(int32 Index, int32 InMaxCardsInPlacement, int32 CardInPlacement_P, float Spacing)
{
	return 0;
}

bool ACardPlacement::CalculateDynamicSpacingLocation_C_Implementation(int32 Index, int32 InMaxCardsInPlacement, int32 InCardInPlacement, bool CenterOut, float& InReturnValue)
{
	return false;
}

void ACardPlacement::CreateVisualBoardTiles_C_Implementation()
{

}

FPlacementState ACardPlacement::GetPlacementState_C()
{
	FPlacementState pPState;
	pPState.InPlayerIndex = PlayerIndex;
	pPState.InIsMultiCardPlacement = bMultiPlacement;
	pPState.InIsFull = bIsFull;
	pPState.InCardsInPlacement = Card_Array.Num();
	pPState.InMaxCardsInPlacement = MaxCardsInPlacement;
	pPState.InSupportAllCardTypes = bSupportsAllCardTypes;
	pPState.ValidCardTypesArray = ValidCardTypes_Array;
	return pPState;
}

void ACardPlacement::CompileSupportedCardTypes_C_Implementation()
{

}

void ACardPlacement::RemoveCardFromPlacement_C_Implementation(ACard3D* CardToClear)
{
	Card_Array.Remove(CardToClear);
}

void ACardPlacement::GetCardInPlacementLocation_C_Implementation(int32 CardIndex, int32 MaxCards, int32 CardsInPlacement, FVector& ReturnVector, FRotator& ReturnRotation)
{
	
}

void ACardPlacement::AddCardToPlacement_C_Implementation(ACard3D* CardToAdd)
{
	Card_Array.Add(CardToAdd);
	bIsFull = Card_Array.Num() >= MaxCardsInPlacement;
	FVector ReturnVector;
	FRotator ReturnRotator;
	for (int32 i = 0; i < Card_Array.Num(); i++)
	{
		GetCardInPlacementLocation_C(i, MaxCardsInPlacement, Card_Array.Num(), ReturnVector, ReturnRotator);
		Card_Array[i]->Multicast_PlaceCardOnBoard_C(ReturnVector, ReturnRotator);
	}
}

void ACardPlacement::Event_RemoveCardFromPlacement_C_Implementation(ACard3D* CardToClear)
{
	RemoveCardFromPlacement_C(CardToClear);

	bIsFull = Card_Array.Num() >= MaxCardsInPlacement;
	FVector ReturnVector;
	FRotator ReturnRotator;
	for (int32 i = 0; i < Card_Array.Num(); i++)
	{
		GetCardInPlacementLocation_C(i, MaxCardsInPlacement, Card_Array.Num(), ReturnVector, ReturnRotator);
		Card_Array[i]->Multicast_PlaceCardOnBoard_C(ReturnVector, ReturnRotator);
	}
}

void ACardPlacement::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACardPlacement, PlayerIndex);
	DOREPLIFETIME(ACardPlacement, Card_Array);
	DOREPLIFETIME(ACardPlacement, bMultiPlacement);
	DOREPLIFETIME(ACardPlacement, bIsFull);
	DOREPLIFETIME(ACardPlacement, Graveyard_Ref);
}
