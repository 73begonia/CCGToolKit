// Fill out your copyright notice in the Description page of Project Settings.


#include "CardGameAI_Pawn.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "../../C++/CardGamePlayerController.h"
#include "../../C++/FunctionLibraries/Controller_FunctionLibrary.h"
#include "CardGameAIController.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

// Sets default values
ACardGameAI_Pawn::ACardGameAI_Pawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACardGameAI_Pawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACardGameAI_Pawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACardGameAI_Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACardGameAI_Pawn::PossessedBy(AController* NewController)
{
	GameMode_Ref = Cast<ACardGameMode>(UGameplayStatics::GetGameMode(this));
	//InitAttributes();
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, UKismetSystemLibrary::GetDisplayName(this).Append(" ------------- > ID: ").Append(FString::FromInt(PlayerID)).Append("< --------------"));

	//StartCoroutine("CoroPossessed", CoroPossessed());
}

WHCoroTask* ACardGameAI_Pawn::CoroPossessed()
{
	WHCORO_PARAM(ACardGameAI_Pawn)

#include WHCORO_BEGIN()

#include WHYIELD_READY()
	WHYIELD_RETURN_SECOND(0.0f);
	D->GameMode_Ref = Cast<ACardGameMode>(UGameplayStatics::GetGameMode(D));
	D->InitAttributes();
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, UKismetSystemLibrary::GetDisplayName(D).Append(" ------------- > ID: ").Append(FString::FromInt(D->PlayerID)).Append("< --------------"));

#include WHCORO_END()
}

void ACardGameAI_Pawn::UpDateAIState(int32 InCardsInHand, int32 InCardsINDeck)
{
	CardsInHand = InCardsInHand;
	OnRep_CardsInHand_C();
	CardsInDeck = InCardsINDeck;
	OnRep_CardsInDeck_C();
	UpdtaeUI();
}

void ACardGameAI_Pawn::InitAttributes()
{
	InitHealth();
}

void ACardGameAI_Pawn::InitHealth()
{
	if (HasAuthority())
	{
		if (GameMode_Ref == nullptr)
		{
			GameMode_Ref = Cast<ACardGameMode>(UGameplayStatics::GetGameMode(this));
		}
		Health = GameMode_Ref->PlayerStartingHealth;
		OnRep_Health_C();
	}
}

void ACardGameAI_Pawn::InitMana()
{
	Mana = 0;
	OnRep_Mana_C();
}

bool ACardGameAI_Pawn::AddCardToPlayersHand()
{
	return false;
}

void ACardGameAI_Pawn::CheckGameOverConditions()
{
	GameMode_Ref->Server_CheckPlayerState_C();
}

ACard3D* ACardGameAI_Pawn::CreatePlayableCard(FTransform SpawnTransfrom)
{
	return nullptr;
}

void ACardGameAI_Pawn::UpdtaeUI()
{
	ACardGamePlayerController* CardGamePlayerController = nullptr;
	if (UWorld* World = GEngine->GetWorldFromContextObject(GWorld, EGetWorldErrorMode::LogAndReturnNull))
	{
		uint32 Index = 0;
		for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			CardGamePlayerController = Cast<ACardGamePlayerController>(Iterator->Get());
		}
	}
	if (CardGamePlayerController)
	{
		CardGamePlayerController->UpdateGameUI_C(CardsInHand == 0);
	}
}

int32 ACardGameAI_Pawn::GetManaForTurn_C_Implementation()
{
	Mana = GameMode_Ref->GetTurnMana(OwningController_Ref);
	OnRep_Mana_C();
	ManaMax = GameMode_Ref->GetTurnMana(OwningController_Ref);
	return Mana;
}

int32 ACardGameAI_Pawn::IncreasePlayerHealth_C_Implementation(int32 IncreaseBy)
{
	if (HasAuthority())
	{
		Health = FMath::Clamp(Health + IncreaseBy, 0, 100);
		OnRep_Health_C();
		return Health;
	}
	else
	{
		return Health;
	}
}

int32 ACardGameAI_Pawn::DecreasePlayerHealth_C_Implementation(int32 DecreaseBy)
{
	if (HasAuthority())
	{
		Health = FMath::Clamp(Health - DecreaseBy, 0, 100);
		OnRep_Health_C();
		CheckGameOverConditions();
		return Health;
	}
	else
	{
		return Health;
	}
}

int32 ACardGameAI_Pawn::IncreasePlayerMana_C_Implementation(int32 IncreaseBy)
{
	if (HasAuthority())
	{
		Mana = FMath::Clamp(Mana + IncreaseBy, 0, ManaMax);
		return Mana;
	}
	else
	{
		return Mana;
	}
}

int32 ACardGameAI_Pawn::DecreasePlayerMana_C_Implementation(int32 DecreaseBy)
{
	if (HasAuthority())
	{
		Mana = FMath::Clamp(Mana - DecreaseBy, 0, ManaMax);
		return Mana;
	}
	else
	{
		return Mana;
	}
}

void ACardGameAI_Pawn::ForceUIUpdate_Implementation(int32 InCardsInHand, int32 InCardsInDeck)
{
	CardsInHand = InCardsInHand;
	OnRep_CardsInHand_C();
	CardsInDeck = InCardsInDeck;
	OnRep_CardsInDeck_C();
}

void ACardGameAI_Pawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACardGameAI_Pawn, Health);
	DOREPLIFETIME(ACardGameAI_Pawn, Mana);
	DOREPLIFETIME(ACardGameAI_Pawn, ManaMax);
	DOREPLIFETIME(ACardGameAI_Pawn, HealthMax);
	DOREPLIFETIME(ACardGameAI_Pawn, CardsInHand);
	DOREPLIFETIME(ACardGameAI_Pawn, CardsInDeck);
	DOREPLIFETIME(ACardGameAI_Pawn, ActiveCards);
	DOREPLIFETIME(ACardGameAI_Pawn, PlayerTurn);
	DOREPLIFETIME(ACardGameAI_Pawn, PlayerID);
	DOREPLIFETIME(ACardGameAI_Pawn, CardGameAIID);
}

void ACardGameAI_Pawn::OnRep_Health_C()
{
	if (HasAuthority())
	{
		ACardGameAIController* CardGameAIController;
		ACardGameAI_Pawn* CardGameAIPawn;
		UController_FunctionLibrary::GetAIControllerReference_ServerOnly(OwningController_Ref, CardGameAIController, CardGameAIPawn);
		if (CardGameAIController->BoardPlayer_Ref)
		{
			CardGameAIController->BoardPlayer_Ref->Update_Health_C(Health);
		}
	}
	UpdtaeUI();
}

void ACardGameAI_Pawn::OnRep_Mana_C()
{
	UpdtaeUI();
}

void ACardGameAI_Pawn::OnRep_CardsInHand_C()
{
	UpdtaeUI();
}

void ACardGameAI_Pawn::OnRep_CardsInDeck_C()
{
	UpdtaeUI();
}

void ACardGameAI_Pawn::OnRep_PlayerTurn_C()
{
	UpdtaeUI();
}

