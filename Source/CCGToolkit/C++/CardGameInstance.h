// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CCGToolkit/C++/Enums/CardGameEnums.h"
#include "../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"
#include "Delegates/IDelegateInstance.h"
#include "CardGameInstance.generated.h"


DECLARE_LOG_CATEGORY_CLASS(CardGameInstanceLog, Log, All);

class IOnlineSubsystem;
class APlayerController;
class UMainMenuWidget;
class ULoadingScreenWidget;
class UDeckBuilderWidget;
/**
 *
 */


UCLASS()
class CCGTOOLKIT_API UCardGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UCardGameInstance();

	void Init() override;

	bool GetCurrentState(EGameState isState);

	bool MoveToGameState(EGameState isState);

	UFUNCTION(BlueprintCallable)
	void ShowMainMenu_C();

	bool GetGameState(EGameState InState, EGameState& CurrentState);

	UFUNCTION(BlueprintCallable)
	void ShowLoadingScreen_C();

	UFUNCTION(BlueprintCallable)
	void HostGame(EArenaList SelectedArena);

	UFUNCTION(BlueprintCallable)
	void JoinGameSession();

	void AssignPlayerController(APlayerController* InController);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetCurrentPlatform_C(EPlatform& ReturnPlatform);

	//创建会话
	void HostSession();

	//加入会话
	void ClientSession();

	//摧毁会话
	void DestroySession();

protected:

	//当创建会话结束后，调用这个函数
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	//当开始会话结束后，调用这个函数
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

	//加入服务器回调函数
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	//销毁会话回调函数
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);


public:

	APlayerController* PlayerController;

	FTimerHandle DelayTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool EnableLAN;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGameState CurrentGameState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardSet ChosenCardSet_Enum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EArenaList Arena;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPlatform Platform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SelectedCardSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Platform_Array;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Deck;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAIOpponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<ULoadingScreenWidget> LoadingScreenWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UDeckBuilderWidget> DeckBuilderWidgetClass;

	UMainMenuWidget* MainMenuWidget;

	ULoadingScreenWidget* LoadingScreenWidget;

	UDeckBuilderWidget* DeckBuilderWidget;

protected:
	//开启服务器委托
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	//开启服务器委托句柄
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;

	//加入服务器委托
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	//加入服务器委托句柄
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	//销毁会话委托与句柄
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	FDelegateHandle OnDestroySessionCompleteDelegateHandle;


	IOnlineSubsystem* OnlineSub;

	TSharedPtr<const FUniqueNetId> UserID;

	//保存寻找到的Sessions
	TSharedPtr<FOnlineSessionSearch> SearchObject;

};