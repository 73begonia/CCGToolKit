// Fill out your copyright notice in the Description page of Project Settings.


#include "Log_FunctionLibrary.h"

void ULog_FunctionLibrary::AddMessageToLog(ACardGamePlayerController* CallingPlayer, FString Message, FLinearColor SpecifiedColor)
{
	if (CallingPlayer->PlayerGameUI_Ref && CallingPlayer->PlayerGameUI_Ref->Message_ManagerWidget)
	{
		CallingPlayer->PlayerGameUI_Ref->Message_ManagerWidget->LogMessages_C(CallingPlayer, Message, SpecifiedColor);
	}
}

FString ULog_FunctionLibrary::CloneCardLogMessage(FText CardName, int32 AbilityInt)
{
	return "";
}

FString ULog_FunctionLibrary::PickupCard_LogMessage(int32 AbilityInt)
{
	return "";
}

FString ULog_FunctionLibrary::IncreaseAttack_LogMessage(FText CardName, int32 AbilityInt)
{
	return "";
}
