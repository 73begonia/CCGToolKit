// Fill out your copyright notice in the Description page of Project Settings.
#if PLATFORM_WINDOWS
#pragma optimize("",off) 
#endif

#include "CardGameMode.h"
#include "CardGameState.h"
#include "CardGameInstance.h"
#include "TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "CCGToolkit/C++/GamePlay/Card3D.h"
#include "CCGToolkit/C++/CardGamePlayerController.h"
#include "CCGToolkit/C++/CardGamePlayerState.h"
#include "CCGToolkit/CardGameAI/AIC++/CardGameAIController.h"
#include "CCGToolkit/CardGameAI/AIC++/CardGameAI_Pawn.h"
#include "CCGToolkit/C++/FunctionLibraries/Controller_FunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "Runtime/AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "FunctionLibraries/Math_Library.h"
#include "Kismet/KismetMathLibrary.h"

ACardGameMode::ACardGameMode()
{
	bReplicates = true;

	bSkipStartTimer = true;
	bSpectator = false;
	GameSeconds = 1;
	CountdownTimer = 4;
	bGameActive = false;
	PlayerStartingHealth = 30;
}

void ACardGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACardGameMode, GameControllersArray);
	DOREPLIFETIME(ACardGameMode, PlayerStateArray);
	DOREPLIFETIME(ACardGameMode, PlayerStartingHealth);
}

void ACardGameMode::BeginPlay()
{
	Super::BeginPlay();

	StartCoroutine("CoroBeginPlay", CoroBeginPlay());
}

WHCoroTask* ACardGameMode::CoroBeginPlay()
{
	WHCORO_PARAM(ACardGameMode)
#include WHCORO_BEGIN()

#include WHYIELD_READY()
	WHYIELD_RETURN_SECOND(0.2f);
	D->GameStateRef = Cast<ACardGameState>(GWorld->GetGameState());

#include WHCORO_END()
}

void ACardGameMode::SetGameActiveAndOnGameStart()
{
	bGameActive = true;

	GameStateRef->OnGameStart_C();
}

void ACardGameMode::PostLogin(APlayerController* NewPlayer)
{
	ACardGamePlayerController* CardGamePlayerController;
	ACardGamePlayerState* CardGamePlayerState = nullptr;

	if (HasAuthority())
	{
		if (UGameplayStatics::GetCurrentLevelName(GetWorld(), true) != "MainMenu")
		{
			if (GameControllersArray.Num() >= MaxNumOfPlayers)
			{
				SetBoardPlayerReference();
				CheckGamePreconditions_C();
			}
			else
			{
				if (bSpectator)
				{
					SetBoardPlayerReference();
					CheckGamePreconditions_C();
				}
				else
				{
					AActor* PlayerState = Cast<AActor>(NewPlayer->PlayerState);
					AController* Controller = AddPlayerToArray(PlayerState, NewPlayer);
					SetCardGamePlayerID(NewPlayer->PlayerState, Controller);
					SetBoardPlayerReference();
					SetBoardPlayerReferences(Controller);
					UController_FunctionLibrary::GetPlayerControllerReference_ServerOnly(NewPlayer, CardGamePlayerController, CardGamePlayerState);
					CardGamePlayerController->ClientPostLogin_C();
					CardGamePlayerController->Server_SetupDeck_C();
					CheckGamePreconditions_C();
				}
			}
		}
	}
	UE_LOG(CardGameModeLog, Warning, TEXT("CardGameModePostLoginEnd!"));
}

AController* ACardGameMode::AddPlayerToArray(AActor* PlayerState, AController* PlayerController)
{
	PlayerStateArray.AddUnique(PlayerState);
	GameControllersArray.AddUnique(PlayerController);
	if (GameStateRef)
	{
		GameStateRef->PlayerState_Array = PlayerStateArray;
	}
	return PlayerController;
}

void ACardGameMode::SetCardGamePlayerID(UObject* PlayerState, AController* Controller)
{
	ACardGamePlayerController* CardGamePlayerController = Cast<ACardGamePlayerController>(Controller);
	ACardGamePlayerState* CardGamePlayerState = Cast<ACardGamePlayerState>(PlayerState);
	//UController_FunctionLibrary::GetPlayerControllerReference_ServerOnly(Controller, CardGamePlayerController, CardGamePlayerState);
	CardGamePlayerState->CardGamePlayerId = GameControllersArray.Num();
	CardGamePlayerState->OwningPlayerController = CardGamePlayerController;
}

void ACardGameMode::Logout(AController* Exiting)
{
	RemovePlayerFromGame(Exiting);
}

void ACardGameMode::RemovePlayerFromGame(AController* Controller)
{
	GameControllersArray.Remove(Controller);
}

void ACardGameMode::SetBoardPlayerReference()
{
	TArray<AActor*> OutActors;
	UClass* BoardPlayerClass = LoadClass<ABoardPlayer>(NULL, TEXT("Blueprint'/Game/CCGToolkitC++/Blueprints/GamePlay/BoardPlayer.BoardPlayer_C'"));
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), BoardPlayerClass, OutActors);
	for (int32 i = 0; i < OutActors.Num(); i++)
	{
		BoardPlayersArray.Add(Cast<ABoardPlayer>(OutActors[i]));
	}
}

