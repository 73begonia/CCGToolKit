// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CCGToolkit/CardGame/Structs/Structs.h"
#include "Define.generated.h"

/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UDefine : public UObject
{
	GENERATED_BODY()
	
};

#define WHCORO_PARAM(UserClass); \
struct HGCoroTask : public WHCoroTask \
{ \
	UserClass* D; \
	HGCoroTask(UserClass* Data, int32 CoroCount) : WHCoroTask(CoroCount) { D = Data; }


//Work方法开头
#define WHCORO_WORK_START \
virtual void Work(float DeltaTime) override \
{ \
	goto WHCORO_LABLE_PICKER; \
WHCORO_LABLE_START:



//Work方法中间
#define WHCORO_WORK_MIDDLE \
	goto WHCORO_LABLE_END; \
WHCORO_LABLE_PICKER:


//Work方法结尾
#define WHCORO_WORK_END \
		goto WHCORO_LABLE_START; \
	WHCORO_LABLE_END: \
		; \
	} \
}; \
return new HGCoroTask(this, WHYIELD_COUNT);

#define WHCORO_BEGIN() "CCGToolkit/CardGame/WHCoroBegin.h"

#define WHCORO_END() "CCGToolkit/CardGame/WHCoroEnd.h"

#define WHYIELD_READY() "CCGToolkit/CardGame/WHYieldReady.h"

#define WHYIELD_RETURN_TICK(Tick); \
if (CoroStack[WHYIELD_COUNT]->UpDateOperate(Tick)) \
	goto WHCORO_LABLE_END;

#define WHYIELD_RETURN_SECOND(Time); \
if (CoroStack[WHYIELD_COUNT]->UpDateOperate(DeltaTime, Time)) \
	goto WHCORO_LABLE_END;

#define WHYIELD_RETURN_BOOL(Param); \
if (CoroStack[WHYIELD_COUNT]->UpDateOperate(Param)) \
	goto WHCORO_LABLE_END;

#define WHYIELD_RETURN_FUN(UserClass, UserFunc); \
if (CoroStack[WHYIELD_COUNT]->UpDateOperate(UserClass, UserFunc)) \
	goto WHCORO_LABLE_END;

#define WHYIELD_RETURN_LAMB(Experssion); \
if (CoroStack[WHYIELD_COUNT]->UpDateOperate([this](){ return Experssion; })) \
	goto WHCORO_LABLE_END;

#define WHYIELD_RETURN_STOP(); \
if (CoroStack[WHYIELD_COUNT]->UpDateOperate()) \
	goto WHCORO_LABLE_END;
