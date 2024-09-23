// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Slate/WidgetTransform.h"
#include "Components/Widget.h"
#include "CardLayoutTypes_FunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UCardLayoutTypes_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/*static FWidgetTransform Fanned(UWidget* CardWidgetReference, int32 CardLoopIndex, int32 CardsInHand, int32 MaxCardsInHand, FVector2D ScreenResolution, FVector2D CardSize,
		bool FlyInDirection, bool RotateCardsToOffset, float CardOffset, float RotationDistanceScaleFactor, float RadiusOffset, float CardSpacing, bool DynamicSpacing, float DynamicSpacingMaxOffset,
		float ScreenOffsetX, float ScreenOffsetY, float ViewportScale, float CardScale);

	static FWidgetTransform Horizontal(UWidget* CardWidgetReference, int32 CardLoopIndex, int32 CardsInHand, int32 MaxCardsInHand, FVector2D ScreenResolution, FVector2D CardSize,
		bool FlyInDirection, bool RotateCardsToOffset, float RadiusOffset, float RotationDistanceScaleFactor, float CardSpacing, bool DynamicSpacing, bool CardSpacingMax, float ScreenOffsetX,
		float ScreenOffsetY, float CardScale, float ViewportScale, float CardOffset);*/
};
