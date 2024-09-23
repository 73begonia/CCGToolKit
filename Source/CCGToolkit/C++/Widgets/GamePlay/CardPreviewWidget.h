// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../GamePlay/Card3D.h"
#include "CardPreviewWidget.generated.h"

/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UCardPreviewWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent)
	void InitiateCardPreview(FName CardName, ECardSet CardSet, ACard3D* CallingParent, float Scale);
};