void ACardGameMode::SetBoardPlayerReferences(AController* Controller)
{
	ACardGamePlayerController* CardGamePlayerController = Cast<ACardGamePlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	ACardGamePlayerState* CardGamePlayerState = Cast<ACardGamePlayerState>(UGameplayStatics::GetGameState(this));
	ACardGameAIController* CardGameAIController = nullptr;
	ACardGameAI_Pawn* CardGameAI_Pawn = nullptr;

	if (UController_FunctionLibrary::GetPlayerControllerReference_ServerOnly(Controller, CardGamePlayerController, CardGamePlayerState))
	{
		CardGamePlayerController->BoardPlayer_Ref = BoardPlayersArray[GameControllersArray.Find(Controller)];
		BoardPlayersArray[GameControllersArray.Find(Controller)]->OwningController = CardGamePlayerController;
	}
	else
	{
		if (UController_FunctionLibrary::GetAIControllerReference_ServerOnly(Controller, CardGameAIController, CardGameAI_Pawn))
		{
			CardGameAIController->BoardPlayer_Ref = BoardPlayersArray[GameControllersArray.Find(Controller)];
			BoardPlayersArray[GameControllersArray.Find(Controller)]->OwningController = CardGameAIController;
		}
	}
}

void ACardGameMode::CreateCardGameAIOpponent()
{
	//UClass* PawnClass = LoadClass<ACardGameAI_Pawn>(NULL, TEXT("Blueprint'/Game/CCGToolkit/CardGameAI/AIBlueprints/CardGameAI_Pawn.CardGameAI_Pawn_C'"));
	TArray<AActor*> OutActors;
	if (GameControllersArray.Num()<2)
	{
		if (bSpectator)
		{
			for (int32 i = 1; i<=2; i++)
			{
				UClass* CardGameAIPawnClass = LoadClass<ACardGameAI_Pawn>(NULL, TEXT("Blueprint'/Game/CCGToolkit/CardGameAI/AIBlueprints/CardGameAI_Pawn_BP.CardGameAI_Pawn_BP_C'"));
				APawn* NewPawn = UAIBlueprintHelperLibrary::SpawnAIFromClass(this, CardGameAIPawnClass, nullptr, BoardPlayersArray[GameControllersArray.Num()]->GetActorLocation(), FRotator(0.0,0.0,0.0), true);
				AddPlayerToArray(NewPawn, NewPawn->GetController());
				SetBoardPlayerReferences(NewPawn->GetController());
				ACardGameAI_Pawn* CardGameAIPawn = Cast<ACardGameAI_Pawn>(NewPawn);
				CardGameAIPawn->PlayerID = GameControllersArray.Num();
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), CardGameAIPawnClass, OutActors);
				CardGameAIPawn->CardGameAIID = OutActors.Num();
				UKismetSystemLibrary::PrintString(this, "AI Opponent Spawned!");
			}
		}
		else
		{
			FTransform Transform;
			Transform.SetLocation(FVector(0, 0, 0));
			Transform.SetRotation(FQuat(FRotator(0, 0, 0)));
			Transform.SetScale3D(FVector(1, 1, 1));
			FActorSpawnParameters ActorSpawnParameters;
			ActorSpawnParameters.Owner = this->GetOwner();
			UClass* CardGameAIControllerClass = LoadClass<ACardGameAIController>(NULL, TEXT("Blueprint'/Game/CCGToolkit/CardGameAI/AIBlueprints/CardGameAIController_BP.CardGameAIController_BP_C'"));
			ACardGameAIController* CardGameAIController = Cast<ACardGameAIController>(GetWorld()->SpawnActor<ACardGameAIController>(CardGameAIControllerClass, Transform, ActorSpawnParameters));

			UClass* CardGameAIPawnClass = LoadClass<ACardGameAI_Pawn>(NULL, TEXT("Blueprint'/Game/CCGToolkit/CardGameAI/AIBlueprints/CardGameAI_Pawn_BP.CardGameAI_Pawn_BP_C'"));
			APawn* NewPawn = UAIBlueprintHelperLibrary::SpawnAIFromClass(this, CardGameAIPawnClass, nullptr, BoardPlayersArray[GameControllersArray.Num()]->GetActorLocation(), FRotator(0.0, 0.0, 0.0), true);	

			CardGameAIController->SetPawn(NewPawn);
			NewPawn->Controller = CardGameAIController;

			AddPlayerToArray(NewPawn, NewPawn->GetController());
			SetBoardPlayerReferences(NewPawn->GetController());
			ACardGameAI_Pawn* CardGameAIPawn = Cast<ACardGameAI_Pawn>(NewPawn);
			CardGameAIPawn->PlayerID = GameControllersArray.Num();
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), CardGameAIPawnClass, OutActors);
			CardGameAIPawn->CardGameAIID = OutActors.Num();

			CardGameAIController->OnPossess(NewPawn);
			NewPawn->PossessedBy(CardGameAIController);
			
			UKismetSystemLibrary::PrintString(this, "AI Opponent Spawned!"); 
		}
	}
}

