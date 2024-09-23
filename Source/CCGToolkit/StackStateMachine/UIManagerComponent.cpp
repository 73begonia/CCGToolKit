// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManagerComponent.h"
#include "Kismet/GameplayStatics.h"

void UUIManagerComponent::InitUIStates_Implementation()
{
	auto PC = UGameplayStatics::GetPlayerController(GetOwner(), 0);
	if (!PC->IsLocalPlayerController())
	{
		UE_LOG(LogUIManager, Display, TEXT("%s Is not local playercontroller, skipping init uistates."), *PC->GetName());
		return;
	}
	for (TTuple<FName, TSubclassOf<UUIState>> UI : UIStates)
	{
		auto Widget = CreateWidget<UUIState>(PC, UI.Value.Get(), UI.Key);
		
		UE_LOG(LogUIManager, Display, TEXT("初始化UI:%s"), *UI.Key.ToString());
		UIInstances.Add(UI.Key, Widget);
	}

	UE_LOG(LogUIManager, Display, TEXT("Initialized UIManager."));
}

UUIState* UUIManagerComponent::GetUI_Implementation(FName UIName)
{
	UUIState** UI = UIInstances.Find(UIName);
	if (UI != nullptr)
	{
		return *UI;
	}

	UE_LOG(LogUIManager, Error, TEXT("UIManager中不存在:%s"), *UIName.ToString());

	return nullptr;
}

void UUIManagerComponent::PushUIState(FName UIName)
{
	auto UI = GetUI(UIName);
	if (UI == nullptr)
	{
		return;
	}
	TScriptInterface<IStackState_Interface> Interface(UI);

	PushState(UI);
}

void UUIManagerComponent::GetCurrentUIState(TSubclassOf<UUIState> StateClass, TEnumAsByte<EUIStateCastResult>& IsValid, UUIState*& OutState)
{
	if (StateCount() <= 0)
	{
		IsValid = EUIStateCastResult::Failed;
		return;
	}

	UUIState* State = Cast<UUIState>(GetCurrentState().GetObject());
	IsValid = State != nullptr && State->GetClass()->IsChildOf(StateClass) ? Success : Failed;
	if (IsValid == Success)
	{
		OutState = State;
	}
}

void UUIManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	InitUIStates();
}

