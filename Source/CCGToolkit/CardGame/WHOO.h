// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CCGToolkit/CardGame/Structs/Structs.h"
#include "CCGToolkit/CardGame/WHMessage.h"
#include "WHOO.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWHOO : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CCGTOOLKIT_API IWHOO
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	FName GetObjectName();

	//开启一个协程, 返回true说明开启成功, 返回false说明已经有同对象名同协程任务名的协程存在
	bool StartCoroutine(FName CoroName, WHCoroTask* CoroTask);

	//停止一个协程, 返回true说明停止协程成功, 返回false说明协程早已经停止
	bool StopCoroutine(FName CoroName);

	//停止该对象的所有协程
	void StopAllCorotine();

protected:

	UObject* IBody;

	FName IObjectName;

	AWHMessage* WHMessage;
};
