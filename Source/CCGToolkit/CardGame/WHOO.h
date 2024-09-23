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

	//����һ��Э��, ����true˵�������ɹ�, ����false˵���Ѿ���ͬ������ͬЭ����������Э�̴���
	bool StartCoroutine(FName CoroName, WHCoroTask* CoroTask);

	//ֹͣһ��Э��, ����true˵��ֹͣЭ�̳ɹ�, ����false˵��Э�����Ѿ�ֹͣ
	bool StopCoroutine(FName CoroName);

	//ֹͣ�ö��������Э��
	void StopAllCorotine();

protected:

	UObject* IBody;

	FName IObjectName;

	AWHMessage* WHMessage;
};
