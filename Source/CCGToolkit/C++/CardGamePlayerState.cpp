// Fill out your copyright notice in the Description page of Project Settings.


#include "CardGamePlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"

ACardGamePlayerState::ACardGamePlayerState()
{
	bReplicates = true;
}

void ACardGamePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACardGamePlayerState, CardGamePlayerId);
	DOREPLIFETIME(ACardGamePlayerState, PlayerTurn);
	DOREPLIFETIME(ACardGamePlayerState, CardsInHand_Array);
	DOREPLIFETIME(ACardGamePlayerState, OwningPlayerController);
	DOREPLIFETIME(ACardGamePlayerState, Mana);
	DOREPLIFETIME(ACardGamePlayerState, Mana_Max);
	DOREPLIFETIME(ACardGamePlayerState, Health);
	DOREPLIFETIME(ACardGamePlayerState, CardsInHand);
	DOREPLIFETIME(ACardGamePlayerState, CardsInDeck);
	DOREPLIFETIME(ACardGamePlayerState, ActiveCards);
}

void ACardGamePlayerState::BeginPlay()
{
	GameMode_Ref = Cast<ACardGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	InitAttributes_C();
}

void ACardGamePlayerState::Server_NotifyPlayerStateChange_C_Implementation()
{
	ACardGameMode* CardGameMode = Cast<ACardGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (CardGameMode)
	{
		if (CardGameMode->bGameActive)
		{
			if (Health <= 0)
			{
				CardGameMode->Server_CheckPlayerState_C();
			}
		}
	}
}

void ACardGamePlayerState::ForceUpdateUI_C_Implementation()
{
	UpdateUI_C();
}

void ACardGamePlayerState::InitAttributes_C()
{
	if (HasAuthority())
	{
		Mana = 0;
		OnRep_Mana_C();
		Health = GameMode_Ref->PlayerStartingHealth;
		OnRep_PlayerHealth_C();
	}
}

void ACardGamePlayerState::InitMana_C()
{
	if (HasAuthority())
	{
		Mana = 0;
		OnRep_Mana_C();
	}
}

void ACardGamePlayerState::InitHealth_C()
{
	if (HasAuthority())
	{
		Health = GameMode_Ref->PlayerStartingHealth;
		OnRep_PlayerHealth_C();
	}
}

void ACardGamePlayerState::UpdatePlayersCardStates_C(int32 CardsInHand_P, int32 CardsInDeck_P, int32 ActiveCards_P)
{
	if (HasAuthority())
	{
		CardsInHand = CardsInHand_P;
		OnRep_CardsInHand_C();
		CardsInDeck = CardsInDeck_P;
		OnRep_CardsInDeck_C();
		ActiveCards = ActiveCards_P;
		OnRep_ActiveCards_C();
	}
}

void ACardGamePlayerState::Set_Mana_C(int32 Mana_P)
{
	if (HasAuthority())
	{
		Mana_Max = Mana = Mana_P;
		OnRep_Mana_Max_C();
		Mana = Mana_P;
		OnRep_Mana_C();
		UpdateUI_C();
	}
	
}

ACard3D* ACardGamePlayerState::CreatePlayableCard_C(FTransform SpawnTransform)
{
	return nullptr;
}

bool ACardGamePlayerState::AddCardToPlayersHand_C(FName CardName)
{
	return false;
}

void ACardGamePlayerState::OnRep_PlayerHealth_C()
{
	UpdateUI_C();
	if (HasAuthority())
	{
		if (Health <= 0)
		{
			Server_NotifyPlayerStateChange_C();
		}
	}
}

void ACardGamePlayerState::OnRep_CardsInHand_Array_C()
{
	UpdateUI_C();
	if (CardsInDeck <= 0 && CardsInHand <= 0 && ActiveCards <= 0)
	{
		Server_NotifyPlayerStateChange_C();
	}
}

void ACardGamePlayerState::OnRep_CardsInDeck_C()
{
	UpdateUI_C();
	if (CardsInDeck <= 0 && CardsInHand <= 0 && ActiveCards <= 0)
	{
		Server_NotifyPlayerStateChange_C();
	}
}

void ACardGamePlayerState::OnRep_ActiveCards_C()
{
	UpdateUI_C();
	if (HasAuthority())
	{
		if (CardsInDeck <= 0 && CardsInHand <= 0 && ActiveCards <= 0)
		{
			Server_NotifyPlayerStateChange_C();
		}
	}
}

void ACardGamePlayerState::OnRep_CardsInHand_C()
{
	UpdateUI_C();
}

void ACardGamePlayerState::OnRep_Mana_C()
{
	UpdateUI_C();
}

void ACardGamePlayerState::OnRep_PlayerTurn_C()
{
	UpdateUI_C();
}

void ACardGamePlayerState::OnRep_Mana_Max_C()
{
	UpdateUI_C();
}

int32 ACardGamePlayerState::GetManaForTurn_C_Implementation()
{
	Mana = GameMode_Ref->GetTurnMana(OwningPlayerController);
	OnRep_Mana_C();
	Mana_Max = GameMode_Ref->GetTurnMana(OwningPlayerController);
	OnRep_Mana_Max_C();
	return Mana;
}

void ACardGamePlayerState::UpdateUI_C()
{
	ACardGamePlayerController* CardGamePlayerController = Cast<ACardGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (CardGamePlayerController)
	{
		CardGamePlayerController->UpdateGameUI_C(CardGamePlayerController->PlayerState != this && CardsInHand == 0);
	}
}

int32 ACardGamePlayerState::DecreasePlayerMana_C_Implementation(int32 DecreaseBy)
{
	if (HasAuthority())
	{
		Mana = UKismetMathLibrary::Clamp(Mana - DecreaseBy, 0, Mana_Max);
		OnRep_Mana_C();
		return Mana;
	}
	else
	{
		return 0;
	}
}

int32 ACardGamePlayerState::IncreasePlayerHealth_C_Implementation(int32 IncreaseBy)
{
	if (HasAuthority())
	{
		Health = UKismetMathLibrary::Clamp(Health + IncreaseBy, 0, 100);
		OnRep_PlayerHealth_C();
		return Health;
	}
	else
	{
		return Health;
	}
}

int32 ACardGamePlayerState::DecreasePlayerHealth_C_Implementation(int32 DecreaseBy)
{
	if (HasAuthority())
	{
		Health = UKismetMathLibrary::Clamp(Health - DecreaseBy, 0 , 100);
		OnRep_PlayerHealth_C();
		return Health;
	}
	else
	{
		return Health;
	}
}

int32 ACardGamePlayerState::IncreasePlayerMana_C_Implementation(int32 IncreaseBy)
{
	if (HasAuthority())
	{
		Mana = UKismetMathLibrary::Clamp(Mana + IncreaseBy, 0, Mana_Max);
		OnRep_Mana_C();
		return Mana;
	}
	else
	{
		return Mana;
	}
}
