// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGame_FunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "../SaveGame/CardGameSave.h"
#include "../SaveGame/CustomDeckSave.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"

void USaveGame_FunctionLibrary::RemoveSaveGameSlot(FString SaveGame)
{
	if (UGameplayStatics::DoesSaveGameExist(SaveGame, 0))
	{
		UCardGameSave* CardGameSave = Cast<UCardGameSave>(UGameplayStatics::LoadGameFromSlot("CardGameSave", 0));
		if (CardGameSave)
		{
			CardGameSave->DeckList.Remove(SaveGame);
			UGameplayStatics::SaveGameToSlot(CardGameSave, "CardGameSave" , 0);
			UGameplayStatics::DeleteGameInSlot(SaveGame, 0);
		}
	}
}

bool USaveGame_FunctionLibrary::CreateOrLoadCardGameSave(FString SlotName, TSubclassOf<USaveGame> SaveGameClass, USaveGame*& SaveGame)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		SaveGame = UGameplayStatics::LoadGameFromSlot(SlotName, 0);
		return true;
	}
	else
	{
		USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(SaveGameClass);
		bool Valid = UGameplayStatics::SaveGameToSlot(SaveGameObject, SlotName, 0);
		SaveGame = SaveGameObject;
		return Valid;
	}
}

bool USaveGame_FunctionLibrary::SaveCustomDeck(FText InText, TArray<FName> CustomDeck, bool bEditable)
{
	USaveGame* SaveGameObject_1;
	USaveGame* SaveGameObject_2;
	if (!UGameplayStatics::DoesSaveGameExist("CardGameSave", 0))
	{
		UClass* CardGameSaveClass = LoadClass<USaveGame>(NULL, TEXT("Blueprint'/Game/CCGToolkit/Blueprints/SaveGame/CardGameSave.CardGameSave_C'"));
		USaveGame_FunctionLibrary::CreateOrLoadCardGameSave("CardGameSave", CardGameSaveClass, SaveGameObject_1);
	}
	USaveGame* SaveGameObject = UGameplayStatics::LoadGameFromSlot("CardGameSave", 0);
	UCardGameSave* CardGameSave = Cast<UCardGameSave>(SaveGameObject);
	if (CardGameSave)
	{
		if (!(CardGameSave->DeckList.Find(InText.ToString()) >= 0))
		{
			CardGameSave->DeckList.Add(InText.ToString());
			UGameplayStatics::SaveGameToSlot(CardGameSave, "CardGameSave", 0);
			UClass* CustomDeckSave = LoadClass<USaveGame>(NULL, TEXT("Blueprint'/Game/CCGToolkit/Blueprints/SaveGame/CustomDeckSave.CustomDeckSave_C'"));
			USaveGame_FunctionLibrary::CreateOrLoadCardGameSave(InText.ToString(), CustomDeckSave, SaveGameObject_2);
		}

		UCustomDeckSave* CustomDeckSave = Cast<UCustomDeckSave>(UGameplayStatics::LoadGameFromSlot(InText.ToString(), 0));
		if (CustomDeckSave)
		{
			CustomDeckSave->CustomDeck = CustomDeck;
			CustomDeckSave->bEditable = bEditable;
			bool Valid = UGameplayStatics::SaveGameToSlot(CustomDeckSave, InText.ToString(), 0);
			return Valid;
		}
		else
		{
			return false;
		}
		
	}
	else
	{
		return false;
	}
	
}

bool USaveGame_FunctionLibrary::LoadCustomDeck(FString DeckName, TArray<FName>& CustomDeck, bool& bDeckEditable)
{
	bool Valid = UGameplayStatics::DoesSaveGameExist(DeckName, 0);
	if (Valid)
	{
		UCustomDeckSave* CustomDeckSave = Cast<UCustomDeckSave>(UGameplayStatics::LoadGameFromSlot(DeckName, 0));
		if (CustomDeckSave)
		{
			CustomDeck = CustomDeckSave->CustomDeck;
			bDeckEditable = CustomDeckSave->bEditable;
			return true;
		}
		else
		{
			CustomDeck = {"None"};
			bDeckEditable = false;
			return false;
		}
	}
	else
	{
		CustomDeck = { "None" };
		bDeckEditable = false;
		return false;
	}
}

bool USaveGame_FunctionLibrary::GetRandomDeck(FString& RandomDeck)
{
	if (UGameplayStatics::DoesSaveGameExist("CardGameSave", 0))
	{
		UCardGameSave* CardGameSave = Cast<UCardGameSave>(UGameplayStatics::LoadGameFromSlot("CardGameSave", 0));
		RandomDeck = CardGameSave->DeckList[UKismetMathLibrary::RandomIntegerInRange(0, CardGameSave->DeckList.Num()-1)];
		return true;
	}
	else
	{
		UKismetSystemLibrary::PrintString(GWorld->GetWorld(), "No Valid 'CardGameSave' Found");
		RandomDeck = "";
		return false;
	}
}

bool USaveGame_FunctionLibrary::GetDeckValid(FString SlotName)
{
	return UGameplayStatics::DoesSaveGameExist(SlotName, 0);
}

bool USaveGame_FunctionLibrary::GetDeckEditable(FString SlotName)
{
	return Cast<UCustomDeckSave>(UGameplayStatics::LoadGameFromSlot(SlotName, 0))->bEditable;
}
