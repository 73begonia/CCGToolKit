// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects_FunctionLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "CCGToolkit/C++/Enums/CardGameEnums.h"
#include "../../CardGame/Structs/Structs.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

void UEffects_FunctionLibrary::GetCardEffectsForState(ECardEffects Effects, FSCard CardStruct, int32 WildcardIndex, UParticleSystem*& Particle, USoundCue*& SoundCue)
{
	switch (Effects)
	{
    case ECardEffects::OnCardPlaced:
        Particle = CardStruct.Placement_Settings.Entry_Particle;
        break;
    case ECardEffects::OnAttack:
        Particle = CardStruct.Attack.Attack_Particle;
        break;
    case ECardEffects::DecreasedHealth:
        break;
    case ECardEffects::IncreasedHealth:
        break;
    case ECardEffects::OnAbilityUsed:
        Particle = CardStruct.Abilities[WildcardIndex].Particle;
        break;
    case ECardEffects::OnDeath:
        Particle = CardStruct.Health.Dealth_Particle;
        break;
    default:
        break;
	}

    switch (Effects)
    {
    case ECardEffects::OnCardPlaced:
        SoundCue = CardStruct.Placement_Settings.Entry_SoundCue;
        break;
    case ECardEffects::OnAttack:
        SoundCue = CardStruct.Attack.Attack_SoundCue;
        break;
    case ECardEffects::DecreasedHealth:
        break;
    case ECardEffects::IncreasedHealth:
        break;
    case ECardEffects::OnAbilityUsed:
        SoundCue = CardStruct.Abilities[WildcardIndex].SoundCue;
        break;
    case ECardEffects::OnDeath:
        SoundCue = CardStruct.Health.Sound_Cue_Death;
        break;
    default:
        break;
    }
}

void UEffects_FunctionLibrary::GetPlayerEffectsforState(EBoardPlayerEffects Effect, UParticleSystem*& Particle, USoundCue*& SoundCue)
{
    switch (Effect)
    {
    case EBoardPlayerEffects::DecreasedHealth:
        break;
    case EBoardPlayerEffects::IncreasedHealth:
        break;
    case EBoardPlayerEffects::Death:
        break;
    default:
        break;
    }

    switch (Effect)
    {
    case EBoardPlayerEffects::DecreasedHealth:
        break;
    case EBoardPlayerEffects::IncreasedHealth:
        break;
    case EBoardPlayerEffects::Death:
        break;
    default:
        break;
    }
}

void UEffects_FunctionLibrary::SpawnParticleAndSoundEffects(UParticleSystem* ParticleToSpawn, FVector Location, AActor* ORActorForLocation, FVector Scale, bool bAutoDestroy, USoundCue* SoundCueToSpwn, float Volume)
{
    FVector TheLocation;
	if (ParticleToSpawn)
	{
        if (ORActorForLocation)
        {
            TheLocation = ORActorForLocation->GetActorLocation();
        }
        else
        {
            TheLocation = Location;
        }
		/*UParticleSystemComponent* ReturnValue = UGameplayStatics::SpawnEmitterAtLocation(GWorld->GetWorld(), ParticleToSpawn, TheLocation, FRotator(0, 0, 0), FVector(1, 1, 1), bAutoDestroy, EPSCPoolMethod::None, true);
		ReturnValue->SetWorldScale3D(Scale);
		if (SoundCueToSpwn)
		{
			UGameplayStatics::SpawnSound2D(GWorld->GetWorld(), SoundCueToSpwn, Volume, 1.0, 0.0, nullptr, false, true);
		}*/
	}
}
