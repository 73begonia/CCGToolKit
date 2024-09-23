// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "OpponentProfileUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UOpponentProfileUIWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UpdatePlayerUI(int32 ControllerID);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void GetPlayerData(int32 PlayerID);
		
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetUIData(const FString& Name , int32 Health, int32 CardsInDeck, int32 PlayerTurn, int32 Mana, int32 ManaMax);
public:
	/*UPROPERTY(Meta = (BindWidget))
	UImage* Player_image;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* PlayerName2_Text;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* HealthInDeck_Text;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* CardsInDeck_Text;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* PlayerTurn_Text;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* Mana_Text;*/
};