bool ACardGameMode::CheckIsPlayerActiveState(int32 ControllerID)
{
	FControllersState ControllersState = UController_FunctionLibrary::GetControllersStateProfile_ServerOnly(ControllerID);
	if (ControllersState.Health > 0)
	{
		return (ControllersState.NumCardsInHand > 0 || ControllersState.CardsInDeck > 0 || ControllersState.ActiveCards > 0);
	}
	else if (ControllersState.Health <= 0)
	{
		return false;
	}
	else
		return false;
}

void ACardGameMode::CollectGameResults(EEndGameResults& Player1Result, EEndGameResults& Player2Result)
{
	if (HasAuthority())
	{
		if (CheckIsPlayerActiveState(1) == false && CheckIsPlayerActiveState(2) == true)
		{
			Player1Result = EEndGameResults::Defeat;
			Player2Result = EEndGameResults::Victory;
		}
		if (CheckIsPlayerActiveState(1) == false && CheckIsPlayerActiveState(2) == false)
		{
			Player1Result = EEndGameResults::Draw;
			Player2Result = EEndGameResults::Draw;
		}
		if (CheckIsPlayerActiveState(1) == true && CheckIsPlayerActiveState(2) == false)
		{
			Player1Result = EEndGameResults::Victory;
			Player2Result = EEndGameResults::Defeat;
		}
	}
}

void ACardGameMode::GetPlayerControllers(AController* Player1, AController* Player2)
{
	if (GameControllersArray.Num() > 1)
	{
		Player1 = GameControllersArray[0];
		Player2 = GameControllersArray[1];
	}
	else
	{
		Player1 = GameControllersArray[0];
	}
}

int32 ACardGameMode::CalculateManaForTurn(int32 PlayerTurn)
{
	return UKismetMathLibrary::Clamp(PlayerTurn + ManaMin, ManaMin, ManaMax);
}

int32 ACardGameMode::GetTurnMana(AController* Controller)
{
	int32 ID = UController_FunctionLibrary::GetControllerID_ServerOnly(Controller);
	FControllersState ControllersState = UController_FunctionLibrary::GetControllersStateProfile_ServerOnly(ID);
	return CalculateManaForTurn(ControllersState.PlayerTurn);
}

void ACardGameMode::CheckGamePreconditions_C_Implementation()
{
	if (HasAuthority())
	{
		if (GameControllersArray.Num() == 2)
		{
			UKismetSystemLibrary::PrintString(this, "All players are in!");
			if (bSkipStartTimer)
			{
				GetWorldTimerManager().SetTimer(GameStartTimerHandle, this, &ACardGameMode::SetGameActiveAndOnGameStart, 3.0f, false);
			}
			else
			{
				GetWorldTimerManager().SetTimer(CountDownTimerHandle, this, &ACardGameMode::GameStartCountdown_C, GameSeconds, true);
			}
		}
		else
		{
			if (bSingleClientEnabled)
			{
				UKismetSystemLibrary::PrintString(this, "Standalone Client ENABLED");
				ForceSpawnAIOpponent_C();
			}
			else
			{
				UKismetSystemLibrary::PrintString(this, "Waiting on one more player...");
			}
		}
	}
}

void ACardGameMode::GameStartCountdown_C()
{
	--CountdownTimer;
	for (int i = 0; i < GameControllersArray.Num(); i++)
	{
		PlayerControllerRef = Cast<ACardGamePlayerController>(GameControllersArray[i]);
		PlayerControllerRef->SetCountdownTimer_C(CountdownTimer);
	}
	if (CountdownTimer < 1)
	{
		GetWorldTimerManager().ClearTimer(CountDownTimerHandle);
		SetGameActiveAndOnGameStart();
	}
}

void ACardGameMode::ForceSpawnAIOpponent_C_Implementation()
{
	CreateCardGameAIOpponent();
	UKismetSystemLibrary::PrintString(this, "All players are in!");
	if (bSkipStartTimer)
	{
		GetWorldTimerManager().SetTimer(GameStartTimerHandle, this, &ACardGameMode::SetGameActiveAndOnGameStart, 3.0f, false);
	}
	else
	{
		GetWorldTimerManager().SetTimer(CountDownTimerHandle, this, &ACardGameMode::GameStartCountdown_C, GameSeconds, true);
	}
}

void ACardGameMode::Server_CheckPlayerState_C_Implementation()
{
	if (!(CheckIsPlayerActiveState(1) && CheckIsPlayerActiveState(2)))
	{
		Server_EndGame_C();
	}
}

void ACardGameMode::Server_EndGame_C_Implementation()
{
	bGameActive = false;
	EEndGameResults Player1Result = EEndGameResults::Defeat;
	EEndGameResults Player2Result = EEndGameResults::Defeat;
	CollectGameResults(Player1Result, Player2Result);
	GameStateRef->Server_NotifyEndGameState_C(Player1Result, Player2Result);
}
#if PLATFORM_WINDOWS
#pragma optimize("",on) 
#endif