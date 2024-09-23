// Fill out your copyright notice in the Description page of Project Settings.


#include "AutoRotateToPlayer.h"
#include "../FunctionLibraries/Misc_FunctionLibrary.h"

// Sets default values for this component's properties
UAutoRotateToPlayer::UAutoRotateToPlayer()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAutoRotateToPlayer::BeginPlay()
{
	Super::BeginPlay();

	// ...
	StartCoroutine("DelaySetActorRotation", DelaySetActorRotation());
	UKismetSystemLibrary::PrintString(this, this->GetOwner()->GetName(), true, true, FColor::Blue, 10);
}


// Called every frame
void UAutoRotateToPlayer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

WHCoroTask* UAutoRotateToPlayer::DelaySetActorRotation()
{
	WHCORO_PARAM(UAutoRotateToPlayer)
		FRotator NewRotator = FRotator(0, 0, 0);
#include WHCORO_BEGIN()
#include WHYIELD_READY()
	WHYIELD_RETURN_SECOND(0.2f);
	if (UMisc_FunctionLibrary::GetWorldRotationForPlayer(FRotator(0, 0, 0), NewRotator))
	{
		D->GetOwner()->SetActorRotation(NewRotator);
	}
	else
	{
#include WHYIELD_READY()
		WHYIELD_RETURN_SECOND(0.2f);
		D->StartCoroutine("DelaySetActorRotation", D->DelaySetActorRotation());
	}

#include WHCORO_END()
}




