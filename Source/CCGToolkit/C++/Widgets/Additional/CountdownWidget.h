// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CountdownWidget.generated.h"

/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UCountdownWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	int32 Timer = 3;
};
