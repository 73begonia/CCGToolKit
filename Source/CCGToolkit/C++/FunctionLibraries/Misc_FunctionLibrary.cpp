// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc_FunctionLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "GameFramework/Controller.h"
#include "Controller_FunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "../Widgets/Additional/DisplayMessageWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Slate/WidgetTransform.h"
#include "Engine/EngineTypes.h"

float UMisc_FunctionLibrary::CalculateFloatPrecision(float Float)
{
	TArray<FString> TargetArray = UKismetStringLibrary::ParseIntoArray(FString::FromInt(Float),".",true);
	FString Dot = ".";

	return UKismetStringLibrary::Conv_StringToFloat(TargetArray[0].Append(Dot.Append(UKismetStringLibrary::GetSubstring(TargetArray[1], 0, 1))));
}

float UMisc_FunctionLibrary::GetWaitTimeWithRandomDeviation(float WaitTime, float RandomDeviation)
{
	return UKismetMathLibrary::RandomFloatInRange(RandomDeviation*(-1), RandomDeviation) + WaitTime;
}
 

bool UMisc_FunctionLibrary::InterpToGoalLocation2D(FWidgetTransform CurrentTransform, FWidgetTransform GoalTransform, float DeltaTime, float InterpSpeed, FWidgetTransform& Transform)
{
	FVector2D CurrentTranslation = CurrentTransform.Translation;
	FVector2D CurrentScale = CurrentTransform.Scale;
	float CurrentAngle = CurrentTransform.Angle;

	FVector2D GoalTranslation = GoalTransform.Translation;
	FVector2D GoalScale = GoalTransform.Scale;
	float GoalAngle = GoalTransform.Angle;

	FVector2D OutTranslation = FMath::Vector2DInterpTo(CurrentTranslation, GoalTranslation, DeltaTime, InterpSpeed);
	FVector2D OutScale = FMath::Vector2DInterpTo(CurrentScale, GoalScale, DeltaTime, InterpSpeed);
	float OutAngle = FMath::FInterpTo(CurrentAngle, GoalAngle, DeltaTime, InterpSpeed);

	if (!(FMath::IsNearlyEqual(FMath::Abs(GoalTranslation.Size()), FMath::Abs(OutTranslation.Size()), 1.e-3f) &&
		FMath::IsNearlyEqual(FMath::Abs(GoalScale.Size()), FMath::Abs(OutScale.Size()), 1.e-3f) &&
		FMath::IsNearlyEqual(FMath::Abs(GoalAngle), FMath::Abs(OutAngle), 1.e-3f)))
	{
		Transform.Translation = OutTranslation;
		Transform.Scale = OutScale;
		Transform.Angle = OutAngle;
	}
	else
	{
		Transform.Translation = GoalTranslation;
		Transform.Scale = GoalScale;
		Transform.Angle = GoalAngle;
	}

	return 
	(
		FMath::IsNearlyEqual(FMath::Abs(GoalTranslation.Size()), FMath::Abs(OutTranslation.Size()), 1.e-3f) &&  
		FMath::IsNearlyEqual(FMath::Abs(GoalScale.Size()), FMath::Abs(OutScale.Size()), 1.e-3f) &&  
		FMath::IsNearlyEqual(FMath::Abs(GoalAngle), FMath::Abs(OutAngle), 1.e-3f)
	);
}

FVector UMisc_FunctionLibrary::MouseToWorldLocation(APlayerController* PlayerController, AActor*& HitActor)
{
	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, true, HitResult);
	HitActor = HitResult.GetActor();
	return HitResult.Location;
}

void UMisc_FunctionLibrary::ReadInteractingCardData(ACard3D* ReadCard, FText& CardName, int32& Attack, int32& Health)
{
	CardName = ReadCard->Name;
	Attack = ReadCard->Attack;
	Health = ReadCard->Health;
}

FVector UMisc_FunctionLibrary::MouseDistanceInWorldSpace(ACardGamePlayerController* PlayerController, float Distance, FTransform& SpawnTranfrom)
{
	FVector WorldLocation;
	FVector WorldDirection;
	FRotator ReturnRotation;

	PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
	
	GetWorldRotationForPlayer(FRotator(0,0,0), ReturnRotation);
	SpawnTranfrom.SetLocation(WorldLocation + WorldDirection*Distance);
	SpawnTranfrom.SetRotation(FQuat(ReturnRotation));
	SpawnTranfrom.SetScale3D(FVector(1.0, 1.0, 1.0));

	return WorldLocation + WorldDirection * Distance;
}

