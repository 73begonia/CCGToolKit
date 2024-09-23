// Fill out your copyright notice in the Description page of Project Settings.


#include "Card3D.h"
#include "CardPlacement.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "../Widgets/GamePlay/CardPreviewWidget.h"
#include "Components/BoxComponent.h"
#include "CardPlacement.h"
#include "Graveyard.h"
#include "../CardGameState.h"
#include "../CardGamePlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Components/PrimitiveComponent.h"
#include "../FunctionLibraries/Effects_FunctionLibrary.h"
#include "../FunctionLibraries/Card_FunctionLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "../FunctionLibraries/Misc_FunctionLibrary.h"
#include "GameFramework/Actor.h"
#include "../FunctionLibraries/Deck_FunctionLibrary.h"
#include "../FunctionLibraries/Math_Library.h"
#include "Kismet/KismetStringLibrary.h"
#include "../FunctionLibraries/CardAbility_FunctionLibrary.h"
#include "../../CardGame/WHOO.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

// Sets default values
ACard3D::ACard3D()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	OuterMaterial = ConstructorHelpers::FObjectFinderOptional<UMaterial>(TEXT("Material'/Game/CCGToolkit/CardGame/CardSets/Basic_Set/Materials/3DCard_Nothing.3DCard_Nothing'")).Get();
	Shadow_Mat_Ref = ConstructorHelpers::FObjectFinderOptional<UMaterial>(TEXT("Material'/Game/CCGToolkit/CardGame/CardSets/Basic_Set/Effects/CardShadow_Mat.CardShadow_Mat'")).Get();
	
	Card3DNothing = ConstructorHelpers::FObjectFinderOptional<UMaterial>(TEXT("Material'/Game/CCGToolkit/CardGame/CardSets/Basic_Set/Materials/3DCard_Nothing.3DCard_Nothing'")).Get();
	Card_Shadow_Glow_Mat = ConstructorHelpers::FObjectFinderOptional<UMaterial>(TEXT("Material'/Game/CCGToolkit/CardGame/CardSets/Basic_Set/Effects/Card_Shadow_Glow_Mat.Card_Shadow_Glow_Mat'")).Get();

	TurnPoints = 1;
	Type = ECardType::Creature;
	CardSet = ECardSet::Empty;
	CardDeltaTime = 0;
	InterpSpeed = 15;
	ErrorTolerance = 0.001;
	CardPreviewScale = 0.7;
	MaxDescriptionCharacterLineLength = 43;
	Platform = EPlatform::Windows;
	CardMovementState = E3DCardMovementState::Placing;
	CardGameState = ETurnState::TurnActive;
	bIsAttacking = true;
	IsAbilityActive= true;

	DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));
	SetRootComponent(DefaultSceneComponent);

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(RootComponent);

	CardBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardBase"));
	CardBase->SetupAttachment(RootComponent);

	Description_Text = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Description_Text"));
	Description_Text->SetupAttachment(RootComponent);
	CardType_Text = CreateDefaultSubobject<UTextRenderComponent>(TEXT("CardType_Text"));
	CardType_Text->SetupAttachment(RootComponent);
	Name_Text = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Name_Text"));
	Name_Text->SetupAttachment(RootComponent);

	ManaGem_Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ManaGem_Plane"));
	ManaGem_Plane->SetupAttachment(CardBase);
	Heart_Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Heart_Plane"));
	Heart_Plane->SetupAttachment(CardBase);
	Attack_Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Attack_Plane"));
	Attack_Plane->SetupAttachment(CardBase);

	ManaCost_Text = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ManaCost_Text"));
	ManaCost_Text->SetupAttachment(ManaGem_Plane);
	Health_Text= CreateDefaultSubobject<UTextRenderComponent>(TEXT("Health_Text"));
	Health_Text->SetupAttachment(Heart_Plane);
	Attack_Text = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Attack_text"));
	Attack_Text->SetupAttachment(Attack_Plane);
}

// Called when the game starts or when spawned
void ACard3D::BeginPlay()
{
	Super::BeginPlay();

	UCardGameInstance* CardGameInstance = Cast<UCardGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	bIsPlatformMobile = CardGameInstance->GetCurrentPlatform_C(Platform);
	CardBase->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GameState_Ref = Cast<ACardGameState>(UGameplayStatics::GetGameState(GetWorld()));
}

// Called every frame
void ACard3D::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACard3D::ReceiveActorBeginCursorOver()
{
	ACardGamePlayerController* CardGamePlayerController = Cast<ACardGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	switch (Platform)
	{
	case EPlatform::Windows:
	case EPlatform::HTML5:
	case EPlatform::Mac:
	case EPlatform::Linux:
		if(CardGamePlayerController->PlayerStateEnum == EPlayerStateEnum::PendingAction)
		{
			if (bIsPlaced)
			{
				PreviewEnabled = true;
				if (!bIsSelected && !bInGraveyard)
				{
					GetWorldTimerManager().SetTimer(SwitchPreviewHandle, this, &ACard3D::SwitchPreviewMode, 0.5, false);
				}
			}
		}
		break;
	case EPlatform::iOS:
		break;
	case EPlatform::Android:
		break;
	default:
		break;
	}
}

