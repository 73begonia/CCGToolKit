// Fill out your copyright notice in the Description page of Project Settings.


#include "BoardPlayer.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "../FunctionLibraries/Effects_FunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "AutoRotateToPlayer.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ABoardPlayer::ABoardPlayer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlayerIndex = 1;
	PlayerName = FText::FromString("Player");
	PlayerHealth = 0;
	Temp_HealthIconRelativeScale = FVector(0,0,0);
	bScaleingHealthIcon = false;
	Temp_HealthTextColour = FColor(0,0,0);
	HealthDecreaseColour = FColor(1,0.88,0);
	HealthIncreaseColour = FVector(1,1,1);

	DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));
	SetRootComponent(DefaultSceneComponent);

	Cube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));
	Cube->SetupAttachment(RootComponent);

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(RootComponent);

	PlayerText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PlayerText"));
	PlayerText->SetupAttachment(RootComponent);

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(RootComponent);

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	Scene->SetupAttachment(RootComponent);

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(Scene);

	Heart_Icon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Heart_Icon"));
	Heart_Icon->SetupAttachment(RootComponent);

	HealthText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HealthText"));
	HealthText->SetupAttachment(Heart_Icon);

	PlayerTile = CreateDefaultSubobject<UChildActorComponent>(TEXT("PlayerTile"));
	PlayerTile->SetupAttachment(RootComponent);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);

	AutoRotateToPlayer = CreateDefaultSubobject<UAutoRotateToPlayer>(TEXT("PlayerRotationActor"));
}

void ABoardPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABoardPlayer, PlayerIndex);
	DOREPLIFETIME(ABoardPlayer, PlayerHealth);
	DOREPLIFETIME(ABoardPlayer, PlayerName);
}

// Called when the game starts or when spawned
void ABoardPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABoardPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABoardPlayer::ScaleHealthIcon_Implementation(bool Decrease)
{
	
}

void ABoardPlayer::UpdatePlayerStats_Implementation()
{
	if (!UKismetSystemLibrary::IsDedicatedServer(this))
	{
		HealthText->Text = FText::FromString(FString::FromInt(PlayerHealth));
	}
}

void ABoardPlayer::OnRep_PlayerHealth_C()
{
	UpdatePlayerStats_Implementation();
}

void ABoardPlayer::Update_Health_C_Implementation(int32 Health)
{
	if (Health < PlayerHealth)
	{
		Multicast_BoardPlayerEffect_C(EBoardPlayerEffects::DecreasedHealth);
	}
	if (Health > PlayerHealth)
	{
		Multicast_BoardPlayerEffect_C(EBoardPlayerEffects::IncreasedHealth);
	}

	PlayerHealth = Health;
	OnRep_PlayerHealth_C();

	if (PlayerHealth <= 0)
	{
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FName(TEXT("Multicast_BoardPlayerEffect_C")), EBoardPlayerEffects::Death);
		GetWorldTimerManager().SetTimer(MulticastBoardDelayTimerHandle, TimerDel, 0.2f, false);
	}
}

void ABoardPlayer::Multicast_BoardPlayerEffect_C_Implementation(EBoardPlayerEffects Effect)
{	
	UParticleSystem* Particle;
	USoundCue* SoundCue;
	FColor ReturnColorStruct;
	UEffects_FunctionLibrary::GetPlayerEffectsforState(Effect,Particle, SoundCue);
	UEffects_FunctionLibrary::SpawnParticleAndSoundEffects(Particle,FVector(0,0,0),this,FVector(1,1,1),true,SoundCue,1.0);
	
	ScaleHealthIcon(Effect == EBoardPlayerEffects::DecreasedHealth);
}


