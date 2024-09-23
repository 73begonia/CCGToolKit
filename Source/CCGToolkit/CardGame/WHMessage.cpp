// Fill out your copyright notice in the Description page of Project Settings.


#include "WHMessage.h"

// Sets default values
AWHMessage::AWHMessage()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWHMessage::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AWHMessage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<FName> CompleteTask;
	for (TMap<FName, TMap<FName, WHCoroTask*>>::TIterator It(CoroStack); It; ++It)
	{
		TArray<FName> CompleteNode;
		for (TMap<FName, WHCoroTask*>::TIterator Ih(It->Value); Ih; ++Ih)
		{
			Ih->Value->Work(DeltaTime);
			if (Ih->Value->IsFinish() || Ih->Value->IsDestory)
			{
				delete Ih->Value;
				CompleteNode.Push(Ih->Key);
			}
		}
		for (int i = 0; i < CompleteNode.Num(); ++i)
			It->Value.Remove(CompleteNode[i]);
		if (It->Value.Num() == 0)
			CompleteTask.Push(It->Key);
	}
	for (int i = 0; i < CompleteTask.Num(); ++i)
		CoroStack.Remove(CompleteTask[i]);
}

bool AWHMessage::StartCoroutine(FName ObjectName, FName CoroName, WHCoroTask* CoroTask)
{
	if (!CoroStack.Contains(ObjectName))
	{
		TMap<FName, WHCoroTask*> NewTaskStack;
		CoroStack.Add(ObjectName, NewTaskStack);
	}
	if (!(CoroStack.Find(ObjectName)->Contains(CoroName)))
	{
		CoroStack.Find(ObjectName)->Add(CoroName, CoroTask);
		return true;
	}
	delete CoroTask;
	return false;
}

bool AWHMessage::StopCoroutine(FName ObjectName, FName CoroName)
{
	if (CoroStack.Contains(ObjectName) && CoroStack.Find(ObjectName)->Find(CoroName))
	{
		(*(CoroStack.Find(ObjectName)->Find(CoroName)))->IsDestory = true;
		return true;
	}
	return false;
}

void AWHMessage::StopAllCorotine(FName ObjectName)
{
	if (CoroStack.Contains(ObjectName))
		for (TMap<FName, WHCoroTask*>::TIterator It(*CoroStack.Find(ObjectName)); It; ++It)
			It->Value->IsDestory = true;
}