void ACard3D::ReceiveActorEndCursorOver()
{
	ACardGamePlayerController* CardGamePlayerController = Cast<ACardGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	switch (Platform)
	{
	case EPlatform::Windows:
	case EPlatform::HTML5:
	case EPlatform::Mac:
	case EPlatform::Linux:
		if (CardGamePlayerController->PlayerStateEnum == EPlayerStateEnum::PendingAction)
		{
			if (bIsPlaced)
			{
				RemoveMouseOverPreview();
			}
		}
		break;
	case EPlatform::iOS:
		break;
	case EPlatform::Android:
		break;
	default:
		break;
	}
}

void ACard3D::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACard3D, CardDataTableName);
	DOREPLIFETIME(ACard3D, Name);
	DOREPLIFETIME(ACard3D, Attack);
	DOREPLIFETIME(ACard3D, Health);
	DOREPLIFETIME(ACard3D, Mana_Cost_Placement);
	DOREPLIFETIME(ACard3D, TurnPoints);
	DOREPLIFETIME(ACard3D, LifeExpectancy);
	DOREPLIFETIME(ACard3D, TurnsAlive);
	DOREPLIFETIME(ACard3D, Type);
	DOREPLIFETIME(ACard3D, CardSet);
	DOREPLIFETIME(ACard3D, Description);
	DOREPLIFETIME(ACard3D, CanAttackPlayer);
	DOREPLIFETIME(ACard3D, CanAttackCards);
	DOREPLIFETIME(ACard3D, OwningPlayerID);
	DOREPLIFETIME(ACard3D, bIsPlaced);
	DOREPLIFETIME(ACard3D, bInGraveyard);
	DOREPLIFETIME(ACard3D, bEnableLifeExpectancy);
	DOREPLIFETIME(ACard3D, bCardActive);
	DOREPLIFETIME(ACard3D, HomeDestination);
	DOREPLIFETIME(ACard3D, AbilityTriggers);
	DOREPLIFETIME(ACard3D, AbilityTypes);
	DOREPLIFETIME(ACard3D, CardDataStruct);
}

void ACard3D::CloseGate_C_Implementation()
{
	
}

void ACard3D::OpenGate_C_Implementation()
{
	
}

void ACard3D::SwitchPreviewMode()
{
	if (PreviewEnabled)
	{
		if (!ViewCardPreview)
		{
			UClass* CardPreviewWidgetClass = LoadClass<UCardPreviewWidget>(NULL, TEXT("WidgetBlueprint'/Game/CCGToolkitC++/Blueprints/Widgets/Gameplay/BP_CardPreviewWidget.BP_CardPreviewWidget'"));
			ViewCardPreview = CreateWidget<UCardPreviewWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), CardPreviewWidgetClass);
		}
		float ReturnValue;
		if (bIsPlatformMobile)
		{
			ReturnValue = CardPreviewScale * 1.5;
		}
		else
		{
			ReturnValue = CardPreviewScale;
		}
		ViewCardPreview->InitiateCardPreview(CardDataTableName, CardSet, this, ReturnValue);
		ViewCardPreview->AddToViewport(5);
	}
	else
	{
		RemoveMouseOverPreview();
	}
}

bool ACard3D::RunCardAbility(int32 AbilityIndex)
{
	int32 AbilityIndex_Ref = 0;
	if (HasAuthority())
	{
		AbilityIndex_Ref = AbilityIndex;
		switch (AbilityTypes[AbilityIndex])
		{
		case EAbilityType::None:
			UMisc_FunctionLibrary::CreateScreenDisplayMessage("This Card has no ability", FColor::Red, 2.0);
			break;
		case EAbilityType::DrawCards:
			UCardAbility_FunctionLibrary::DrawCards(this, AbilityIndex_Ref);
			break;
		case EAbilityType::IncreaseAttack:
			UCardAbility_FunctionLibrary::IncreaseAttack(this, AbilityIndex_Ref);
			break;
		case EAbilityType::Clone:
			UCardAbility_FunctionLibrary::CloneCard(this, AbilityIndex_Ref, false);
			break;
		case EAbilityType::IncreasePlayerHealth:
			UCardAbility_FunctionLibrary::IncreasePlayerHealth(this, AbilityIndex_Ref);
			break;
		case EAbilityType::RetaliationDamage:
			UCardAbility_FunctionLibrary::DealRetaliationDamage(this, AbilityIndex_Ref);
			break;
		case EAbilityType::DamageAllCardsOnBoard:
			UCardAbility_FunctionLibrary::DamageAllCardsOnBoard(this, AbilityIndex_Ref);
			break;
		case EAbilityType::SpawnRandomCardFromDeck:
			UCardAbility_FunctionLibrary::SpawnRandomCardFromDeck(this, AbilityIndex_Ref);
			break;
		case EAbilityType::GiveTurnPointsToAllActiveCards:
			UCardAbility_FunctionLibrary::GiveTurnPointstoAllActiveCards(this, AbilityIndex_Ref);
			break;
		case EAbilityType::IncreaseTurnPoints:
			UCardAbility_FunctionLibrary::IncreaseTurnPoints(this, AbilityIndex_Ref);
			break;
		case EAbilityType::DiscardCardsInHand:
			UCardAbility_FunctionLibrary::DiscardRandomCardsFromHand(this, AbilityIndex_Ref);
			break;
		case EAbilityType::PickupFirstCreatureInTheGraveyardToBoard:
			UCardAbility_FunctionLibrary::PickupCardsFromGraveyard(this, AbilityIndex_Ref, true, ECardType::Creature);
			break;
		case EAbilityType::PickupFirstCreatureInTheGraveyardToHand:
			UCardAbility_FunctionLibrary::PickupCardsFromGraveyard(this, AbilityIndex_Ref, false, ECardType::None);
			break;
		case EAbilityType::PossessOpponentsCard:
			UCardAbility_FunctionLibrary::ChangePlayedCardOwner(this, AbilityIndex_Ref);
			break;
		case EAbilityType::SwapCardsInHand:
			UCardAbility_FunctionLibrary::ChangeInHandCardOwner(this, AbilityIndex_Ref);
			break;
		default:
			break;
		}
	}
	return true;
}

