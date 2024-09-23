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
	//绑定回调函数
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


	//获取OnlineSub
	//获取方式一 : Online::GetSubsystem(GetWorld(), NAME_None), 推荐方式
	//获取方式二 : 使用IOnlineSubsystem::Get(), 直接获取可以createSession但是joinSession后客户端没有跳转场景
	OnlineSub = Online::GetSubsystem(PlayerController->GetWorld(), NAME_None);

	//获取UserID
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
			//会话设置
			FOnlineSessionSettings Settings;
			//连接数
			Settings.NumPublicConnections = 10;
			Settings.bShouldAdvertise = true;
			Settings.bAllowJoinInProgress = true;
			//使用局域网
			Settings.bIsLANMatch = true;
			Settings.bUsesPresence = true;
			Settings.bAllowJoinViaPresence = true;
			//绑定委托
			OnCreateSessionCompleteDelegateHandle = Session
				->AddOnCreateSessionCompleteDelegate_Handle
				(OnCreateSessionCompleteDelegate);
			//创建会话
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
			//实例化搜索结果指针并且设定参数
			SearchObject = MakeShareable(new FOnlineSessionSearch);
			//返回结果数
			SearchObject->MaxSearchResults = 10;
			//是否是局域网，就是IsLAN
			SearchObject->bIsLanQuery = true;
			SearchObject->QuerySettings.Set(SEARCH_PRESENCE, true,
				EOnlineComparisonOp::Equals);
			//绑定寻找会话委托
			OnFindSessionsCompleteDelegateHandle = Session->
				AddOnFindSessionsCompleteDelegate_Handle
				(OnFindSessionsCompleteDelegate);
			//进行会话寻找
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
			//绑定销毁会话委托
			OnDestroySessionCompleteDelegateHandle = Session->
				AddOnDestroySessionCompleteDelegate_Handle
				(OnDestroySessionCompleteDelegate);
			//执行销毁会话
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
			//解绑创建会话完成回调函数
			Session->
				ClearOnCreateSessionCompleteDelegate_Handle
				(OnCreateSessionCompleteDelegateHandle);
			//判断创建会话是否成功
			if (bWasSuccessful)
			{
				UE_LOG(CardGameInstanceLog, Warning, TEXT("CreatSession Succeed"));

				//绑定开启会话委托
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
			//注销开启会话委托绑定
			Session->ClearOnStartSessionCompleteDelegate_Handle
			(OnStartSessionCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				UE_LOG(CardGameInstanceLog, Warning, TEXT("StartSession Succeed"));
				//服务端跳转场景
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
			//取消寻找会话委托绑定
			Session->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
			//如果寻找会话成功
			if (bWasSuccessful)
			{
				//如果收集的结果存在并且大于1
				if (SearchObject.IsValid() && SearchObject->SearchResults.Num() > 0)
				{
					UE_LOG(CardGameInstanceLog, Warning, TEXT("Find Sessions Succeed"))
						//绑定加入Session委托
						OnJoinSessionCompleteDelegateHandle = Session->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
					//执行加入Session
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
			//取消加入会话委托绑定
			Session->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
			//如果加入成功
			if (Result == EOnJoinSessionCompleteResult::Success)
			{
				//传送玩家到新地图
				FString ConnectString;
				if (Session->GetResolvedConnectString(NAME_GameSession, ConnectString))
				{
					UE_LOG(CardGameInstanceLog, Warning, TEXT("Join Sessions Succeed"));
					//客户端切换到服务器的关卡
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
			//注销销毁会话委托
			Session->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
			//其他逻辑...........
		}
	}
}
