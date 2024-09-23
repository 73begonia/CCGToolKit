// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Slate/WidgetTransform.h"
#include "GameFramework/PlayerController.h"
#include "../GamePlay/Card3D.h"
#include "../CardGamePlayerController.h"
#include "../CardGameCamera.h"
#include "Misc_FunctionLibrary.generated.h"

class AController;
/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UMisc_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static bool InterpToGoalLocation2D(FWidgetTransform CurrentTransform, FWidgetTransform GoalTransform, float DeltaTime, float InterpSpeed, FWidgetTransform& Transform);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FVector MouseToWorldLocation(APlayerController* PlayerController, AActor*& HitActor);

	static void ReadInteractingCardData(ACard3D* ReadCard, FText& CardName, int32& Attack, int32& Health);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FVector MouseDistanceInWorldSpace(ACardGamePlayerController* PlayerController, float Distance, FTransform& SpawnTranfrom);

	static void CreateScreenDisplayMessage(FString Message, FLinearColor SpecifiedColor, float Duration);

	static void CustomPrint(FString A, FString B, FString C, FString D, FLinearColor TextColor, float Duration, bool bPrintToScreen);

	UFUNCTION(BlueprintCallable)
	static void GetPlayerStateReference(AController* Controller, int32& SelfPlayer, int32& Oppenent);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool GetWorldRotationForPlayer(FRotator OverrideRotationAxis, FRotator &ReturnRotation);

	static FVector2D GetMousePositionInRange(FVector2D SizeOffset, float GlobalEdgeOffset, float XEdgeOffset, float YEdgeOffset);

	static FVector2D GetPositionInRange(float GlobalEdgeOffset, float XEdgeOffset, float YEdgeOffset);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float ModifyDPIScaling(float Value, bool InvertDPIScaling);

	static ACardGameCamera* GetCardGamePlayerCamera(ACardGamePlayerController* PlayerController);

	static float CalculateFloatPrecision(float TheFloat);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float GetWaitTimeWithRandomDeviation(float WaitTime, float RandomDeviation);
};