void ACard3D::SetCardData(FName CardTableName, ECardSet InCardSet, FSCard CardStruct, bool SetCardDataFromStruct)
{
	if (SetCardDataFromStruct)
	{
		CardDataStruct = CardStruct;
	}
	else
	{
		FSCard TempCardStruct;
		UDeck_FunctionLibrary::GetCardData(CardTableName, InCardSet, TempCardStruct);
		CardDataStruct = TempCardStruct;
	}
	CardDataTableName = CardTableName;
	CardSet = InCardSet;
	Name = CardDataStruct.Name;
	Attack = CardDataStruct.Attack.Damage;
	OnRep_Attack_C();
	CanAttackPlayer = CardDataStruct.Attack.CanAttackPlayer;
	CanAttackCards = CardDataStruct.Attack.CanAttackCards;
	Health = CardDataStruct.Health.Health;
	OnRep_Health_C();
	LifeExpectancy = CardDataStruct.Health.Life_Expectancy;
	bEnableLifeExpectancy = LifeExpectancy > 0;
	Mana_Cost_Placement = CardDataStruct.Placement_Settings.Mana_Cost;
	OnRep_Mana_Cost_Placement_C();
	Type = CardDataStruct.Type;
	OnRep_Type_C();
	for (int32 i = 0; i < CardDataStruct.Abilities.Num(); i++)
	{
		Ability_Struct_Refs.Add(CardDataStruct.Abilities[i]);
		AbilityTypes.Add(CardDataStruct.Abilities[i].Type);
		AbilityTriggers.Add(CardDataStruct.Abilities[i].Trigger);
	}
	Description = CardDataStruct.Description.Description;
}

bool ACard3D::InterpSelfToLocation(FVector& ReturnLocation)
{
	if (FMath::IsNearlyEqual((this->GetActorLocation()-GoalDestination).Size(), 0, ErrorTolerance))
	{
		ReturnLocation = GoalDestination;
		return true;
	}
	else
	{
		ReturnLocation = FMath::VInterpTo(this->GetActorLocation(), GoalDestination, CardDeltaTime, InterpSpeed);
		return false;
	}
}

bool ACard3D::InterpSelfToRotation(FRotator& ReturnRotation)
{
	if (UKismetMathLibrary::EqualEqual_RotatorRotator(this->GetActorRotation(), GoalRotation, ErrorTolerance/5))
	{
		ReturnRotation = GoalRotation;
		return true;
	}
	else
	{
		ReturnRotation = FMath::RInterpTo(this->GetActorRotation(), GoalRotation, CardDeltaTime, InterpSpeed);
		return false;
	}
}

void ACard3D::SetCardVisuals()
{
	if (!UKismetSystemLibrary::IsDedicatedServer(this))
	{
		CardFrameMaterial = UMaterialInstanceDynamic::Create(CardDataStruct.Visual.Frame_Material, CardBase);
		CardBase->SetMaterial(3, CardFrameMaterial);
		CardImageMaterial = UMaterialInstanceDynamic::Create(CardDataStruct.Visual.Image_Material, CardBase);
		CardBase->SetMaterial(1, CardImageMaterial);
		CardBackMaterial = UMaterialInstanceDynamic::Create(CardDataStruct.Visual.Back_material, CardBase);
		CardBase->SetMaterial(2, CardBackMaterial);

		Name_Text->SetText(Name);
		CardType_Text->SetText(FText::FromString(UMath_Library::GetEnumValueAsString("ECardType", Type)));
		GenerateDescription(CardDataStruct.Description.Description);
		Multicast_UpdateVisualStats_C();
		OuterMaterial = Shadow_Mat_Ref;
	}
}

void ACard3D::GenerateDescription_Implementation(const FString& DescriptionText)
{
	
}

void ACard3D::Multicast_MoveToGraveyard_C_Implementation()
{
	if (!UKismetSystemLibrary::IsDedicatedServer(this))
	{
		Event_Deselected_C();
		SetTurnStateVisual(true);
		RemoveMouseOverPreview();
		CardBase->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CardBase->SetMaterial(0, Card3DNothing);
		PlayMovementAnimation(E3DCardMovementState::Graveyard, GameState_Ref->GetGraveyardReference_C(OwningPlayerID)->GetCardInGraveyardLocation_C(0), GameState_Ref->GetGraveyardReference_C(OwningPlayerID)->GetActorRotation(), nullptr, 50, 2.5, 12);
	}
}