void UMisc_FunctionLibrary::CreateScreenDisplayMessage(FString Message, FLinearColor SpecifiedColor, float Duration)
{
	if (!UKismetSystemLibrary::IsDedicatedServer(GWorld->GetWorld()))
	{
		UClass* DisplayMessageWidgetClass = LoadClass<UDisplayMessageWidget>(NULL, TEXT("WidgetBlueprint'/Game/CCGToolkitC++/Blueprints/Widgets/Additional/BP_DisplayMessageWidget.BP_DisplayMessageWidget_C'"));
		UDisplayMessageWidget* DisplayMessageWidget = CreateWidget<UDisplayMessageWidget>(UGameplayStatics::GetPlayerController(GWorld->GetWorld(), 0), DisplayMessageWidgetClass);
		if (DisplayMessageWidget)
		{
			DisplayMessageWidget->DisplayMessage(Message, SpecifiedColor, Duration);
			DisplayMessageWidget->AddToViewport(5);
		}
	}
}

void UMisc_FunctionLibrary::CustomPrint(FString A, FString B, FString C, FString D, FLinearColor TextColor, float Duration, bool bPrintToScreen)
{
	UKismetSystemLibrary::PrintString(GWorld->GetWorld(), A.Append(B).Append(C).Append(D), bPrintToScreen, true);
}

void UMisc_FunctionLibrary::GetPlayerStateReference(AController* Controller, int32& SelfPlayer, int32& Oppenent)
{
	if (UController_FunctionLibrary::GetControllerID_ServerOnly(Controller) == 1)
	{
		SelfPlayer = 1;
		Oppenent = 2;
	}
	else
	{
		SelfPlayer = 2;
		Oppenent = 1;
	}
}

bool UMisc_FunctionLibrary::GetWorldRotationForPlayer(FRotator OverrideRotationAxis, FRotator& ReturnRotation)
{
	APlayerCameraManager* PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GWorld->GetWorld(), 0);
	if (PlayerCameraManager)
	{
		if (OverrideRotationAxis.Roll>0)
		{
			ReturnRotation.Roll = OverrideRotationAxis.Roll;
		}
		else
		{
			ReturnRotation.Roll = 0.0;
		}

		if (OverrideRotationAxis.Pitch>0)
		{
			ReturnRotation.Pitch = OverrideRotationAxis.Pitch;
		}
		else
		{
			ReturnRotation.Pitch = 0.0;
		}

		if (OverrideRotationAxis.Yaw>0)
		{
			ReturnRotation.Yaw = OverrideRotationAxis.Yaw;
		}
		else
		{
			ReturnRotation.Yaw = PlayerCameraManager->GetCameraRotation().Yaw;
		}

		return true;
	}
	else
	{
		ReturnRotation = FRotator(0.0,0.0,0.0);
		return false;
	}
}

FVector2D UMisc_FunctionLibrary::GetMousePositionInRange(FVector2D SizeOffset, float GlobalEdgeOffset, float XEdgeOffset, float YEdgeOffset)
{
	return FVector2D(0,0);
}

FVector2D UMisc_FunctionLibrary::GetPositionInRange(float GlobalEdgeOffset, float XEdgeOffset, float YEdgeOffset)
{
	return FVector2D(0, 0);
}

float UMisc_FunctionLibrary::ModifyDPIScaling(float Value, bool InvertDPIScaling)
{
	if (InvertDPIScaling)
	{
		return Value* (2 - UWidgetLayoutLibrary::GetViewportScale(GWorld->GetWorld()));
	}
	else
	{
		return Value / (2 - UWidgetLayoutLibrary::GetViewportScale(GWorld->GetWorld()));
	}
}

ACardGameCamera* UMisc_FunctionLibrary::GetCardGamePlayerCamera(ACardGamePlayerController* PlayerController)
{
	TArray<AActor*> OutActors;
	ACardGamePlayerState* CardGamePlayerState = Cast<ACardGamePlayerState>(PlayerController->PlayerState);
	if (CardGamePlayerState)
	{
		UGameplayStatics::GetAllActorsOfClass(GWorld->GetWorld(), ACardGameCamera::StaticClass(), OutActors);
		for (int32 i = 0; i< OutActors.Num();i++)
		{
			ACardGameCamera* CardGameCamera = Cast<ACardGameCamera>(OutActors[i]);
			if (CardGameCamera)
			{
				if (CardGameCamera->GetAutoActivatePlayerIndex()+1 == CardGamePlayerState->CardGamePlayerId)
				{
					return CardGameCamera;
				}
				else
				{
					return nullptr;
				}
			}
			else
			{
				return nullptr;
			}
		}
	}
	else
	{
		return nullptr;
	}
	return nullptr;
}
