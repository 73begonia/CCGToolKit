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

	//����һ��Э��, ����true˵�������ɹ�, ����false˵���Ѿ���ͬ������ͬЭ����������Э�̴���
	bool StartCoroutine(FName ObjectName, FName CoroName, WHCoroTask* CoroTask);

	//ֹͣһ��Э��, ����true˵��ֹͣЭ�̳ɹ�, ����false˵��Э�����Ѿ�ֹͣ
	bool StopCoroutine(FName ObjectName, FName CoroName);

	//ֹͣ�ö��������Э��
	void StopAllCorotine(FName ObjectName);

protected:
	//Э������, ��1���������, ֵ�ļ�FName��Ӧ����Э�����������
	TMap<FName, TMap<FName, WHCoroTask*>> CoroStack;

};