void ACard3D::ArrivedAtDestination_C_Implementation()
{
	switch (CardMovementState)
	{
	case E3DCardMovementState::Placing:
		break;
	case E3DCardMovementState::MoveToHomeDestination:
		if(!CheckAblilityUseState(EAbilityTrigger::OnDrop, ECardUseState::RemainInPlay, 0, true))		
		{
			CardBase->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		CloseGate_C();
		//SpawnEffect_C(ECardEffects::OnCardPlaced);
		if (AbilityTypes.Num() > 0)
		{
			RunActiveAbility(EAbilityTrigger::OnDrop);
		}
		break;
	case E3DCardMovementState::MovingToDestination:
		break;
	case E3DCardMovementState::PlacedOnBoard:
		break;
	case E3DCardMovementState::Attacking:
		IsAbilityActive = false;
		Multicast_SpawnEffect_C(ECardEffects::OnAttack);
		if (TurnPoints > 0)
		{
			PlayMovementAnimation(E3DCardMovementState::MoveToHomeDestination, HomeDestination, HomeRotation, nullptr, 0, 5, 12);
		}
		else
		{
			bCardActive = false;
			OnRep_bCardActive_C();
			PlayMovementAnimation(E3DCardMovementState::MoveToHomeDestination, HomeDestination, HomeRotation, nullptr, 0, 5, 12);
		}
		break;
	case E3DCardMovementState::Selected:
		break;
	case E3DCardMovementState::Graveyard:
		CloseGate_C();
		if (PlacementOwner)
		{
			PlacementOwner->Graveyard_Ref->AddToGraveyardVisual_C();
		}
		break;
	default:
		break;
	}
}

void ACard3D::Server_SetCardData_C_Implementation(FName CardTableName, ECardSet CardRace, FSCard CardStruct, bool SetCardDataFromStruct)
{
	if (HasAuthority())
	{
		SetCardData(CardTableName, CardRace, CardStruct, SetCardDataFromStruct);
		Mutlicast_SetCardVisuals_C();
	}
}

void ACard3D::Multicast_SpawnEffect_C_Implementation(ECardEffects CardEffect)
{
	UParticleSystem* Particle;
	USoundCue* SoundCue;
	if (!UKismetSystemLibrary::IsDedicatedServer(this))
	{
		UEffects_FunctionLibrary::GetCardEffectsForState(CardEffect, CardDataStruct, 0, Particle, SoundCue);
		UEffects_FunctionLibrary::SpawnParticleAndSoundEffects(Particle, FVector(0, 0, 0), this, FVector(1, 1, 1), true, SoundCue, 1);
	}
}

void ACard3D::Multicast_DamageVisual_C_Implementation()
{
	StartCoroutine("DamageColourChange2", DamageColourChange2());
}

void ACard3D::Mutlicast_SetCardVisuals_C_Implementation()
{
	SetCardVisuals();
}

void ACard3D::Server_ActivateAbility_C_Implementation(int32 AbilityIndex)
{
	if (HasAuthority())
	{
		Multicast_AbilityVisual_C();
		//Multicast_SpawnEffect_C_Implementation(ECardEffects::OnAbilityUsed);
		if (RunCardAbility(AbilityIndex))
		{
			if (CheckAblilityUseState(EAbilityTrigger::None, ECardUseState::SingleUseSendToGraveyardAfterAbility, AbilityIndex, false))
			{
				StartCoroutine("CoreMoveCardtoGravyard", CoreMoveCardtoGravyard());
			}
		}
	}
}

void ACard3D::Multicast_Attacking_C_Implementation(AActor* Attacking)
{
	if (HasAuthority())
	{
		if (!UKismetSystemLibrary::IsDedicatedServer(this))
		{
			bIsAttacking = true;
			CardBase->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			PlayMovementAnimation(E3DCardMovementState::Attacking, FVector(0, 0, 0), FRotator(0, 0, 0), Attacking, 50, 50, 12);
		}
	}
	else
	{
		bIsAttacking = true;
		CardBase->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		PlayMovementAnimation(E3DCardMovementState::Attacking, FVector(0,0,0), FRotator(0,0,0), Attacking, 50, 50, 12);
	}
}

void ACard3D::Multicast_UpdateVisualStats_C_Implementation()
{
	if (this)
	{
		Attack_Text->SetText(FText::FromString(FString::FromInt(Attack)));
		Health_Text->SetText(FText::FromString(FString::FromInt(Health)));
		ManaCost_Text->SetText(FText::FromString(FString::FromInt(Mana_Cost_Placement)));
	}
}

void ACard3D::Multicast_PlaceCardOnBoard_C_Implementation(FVector Destination, FRotator Rotation)
{
	HomeDestination = Destination;
	FRotator WorldRotationForPlayer;
	UMisc_FunctionLibrary::GetWorldRotationForPlayer(FRotator(0, 0, 0), WorldRotationForPlayer);
	if (!bIsPlaced)
	{
		bIsPlaced = true;
		SetActorRotation(FQuat(WorldRotationForPlayer));
		CardBase->SetMaterial(0, OuterMaterial);
	}
	HomeRotation = UKismetMathLibrary::ComposeRotators(WorldRotationForPlayer, Rotation);
	PlayMovementAnimation(E3DCardMovementState::MoveToHomeDestination, HomeDestination, HomeRotation, nullptr, 0, 5, 12);
}

void ACard3D::Multicast_UpdateCardVisual_C_Implementation(bool IsActive)
{
	SetTurnStateVisual(IsActive);
}

void ACard3D::SetClientSideData_C(FName CardTableName, ECardSet CardRace)
{
	FSCard CardStruct;
	SetCardData(CardTableName, CardRace, CardStruct, false);
	SetCardVisuals();
}

void ACard3D::SpawnEffect_C(ECardEffects CardEffect)
{
	UParticleSystem* Particle;
	USoundCue* SoundCue;
	if (!UKismetSystemLibrary::IsDedicatedServer(this))
	{
		UEffects_FunctionLibrary::GetCardEffectsForState(CardEffect, CardDataStruct, 0, Particle, SoundCue);
		UEffects_FunctionLibrary::SpawnParticleAndSoundEffects(Particle, FVector(0, 0, 0), this, FVector(1, 1, 1), true, SoundCue, 1);
	}
}

bool ACard3D::TakesDamage_C_Implementation(ACard3D* DealingCard, int32 DamageAttack, int32 DamageHealth)
{
	if (HasAuthority())
	{
		if (DealingCard)
		{
			DamageDealingCard_Ref = DealingCard;
		}
		else
		{
			DamageDealingCard_Ref = nullptr;
		}
		if (DamageAttack > 0)
		{
			Attack = FMath::Clamp(Attack - DamageAttack, 0, 100);
			OnRep_Attack_C();
		}
		if (DamageHealth > 0)
		{
			Health = FMath::Clamp(Health - DamageHealth, 0, 100);
			OnRep_Health_C();
		}
		Multicast_DamageVisual_C();
		Multicast_SpawnEffect_C(ECardEffects::DecreasedHealth);
		if (AbilityTypes.Num() > 0)
		{
			RunActiveAbility(EAbilityTrigger::TakesDamage);
			if (Health <= 0)
			{
				bInGraveyard = true;
				FTimerHandle CoreMoveCardtoGravyard;
				FTimerHandle Multicast_UpdateVisualStats;
				GetWorldTimerManager().SetTimer(CoreMoveCardtoGravyard, this, &ACard3D::MoveCardToGravyard, 1);
				//StartCoroutine("CoreMoveCardtoGravyard", CoreMoveCardtoGravyard());
			}
			Multicast_UpdateVisualStats_C();
		}
	}
	return false;
}

bool ACard3D::OnActivePlayerTurn_C_Implementation()
{
	if (HasAuthority())
	{
		TurnPoints = 1;
		bCardActive = true;
		OnRep_bCardActive_C();
		if (AbilityTypes.Num() > 0)
		{
			RunActiveAbility(EAbilityTrigger::StartOfTurn);
		}
	}
	return false;
}

bool ACard3D::OnEndActivePlayerTurn_C_Implementation()
{
	bCardActive = false;
	OnRep_bCardActive_C();
	TurnsAlive++;
	if (HasAuthority())
	{
		if (bEnableLifeExpectancy && !bInGraveyard && TurnsAlive >= LifeExpectancy)
		{
			RunActiveAbility(EAbilityTrigger::DeathByLifeExpectancy);
			MoveCardToGravyard();
			Multicast_UpdateCardVisual_C(true);
		}
		else
		{
			RunActiveAbility(EAbilityTrigger::EndOfTurn);
			Multicast_UpdateCardVisual_C(true);
		}
	}
	return false;
}

void ACard3D::Event_Deselected_C()
{
	if (bIsSelected)
	{
		bIsSelected = false;
		this->SetActorScale3D(FVector(1,1,1));
		UMaterialInstanceDynamic* OuterMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(OuterMaterial, CardBase);
		CardBase->SetMaterial(0, OuterMaterialInstanceDynamic);
	}
}

void ACard3D::Event_Selected_C(int32 SelectingPlayerID)
{	
	if (!bIsSelected)
	{
		bIsSelected = true;
		this->SetActorScale3D(FVector(1.15,1.15,1.15));
		if (SelectingPlayerID == OwningPlayerID)
		{
			CardGreenGlow_Mat = UMaterialInstanceDynamic::Create(Card_Shadow_Glow_Mat, CardBase);
			CardGreenGlow_Mat->SetVectorParameterValue("Colour", FLinearColor(0.0065, 0.835, 0));
			CardBase->SetMaterial(0, CardGreenGlow_Mat);
		}
		else
		{
			CardRedGlow_Mat = UMaterialInstanceDynamic::Create(Card_Shadow_Glow_Mat, CardBase);
			CardRedGlow_Mat->SetVectorParameterValue("Colour", FLinearColor(0.725, 0.006, 0));
			CardBase->SetMaterial(0, CardRedGlow_Mat);
		}
		RemoveMouseOverPreview();
	}
}	

void ACard3D::DisableMobileCardPreview_C()
{
	if (PreviewEnabled)
	{
		switch (Platform)
		{
		case EPlatform::Windows:
			break;
		case EPlatform::iOS:
			if (bIsPlaced)
			{
				RemoveMouseOverPreview();
			}
			break;
		case EPlatform::Android:
			if (bIsPlaced)
			{
				RemoveMouseOverPreview();
			}
			break;
		case EPlatform::HTML5:
			break;
		case EPlatform::Mac:
			break;
		case EPlatform::Linux:
			break;
		default:
			break;
		}
	}
}

void ACard3D::EnableMobileCardPreview_C()
{
	if (!PreviewEnabled)
	{
		switch (Platform)
		{
		case EPlatform::Windows:
			break;
		case EPlatform::iOS:
			if (bIsPlaced)
			{
				PreviewEnabled = true;
				if (!ViewCardPreview)
				{
					UClass* CardPreviewWidgetClass = LoadClass<UCardPreviewWidget>(NULL, TEXT("WidgetBlueprint'/Game/CCGToolkitC++/Blueprints/Widgets/Gameplay/BP_CardPreviewWidget.BP_CardPreviewWidget'"));
					ViewCardPreview = CreateWidget<UCardPreviewWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), CardPreviewWidgetClass);
				}
				float ReturnValue;
				if (bIsPlatformMobile)
				{
					ReturnValue = CardPreviewScale * 1.5;
				}
				else
				{
					ReturnValue = CardPreviewScale;
				}
				ViewCardPreview->InitiateCardPreview(CardDataTableName, CardSet, this, ReturnValue);
				ViewCardPreview->AddToViewport(5);
			}
			break;
		case EPlatform::Android:
			if (bIsPlaced)
			{
				PreviewEnabled = true;
				if (!ViewCardPreview)
				{
					UClass* CardPreviewWidgetClass = LoadClass<UCardPreviewWidget>(NULL, TEXT("WidgetBlueprint'/Game/CCGToolkitC++/Blueprints/Widgets/Gameplay/BP_CardPreviewWidget.BP_CardPreviewWidget'"));
					ViewCardPreview = CreateWidget<UCardPreviewWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), CardPreviewWidgetClass);
				}
				float ReturnValue;
				if (bIsPlatformMobile)
				{
					ReturnValue = CardPreviewScale * 1.5;
				}
				else
				{
					ReturnValue = CardPreviewScale;
				}
				ViewCardPreview->InitiateCardPreview(CardDataTableName, CardSet, this, ReturnValue);
				ViewCardPreview->AddToViewport(5);
			}
			break;
		case EPlatform::HTML5:
			break;
		case EPlatform::Mac:
			break;
		case EPlatform::Linux:
			break;
		default:
			break;
		}
	}
}

