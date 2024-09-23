#include "CardGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "CCGToolkit/C++/Widgets/Menu/MainMenuWidget.h"
#include "CCGToolkit/C++/Widgets/Menu/LoadingScreenWidget.h"
#include "CCGToolkit/C++/Widgets/Menu/DeckBuilder/DeckBuilderWidget.h"
#include "CCGToolkit/C++/CardGamePlayerController.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "../Plugins/Online/OnlineSubsystem/Source/Public/Online.h"
#include "../Plugins/Online/OnlineSubsystem/Source/Public/OnlineSubsystem.h"
#include "../Plugins/Online/OnlineSubsystem/Source/Public/OnlineSessionSettings.h"
#include "../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"
#include "../Plugins/Online/OnlineSubsystemUtils/Source/OnlineSubsystemUtils/Public/OnlineSubsystemUtils.h"
#include "Net/UnrealNetwork.h"


UCardGameInstance::UCardGameInstance()
{
	//�󶨻ص�����
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::
		CreateUObject(this, &UCardGameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::
		CreateUObject(this, &UCardGameInstance::OnStartSessionComplete);
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::
		CreateUObject(this, &UCardGameInstance::OnFindSessionsComplete);
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::
		CreateUObject(this, &UCardGameInstance::OnJoinSessionComplete);
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::
		CreateUObject(this, &UCardGameInstance::OnDestroySessionComplete);
}

void UCardGameInstance::Init()
{
	Super::Init();

	Platform_Array = { "Windows", "iOS", "Android", "HTML5", "Mac", "Linux" };
	EnableLAN = true;
	CurrentGameState = EGameState::Startup;
	ChosenCardSet_Enum = ECardSet::BasicSet;
	Arena = EArenaList::Arena_Classic;
	Platform = EPlatform::Windows;
	SelectedCardSet = "Creature Reborn";

	Platform = (EPlatform)(uint8(Platform_Array.Find(UGameplayStatics::GetPlatformName())));

}

bool UCardGameInstance::GetCurrentState(EGameState isState)
{
	if (CurrentGameState == isState)
	{
		return true;
	}
	return false;
}

bool UCardGameInstance::MoveToGameState(EGameState isState)
{
	if (GetCurrentState(isState))
	{
		return true;
	}
	else
	{
		if (UKismetSystemLibrary::IsDedicatedServer(GetWorld()))
		{
			CurrentGameState = isState;
			return false;
		}
		else
		{
			switch (CurrentGameState)
			{
			case EGameState::Startup:
				CurrentGameState = isState;
				break;
			case EGameState::MainMenu:
				MainMenuWidget->RemoveFromParent();
				CurrentGameState = isState;
				break;
			case EGameState::LoadingScreen:
				LoadingScreenWidget->RemoveFromParent();
				CurrentGameState = isState;
				break;
			case EGameState::Playing:
				CurrentGameState = isState;
				break;
			case EGameState::StoreComingSoon:
				CurrentGameState = isState;
				break;
			case EGameState::DeckBuildingMenuComingSoon:
				DeckBuilderWidget->RemoveFromParent();
				CurrentGameState = isState;
				break;
			default:
				break;
			}
		}
		return false;
	}
}

void UCardGameInstance::ShowMainMenu_C()
{
	EGameState GameState = EGameState::Playing;
	if (GetCurrentState(GameState))
	{
		UGameplayStatics::OpenLevel(GWorld, FName("MainMenu"), true, FString("Listen"));
	}
	MoveToGameState(EGameState::MainMenu);
	if (!UKismetSystemLibrary::IsDedicatedServer(GetWorld()))
	{
		MainMenuWidgetClass = LoadClass<UMainMenuWidget>(NULL, TEXT("WidgetBlueprint'/Game/CCGToolkitC++/Blueprints/Widgets/Menu/BP_MainMenuWidget.BP_MainMenuWidget_C'"));
		MainMenuWidget = CreateWidget<UMainMenuWidget>(GetWorld(), MainMenuWidgetClass);
		MainMenuWidget->AddToViewport(5);
	}
	DestroySession();
}

bool UCardGameInstance::GetGameState(EGameState InState, EGameState& CurrentState)
{
	CurrentState = CurrentGameState;
	if (CurrentGameState == InState)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void UCardGameInstance::ShowLoadingScreen_C()
{
	if (MoveToGameState(EGameState::LoadingScreen) == false)
	{
		if (UKismetSystemLibrary::IsDedicatedServer(GetWorld()) == false)
		{
			LoadingScreenWidgetClass = LoadClass<ULoadingScreenWidget>(NULL, TEXT("WidgetBlueprint'/Game/CCGToolkitC++/Blueprints/Widgets/Menu/BP_LoadingScreenWidget.BP_LoadingScreenWidget_C'"));
			LoadingScreenWidget = CreateWidget<ULoadingScreenWidget>(GetWorld(), LoadingScreenWidgetClass);
			LoadingScreenWidget->AddToViewport();
		}
	}
}

void UCardGameInstance::HostGame(EArenaList SelectedArena)
{
	ShowLoadingScreen_C();
	GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, this, &UCardGameInstance::HostSession, 2.f, false);
}

void UCardGameInstance::JoinGameSession()
{
	ShowLoadingScreen_C();
	ClientSession();
}

void UCardGameInstance::AssignPlayerController(APlayerController* InController)
{
	PlayerController = InController;


	//��ȡOnlineSub
	//��ȡ��ʽһ : Online::GetSubsystem(GetWorld(), NAME_None), �Ƽ���ʽ
	//��ȡ��ʽ�� : ʹ��IOnlineSubsystem::Get(), ֱ�ӻ�ȡ����createSession����joinSession��ͻ���û����ת����
	OnlineSub = Online::GetSubsystem(PlayerController->GetWorld(), NAME_None);

	//��ȡUserID
	//UGameplayStatics::GetGameInstance(GetWorld())->GetLocalPlayers()[0]->GetPreferredUniqueNetId()
	if (GetLocalPlayers().Num() == 0)
		UKismetSystemLibrary::PrintString(this, "No LocalPlayer Exis, Can't Get UserID");
	else
		UserID = (*GetLocalPlayers()[0]->GetPreferredUniqueNetId()).AsShared();

}

void UCardGameInstance::HostSession()
{
	if (OnlineSub)
	{
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();
		if (Session.IsValid())
		{
			//�Ự����
			FOnlineSessionSettings Settings;
			//������
			Settings.NumPublicConnections = 10;
			Settings.bShouldAdvertise = true;
			Settings.bAllowJoinInProgress = true;
			//ʹ�þ�����
			Settings.bIsLANMatch = true;
			Settings.bUsesPresence = true;
			Settings.bAllowJoinViaPresence = true;
			//��ί��
			OnCreateSessionCompleteDelegateHandle = Session
				->AddOnCreateSessionCompleteDelegate_Handle
				(OnCreateSessionCompleteDelegate);
			//�����Ự
			Session->CreateSession(*UserID, NAME_GameSession, Settings);
		}
	}
}

void UCardGameInstance::ClientSession()
{
	if (OnlineSub)
	{
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();
		if (Session.IsValid())
		{
			//ʵ�����������ָ�벢���趨����
			SearchObject = MakeShareable(new FOnlineSessionSearch);
			//���ؽ����
			SearchObject->MaxSearchResults = 10;
			//�Ƿ��Ǿ�����������IsLAN
			SearchObject->bIsLanQuery = true;
			SearchObject->QuerySettings.Set(SEARCH_PRESENCE, true,
				EOnlineComparisonOp::Equals);
			//��Ѱ�һỰί��
			OnFindSessionsCompleteDelegateHandle = Session->
				AddOnFindSessionsCompleteDelegate_Handle
				(OnFindSessionsCompleteDelegate);
			//���лỰѰ��
			Session->FindSessions(*UserID, SearchObject.ToSharedRef());
		}
	}
}

void UCardGameInstance::DestroySession()
{
	if (OnlineSub)
	{
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();
		if (Session.IsValid())
		{
			//�����ٻỰί��
			OnDestroySessionCompleteDelegateHandle = Session->
				AddOnDestroySessionCompleteDelegate_Handle
				(OnDestroySessionCompleteDelegate);
			//ִ�����ٻỰ
			Session->DestroySession(NAME_GameSession);
		}
	}
}

bool UCardGameInstance::GetCurrentPlatform_C(EPlatform& ReturnPlatform)
{
	ReturnPlatform = Platform;
	return Platform == EPlatform::iOS || Platform == EPlatform::Android;
}

void UCardGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (OnlineSub)
	{
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();
		if (Session.IsValid())
		{
			//��󴴽��Ự��ɻص�����
			Session->
				ClearOnCreateSessionCompleteDelegate_Handle
				(OnCreateSessionCompleteDelegateHandle);
			//�жϴ����Ự�Ƿ�ɹ�
			if (bWasSuccessful)
			{
				UE_LOG(CardGameInstanceLog, Warning, TEXT("CreatSession Succeed"));

				//�󶨿����Ựί��
				OnStartSessionCompleteDelegateHandle = Session->
					AddOnStartSessionCompleteDelegate_Handle
					(OnStartSessionCompleteDelegate);
				Session->StartSession(NAME_GameSession);
			}
			else
				UE_LOG(CardGameInstanceLog, Warning, TEXT("CreateSession Failed"));
		}
	}
}

