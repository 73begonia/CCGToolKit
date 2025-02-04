// Fill out your copyright notice in the Description page of Project Settings.


#include "UIState.h"

void UUIState::EnterState(EStackAction StackAction)
{
	ReceiveEnterState(StackAction);
}

void UUIState::ExitState(EStackAction StackAction)
{
	ReceiveExitState(StackAction);
}

void UUIState::UpdateState(float DeltaTime)
{
	ReceiveUpdateState(DeltaTime);
}

void UUIState::ReceiveUpdateState_Implementation(float DeltaTime)
{

}

void UUIState::ReceiveExitState_Implementation(EStackAction StackAction)
{
	switch (StackAction)
	{
	case Push:
		{
			SetVisibility(ESlateVisibility::Hidden);
			break;
		}
	case Pop:
		{
			RemoveFromParent();
			break;
		}
	default:
		break;
	}
}

void UUIState::ReceiveEnterState_Implementation(EStackAction StackAction)
{
	switch (StackAction)
	{
	case Push:
		{
			AddToViewport();
			break;
		}	
	case Pop:
		{
			SetVisibility(ESlateVisibility::Visible);
			break;
		}
	default:
		break;
	}
}