void ACard3D::SetTurnStateVisual(bool InIsActive)
{
	if (InIsActive)
	{
		ChangeTurnStateVisual(ETurnState::TurnActive);
	}
	else
	{
		ChangeTurnStateVisual(ETurnState::TurnInactive);
	}
}

void ACard3D::ChangeTurnStateVisual(ETurnState InCardGameState)
{
	if (!CardFrameMaterial_Instance)
	{
		CardFrameMaterial_Instance = UMaterialInstanceDynamic::Create(CardDataStruct.Visual.Frame_Material, CardBase);
		CardBase->SetMaterial(3, CardFrameMaterial_Instance);
	}
	if (!CardImageMaterial_Instance)
	{
		CardImageMaterial_Instance = UMaterialInstanceDynamic::Create(CardDataStruct.Visual.Image_Material, CardBase);
		CardBase->SetMaterial(1, CardImageMaterial_Instance);
	}
	if (InCardGameState != ETurnState::UpdateCurrentState)
	{
		CardGameState = InCardGameState;
	}

	switch (CardGameState)
	{
	case ETurnState::TurnActive:	
		CardFrameMaterial_Instance->SetVectorParameterValue("Tint", FLinearColor::White);
		CardImageMaterial_Instance->SetVectorParameterValue("Tint", FLinearColor::White);
		CardFrameMaterial_Instance->SetScalarParameterValue("Brightness", 1.3);
		CardImageMaterial_Instance->SetScalarParameterValue("Brightness", 1.3);
		break;
	case ETurnState::TurnInactive:
		CardFrameMaterial_Instance->SetVectorParameterValue("Tint", FLinearColor(0.13,0.13,0.13));
		CardImageMaterial_Instance->SetVectorParameterValue("Tint", FLinearColor(0.13, 0.13, 0.13));
		CardFrameMaterial_Instance->SetScalarParameterValue("Brightness", 1.3);
		CardImageMaterial_Instance->SetScalarParameterValue("Brightness", 1.3);
		break;
	case ETurnState::UpdateCurrentState:
		break;
	default:
		break;
	}
}