void UCardGameInstance::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(CardGameInstanceLog, Warning, TEXT("StartSession Start"));
	if (OnlineSub)
	{
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();
		if (Session.IsValid())
		{
			//ע�������Ựί�а�
			Session->ClearOnStartSessionCompleteDelegate_Handle
			(OnStartSessionCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				UE_LOG(CardGameInstanceLog, Warning, TEXT("StartSession Succeed"));
				//�������ת����
				UGameplayStatics::OpenLevel(PlayerController->GetWorld(),
					FName("Arena_Classic"), true, FString("Listen"));
			}
			else
			{
				UE_LOG(CardGameInstanceLog, Warning, TEXT("StartSession Failed"));
				ShowMainMenu_C();
			}
		}
	}
}

void UCardGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (OnlineSub)
	{
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();
		if (Session.IsValid())
		{
			//ȡ��Ѱ�һỰί�а�
			Session->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
			//���Ѱ�һỰ�ɹ�
			if (bWasSuccessful)
			{
				//����ռ��Ľ�����ڲ��Ҵ���1
				if (SearchObject.IsValid() && SearchObject->SearchResults.Num() > 0)
				{
					UE_LOG(CardGameInstanceLog, Warning, TEXT("Find Sessions Succeed"))
						//�󶨼���Sessionί��
						OnJoinSessionCompleteDelegateHandle = Session->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
					//ִ�м���Session
					Session->JoinSession(*UserID, NAME_GameSession, SearchObject->SearchResults[0]);
				}
				else
					UE_LOG(CardGameInstanceLog, Warning, TEXT("Find Sessions Succeed But Num == 0"));
			}
			else
				UE_LOG(CardGameInstanceLog, Warning, TEXT("Find Sessions Failed"));
		}
	}
}

void UCardGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (OnlineSub)
	{
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();
		if (Session.IsValid())
		{
			//ȡ������Ựί�а�
			Session->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
			//�������ɹ�
			if (Result == EOnJoinSessionCompleteResult::Success)
			{
				//������ҵ��µ�ͼ
				FString ConnectString;
				if (Session->GetResolvedConnectString(NAME_GameSession, ConnectString))
				{
					UE_LOG(CardGameInstanceLog, Warning, TEXT("Join Sessions Succeed"));
					//�ͻ����л����������Ĺؿ�
					PlayerController->ClientTravel(ConnectString, TRAVEL_Absolute);
				}
			}
			else
			{
				UE_LOG(CardGameInstanceLog, Warning, TEXT("Join Sessions Failed"));
				ShowMainMenu_C();
			}
		}
	}
}

void UCardGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (OnlineSub)
	{
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();
		if (Session.IsValid())
		{
			//ע�����ٻỰί��
			Session->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
			//�����߼�...........
		}
	}
}
