// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "../Enums/CardGameEnums.h"
#include "../../CardGame/Structs/Structs.h"
#include "Effects_FunctionLibrary.generated.h"


class UParticleSystem;
class USoundCue;
/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UEffects_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	static void GetCardEffectsForState(ECardEffects Effects, FSCard CardStruct, int32 WildcardIndex, UParticleSystem*& Particle, USoundCue*& SoundCue);

	static void GetPlayerEffectsforState(EBoardPlayerEffects Effect, UParticleSystem*& Particle, USoundCue*& SoundCue);

	static void SpawnParticleAndSoundEffects(UParticleSystem* ParticleToSpawn, FVector Location, AActor* ORActorForLocation, FVector Scale, bool bAutoDestroy, USoundCue* SoundCueToSpwn, float Volume);
};
