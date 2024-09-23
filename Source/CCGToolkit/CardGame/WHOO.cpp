// Fill out your copyright notice in the Description page of Project Settings.


#include "WHOO.h"
#include "Engine/World.h"

FName IWHOO::GetObjectName()
{
	IBody = Cast<UObject>(this);
	if (!IObjectName.IsNone())
		return IObjectName;
	IObjectName = IBody->GetFName();
	return IObjectName;
}

// Add default functionality here for any IWHOO functions that are not pure virtual.
bool IWHOO::StartCoroutine(FName CoroName, WHCoroTask* CoroTask)
{
	WHMessage = GWorld->SpawnActor<AWHMessage>();
	return WHMessage->StartCoroutine(GetObjectName(), CoroName, CoroTask);
}

bool IWHOO::StopCoroutine(FName CoroName)
{
	return WHMessage->StopCoroutine(GetObjectName(), CoroName);
}

void IWHOO::StopAllCorotine()
{
	return WHMessage->StopAllCorotine(GetObjectName());
}
