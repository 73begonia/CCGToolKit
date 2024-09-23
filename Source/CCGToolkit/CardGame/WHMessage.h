// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CCGToolkit/CardGame/Structs/Structs.h"
#include "WHMessage.generated.h"

UCLASS()
class CCGTOOLKIT_API AWHMessage : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AWHMessage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//开启一个协程, 返回true说明开启成功, 返回false说明已经有同对象名同协程任务名的协程存在
	bool StartCoroutine(FName ObjectName, FName CoroName, WHCoroTask* CoroTask);

	//停止一个协程, 返回true说明停止协程成功, 返回false说明协程早已经停止
	bool StopCoroutine(FName ObjectName, FName CoroName);

	//停止该对象的所有协程
	void StopAllCorotine(FName ObjectName);

protected:
	//协程序列, 键1保存对象名, 值的键FName对应的是协程任务的名字
	TMap<FName, TMap<FName, WHCoroTask*>> CoroStack;

};
