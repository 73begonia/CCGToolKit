// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CCGToolKit/C++/Enums/CardGameEnums.h"
#include "StackState_Interface.generated.h"

// This class does not need to be modified.
UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class UStackState_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CCGTOOLKIT_API IStackState_Interface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void EnterState(EStackAction StackAction) = 0;
	virtual void ExitState(EStackAction StackAction) = 0;
	virtual void UpdateState(float DeltaTime) = 0;
};