bool ACard3D::CheckAblilityUseState(EAbilityTrigger CheckAbilityTrigger, ECardUseState CheckCardUseState, int32 AbilityIndex, bool SearchAllAbilities)
{
	bool TempBoolenan = false;

	if (!bInGraveyard)
	{
		TempBoolenan = false;
		if (SearchAllAbilities)
		{
			for (int32 i = 0; i < CardDataStruct.Abilities.Num(); i++)
			{
				if (CardDataStruct.Abilities[i].Trigger == CheckAbilityTrigger && CardDataStruct.Abilities[i].After_Use_State != ECardUseState::RemainInPlay)
				{
					TempBoolenan = true;
				}
			}
			return TempBoolenan;
		}
		else
		{
			if (CardDataStruct.Abilities[AbilityIndex].After_Use_State == CheckCardUseState && CardDataStruct.Abilities[AbilityIndex].After_Use_State != ECardUseState::RemainInPlay)
			{
				TempBoolenan = true;
				return TempBoolenan;
			}
			else
			{
				return TempBoolenan;
			}
		}
	}
	else
	{
		TempBoolenan = false;
		return TempBoolenan;
	}
}

bool ACard3D::RunActiveAbility(EAbilityTrigger InTrigger)
{
	if (HasAuthority())
	{
		if (AbilityTriggers.Contains(InTrigger))
		{
			if (IsAbilityActive)
			{
				for (int32 i = 0; i < AbilityTriggers.Num(); i++)
				{
					if (AbilityTriggers[i] == InTrigger)
					{
						if (CheckAblilityUseState(EAbilityTrigger::None, ECardUseState::SingleUseSendToGraveyardBeforeAbility, i, false))
						{
							MoveCardToGravyard();							
						}
						Server_ActivateAbility_C(i);
					}
				}
				return true;
			}
			else
			{
				if (Type == ECardType::Spell || Type == ECardType::Booster)
				{
					for (int32 i = 0; i < AbilityTriggers.Num(); i++)
					{
						if (AbilityTriggers[i] == InTrigger)
						{
							if (CheckAblilityUseState(EAbilityTrigger::None, ECardUseState::SingleUseSendToGraveyardBeforeAbility, i, false))
							{
								MoveCardToGravyard();
							}
							Server_ActivateAbility_C(i);
						}
					}
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}
	return false;
}

void ACard3D::MoveCardToGravyard()
{
	if (HasAuthority())
	{
		bIsPlaced = false;
		TurnPoints = 0;
		if (!bInGraveyard)
		{
			bInGraveyard = false;
		}
		CardMovementState = E3DCardMovementState::Graveyard;
		if (AbilityTypes.Num() > 0)
		{
			RunActiveAbility(EAbilityTrigger::SentToGraveyard);
		}
		IsAbilityActive = false;
		AGraveyard* Graveyard = GameState_Ref->GetGraveyardReference_C(OwningPlayerID);
		if (CardDataStruct.DeckSettings.AddToPlayerDeck && Graveyard)
		{
			Graveyard->AddToGraveyard_C(this, "None");
			Multicast_MoveToGraveyard_C();
		}
		else
		{
			SetCardHiddenState_C(true);
			RemoveCardActor_C();
		}
		GameState_Ref->RemoveCardOnBoard_C(this, OwningPlayerID);
		PlacementOwner->Event_RemoveCardFromPlacement_C(this);
	}
}

void ACard3D::PlayMovementAnimation(E3DCardMovementState InCardMovementState, FVector ManualGoalDestination, FRotator ManualGoalRotation, AActor* ORGoalActor, float ZOffset, float InErrorTolerance, float InInterpSpeed)
{
	CardMovementState = InCardMovementState;
	if (ORGoalActor)
	{
		FVector ReturnLoaction;
		ReturnLoaction.X = ORGoalActor->GetActorLocation().X;
		ReturnLoaction.Y = ORGoalActor->GetActorLocation().Y;
		ReturnLoaction.Z = ORGoalActor->GetActorLocation().Z + ZOffset;
		GoalDestination = ReturnLoaction;
		GoalRotation = ORGoalActor->GetActorRotation();
	}
	else
	{
		GoalDestination = ManualGoalDestination;
		GoalRotation = ManualGoalRotation;
	}
	ErrorTolerance = InErrorTolerance;
	InInterpSpeed = InInterpSpeed;
	OpenGate_C();
}

void ACard3D::RemoveMouseOverPreview()
{
	if (PreviewEnabled)
	{
		PreviewEnabled = false;
		if (ViewCardPreview)
		{
			ViewCardPreview->RemoveFromParent();
		}
	}
}

void ACard3D::SaveCardStateToStruct()
{
	CardDataStruct.Attack.Damage = Attack;
	CardDataStruct.Health.Health = Health;
	CardDataStruct.Health.Life_Expectancy = LifeExpectancy - TurnsAlive;
	CardDataStruct.Placement_Settings.Mana_Cost = Mana_Cost_Placement;
}

void ACard3D::ForceMoveCardDirectlyToGraveyard_C_Implementation(AGraveyard* Graveyard)
{
	FRotator WorldRotationForPlayer;
	UMisc_FunctionLibrary::GetWorldRotationForPlayer(FRotator(0,0,0), WorldRotationForPlayer);
	this->SetActorRotation(FQuat(WorldRotationForPlayer));
	CardBase->SetMaterial(0, OuterMaterial);
	PlayMovementAnimation(E3DCardMovementState::Graveyard, Graveyard->GetCardInGraveyardLocation_C(-1), Graveyard->GetActorRotation(), nullptr, 50, 2.5, 12);
}

void ACard3D::SetCardHiddenState_C_Implementation(bool bNewHidden)
{
	SetActorHiddenInGame(bNewHidden);
	CardBase->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACard3D::Multicast_AbilityVisual_C_Implementation()
{
	//StartCoroutine("CoroAbilityColourChange", CoroAbilityColourChange());
}

void ACard3D::RemoveCardActor_C_Implementation()
{
	StartCoroutine("CoroRemoveCardActor", CoroRemoveCardActor());
}

void ACard3D::OnRep_OwningPlayerID_C()
{
	
}

void ACard3D::OnRep_bCardActive_C()
{
	Multicast_UpdateCardVisual_C(bCardActive);
}
	
void ACard3D::OnRep_Attack_C()
{
	Multicast_UpdateVisualStats_C();
	if (HasAuthority())
	{
		if (bIsPlaced && !bInGraveyard)
		{
			SaveCardStateToStruct();
		}
	}
}

void ACard3D::OnRep_Health_C()
{
	Multicast_UpdateVisualStats_C();
	if (HasAuthority())
	{
		if (bIsPlaced && !bInGraveyard)
		{
			SaveCardStateToStruct();
		}
	}
}

void ACard3D::OnRep_Mana_Cost_Placement_C()
{
	Multicast_UpdateVisualStats_C();
	if (HasAuthority())
	{
		if (bIsPlaced && !bInGraveyard)
		{
			SaveCardStateToStruct();
		}
	}
}

void ACard3D::OnRep_TurnsAlive_C()
{
	Multicast_UpdateVisualStats_C();
	if (HasAuthority())
	{
		if (bIsPlaced && !bInGraveyard)
		{
			SaveCardStateToStruct();
		}
	}
}

void ACard3D::OnRep_Type_C()
{
	CardType_Text->Text = FText::FromString(UMath_Library::GetEnumValueAsString("ECardType", Type));
}

WHCoroTask* ACard3D::CoroMoveToGravyard()
{
	WHCORO_PARAM(ACard3D)
#include WHCORO_BEGIN()
#include WHYIELD_READY()	
		WHYIELD_RETURN_SECOND(1.0f);
	D->MoveCardToGravyard();
	D->Multicast_SpawnEffect_C(ECardEffects::OnDeath);
#include WHCORO_END()
}

WHCoroTask* ACard3D::CoreMoveCardtoGravyard()
{
	WHCORO_PARAM(ACard3D)
#include WHCORO_BEGIN()
#include WHYIELD_READY()	
		WHYIELD_RETURN_SECOND(1.0f);
		D->MoveCardToGravyard();
#include WHCORO_END()
}

WHCoroTask* ACard3D::CoroAbilityColourChange()
{
	WHCORO_PARAM(ACard3D)
#include WHCORO_BEGIN()
	D->ChangeTurnStateVisual(ETurnState::UpdateCurrentState);
#include WHYIELD_READY()	
		WHYIELD_RETURN_SECOND(1.0f);
	D->CardFrameMaterial_Instance->SetVectorParameterValue("Tint", FLinearColor(0,0.82,1));
	D->CardImageMaterial_Instance->SetVectorParameterValue("Tint", FLinearColor(0, 0.82, 1));
	D->CardFrameMaterial_Instance->SetScalarParameterValue("Brightness", 2);
	D->CardImageMaterial_Instance->SetScalarParameterValue("Brightness", 2);
#include WHYIELD_READY()	
		WHYIELD_RETURN_SECOND(0.3f);
	D->ChangeTurnStateVisual(ETurnState::UpdateCurrentState);
#include WHCORO_END()
}

WHCoroTask* ACard3D::CoroRemoveCardActor()
{
	WHCORO_PARAM(ACard3D)
#include WHCORO_BEGIN()
#include WHYIELD_READY()	
		WHYIELD_RETURN_SECOND(0.5f);
	D->Destroy();
#include WHCORO_END()
}

WHCoroTask* ACard3D::DamageColourChange2()
{
	WHCORO_PARAM(ACard3D)
#include WHCORO_BEGIN()
		D->ChangeTurnStateVisual(ETurnState::UpdateCurrentState);
#include WHYIELD_READY()	
	WHYIELD_RETURN_SECOND(1.0f);
	D->CardFrameMaterial_Instance->SetVectorParameterValue("Tint", FLinearColor(0.6, 0, 0.05));
	D->CardImageMaterial_Instance->SetVectorParameterValue("Tint", FLinearColor(0.6, 0, 0.05));
	D->CardFrameMaterial_Instance->SetScalarParameterValue("Brightness", 1.5);
	D->CardImageMaterial_Instance->SetScalarParameterValue("Brightness", 1.5);
#include WHYIELD_READY()	
	WHYIELD_RETURN_SECOND(0.3f);
	D->ChangeTurnStateVisual(ETurnState::UpdateCurrentState);
#include WHCORO_END()
}
