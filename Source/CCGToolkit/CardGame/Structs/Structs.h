// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CCGToolkit/C++/Enums/CardGameEnums.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "../Plugins/2D/Paper2D/Source/Paper2D/Classes/PaperSprite.h"
#include "Engine/DataTable.h"
#include "Structs.generated.h"

/**
 *
 */


USTRUCT(BlueprintType)
struct FSAbility : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAbilityType Type = EAbilityType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Ability_Int = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAbilityTrigger Trigger = EAbilityTrigger::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardUseState After_Use_State = ECardUseState::RemainInPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPlayers Affecting_Player = EPlayers::Self;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* Particle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* SoundCue;
};

USTRUCT(BlueprintType)
struct FSAIPersonality : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIPersonality")
	FName AI_Name = "None";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIPersonality")
	FName AI_Points_Allocation = "AI_Points_Default";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIPersonality")
	EAIDifficulty Difficulty = EAIDifficulty::Easy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIPersonality")
	float Play_Speed = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIPersonality")
	TArray<EAIPersonalityFocus> Personality_Focus = { {EAIPersonalityFocus::Damage_Cards} };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIPersonality")
	TArray<FName> Playable_Decks = { "CloneDeck" };
};

USTRUCT(BlueprintType)
struct FSAIState
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Card_On_Board_State = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Cards_In_Hand_State = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Health_State = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Tota = 0;
};

USTRUCT(BlueprintType)
struct FSAttack : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Damage = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Mana_Cost = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanAttackPlayer = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanAttackCards = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDamageType Damage_Type = EDamageType::Basic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* Attack_Particle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* Attack_SoundCue;
};

USTRUCT(BlueprintType)
struct FSCalculationPoints : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInclude_AI_Points_Per_Card = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Pts_Card_Removed_From_Play = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Pts_Card_Damaged = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Pts_Per_Damage_Point_To_Player = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Pts_Player_Removed_From_Play = 99;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Pts_Owning_Card_Removed_From_Play = -3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Pts_Attack_Owned_Card_With_Value = 0;

};

USTRUCT(BlueprintType)
struct FSCardInteraction
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OwningControllerID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPlayType Interaction_Type = EPlayType::AttackCard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* TalkingCard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* RecievingActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value = 0;
};

USTRUCT(BlueprintType)
struct FSCardLayout : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radial_Offset = -0.05;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Spacing_Min = 180.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Spacing_Max = 440.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Dynamic_Spacing = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Screen_Offset_X = 50.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Screen_Offset_Y = 18.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Rotation_Scale_Factor = 0.15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Max_Cards_In_Hand = 7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Fly_In_Direction = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Transform_Pivot = { 0.5,1.0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Scale = 0.8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Offset = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Rotate_Cards_To_Offset = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPositioning_Profiles Positioning_Profile = EPositioning_Profiles::Fanned;
};

USTRUCT(BlueprintType)
struct FSCardSettings : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Unlocked = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool AddToPlayerDeck = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxNumInDeck = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Weight = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D ScreenSize = { 0.0,0.0 };
};

USTRUCT(BlueprintType)
struct FSCardSystemData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EAIPersonalityFocus> Card_Purpose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Card_Value = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ECardPlayConditions> PlayConditions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EInteractionConditions> InteractionConditions;
};

USTRUCT(BlueprintType)
struct FSVisuals : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture* Frame_Texture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture* Image_Texture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture* Back_Texture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* Frame_Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* Image_Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* Back_material;
};

USTRUCT(BlueprintType)
struct FSDescription : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description = "(Description Top)";
};

USTRUCT(BlueprintType)
struct FSEvaluationPoints : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Pts_Per_Active_Card = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Pts_Per_Card_In_Hand = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Pts_Per_Health_Point = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInclude_Active_Card_Value = false;
};

USTRUCT(BlueprintType)
struct FSEvaluationState
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Board_State_Comp_Value = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float In_Hand_State_Comp_Value = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health_Comp_Value = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Total = 0.0;
};

USTRUCT(BlueprintType)
struct FSHealth : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Health = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Life_Expectancy = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* Dealth_Particle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* Sound_Cue_Death;
};

USTRUCT(BlueprintType)
struct FSPlacement : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPlayers Owner = EPlayers::Self;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Mana_Cost = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* Entry_Particle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* Entry_SoundCue;
};

USTRUCT(BlueprintType)
struct FSPlayerStateSnapshot
{
	GENERATED_USTRUCT_BODY()
public:
	
	int32 PlayerID = 0;

	TArray<FSCard> Card_Struct_Array;

	TArray<int32> Card_IDs_Array;

	TArray<FName> Cards_In_Hand;

	TArray<FName> Deck;

	int32 Health = 0;

	int32 Mana = 0;

	int32 Turn = 0;

};

USTRUCT(BlueprintType)
struct FSCard : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name = NSLOCTEXT("Namespace", "Name", "Hello");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardClass Class = ECardClass::Warrior;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardType Type = ECardType::Creature;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardRarity Rarity = ECardRarity::Common;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Special = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSVisuals Visual;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSAttack Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSHealth Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSAbility> Abilities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSDescription Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSPlacement Placement_Settings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSCardSettings DeckSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSCardSystemData Card_System_Data;

};

USTRUCT(BlueprintType)
struct FSGameStateSnapshot
{
	GENERATED_USTRUCT_BODY()
public:

	TArray<FSPlayerStateSnapshot> Player_Snapshot;
};

USTRUCT(BlueprintType)
struct FSPlaySub
{
	GENERATED_USTRUCT_BODY()
public:

	TArray<FSCardInteraction> CardPlayStruct;
};

USTRUCT(BlueprintType)
struct FSPlay
{
	GENERATED_USTRUCT_BODY()
public:

	TArray<FSPlaySub> Card_Interactions;
};

USTRUCT(BlueprintType)
struct FSPointAllocation : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:

	FSPointAllocation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evaluation")
	FSEvaluationPoints Evaluation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Calculation")
	FSCalculationPoints Calculation;
};

USTRUCT(BlueprintType)
struct FSPrebuildDecksSub : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Card_Table_Name = "None";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity = 0;
};

USTRUCT(BlueprintType)
struct FSPrebuiltDecks : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Table")
	FText Deck_Name = NSLOCTEXT("Namespace","Name","Hello");
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Table")
	bool bEditable = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Table")
	FString Deck_Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Table")
	TArray<FSPrebuildDecksSub> CardsInDeck;
};

USTRUCT(BlueprintType)
struct FSBattleHistory
{
	GENERATED_USTRUCT_BODY()
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OwningControllerID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPlayType Interaction_Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSCard TalkingCard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSCard RecievingCard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OR_RecievingPlayerID;

	FSBattleHistory(){};

};

#pragma region Coroutine

DECLARE_DELEGATE_RetVal(bool, FCoroCondition)

struct WHCoroNode
{
public:
	bool IsActive;
	float RemainTime;
	FCoroCondition ConditionDele;
	WHCoroNode() : IsActive(false) {}

	bool UpDateOperate(int32 SpaceTick)
	{
		if (!IsActive)
		{
			RemainTime = SpaceTick;
			IsActive = true;
			return true;
		}
		else
		{
			RemainTime -= 1;
			if (RemainTime > 0)
				return true;
			else
			{
				IsActive = false;
				return false;
			}
		}
	}

	bool UpDateOperate(float DeltaTime, float SpaceTime)
	{
		if (!IsActive)
		{
			RemainTime = SpaceTime;
			IsActive = true;
			return true;
		}
		else
		{
			RemainTime -= DeltaTime;
			if (RemainTime > 0)
				return true;
			else
			{
				IsActive = false;
				return false;
			}
		}
	}

	bool UpDateOperate(bool* Condition)
	{
		if (!IsActive)
		{
			IsActive = true;
			return true;
		}
		else
		{
			if (*Condition)
				return true;
			else
			{
				IsActive = false;
				return false;
			}
		}
	}

	template<typename UserClass>
	bool UpDateOperate(UserClass* UserObj, typename FCoroCondition::TUObjectMethodDelegate<UserClass>::FMethodPtr InMethod)
	{
		if (!IsActive)
		{
			if (!ConditionDele.IsBound())
				ConditionDele.BindUObject(UserObj, InMethod);
			IsActive = true;
			return true;
		}
		else
		{
			if (ConditionDele.Execute())
				return true;
			else
			{
				IsActive = false;
				return false;
			}
		}
	}

	bool UpDateOperate(TFunction<bool()> InFunction)
	{
		if (!IsActive)
		{
			IsActive = true;
			return true;
		}
		else
		{
			if (InFunction())
				return true;
			else
			{
				IsActive = false;
				return false;
			}
		}
	}

	bool UpDateOperate()
	{
		IsActive = false;
		return false;
	}
};

struct WHCoroTask
{
public:
	bool IsDestory;
	TArray<WHCoroNode*> CoroStack;
	WHCoroTask(int32 CoroCount)
	{
		IsDestory = false;
		for (int i = 0; i <= CoroCount; ++i)
			CoroStack.Push(new WHCoroNode());
	}
	virtual ~WHCoroTask()
	{
		for (int i = 0; i < CoroStack.Num(); i++)
			delete CoroStack[i];
	}
	virtual void Work(float DeltaTime) {}
	bool IsFinish()
	{
		bool Flag = true;
		for (int i = 0; i < CoroStack.Num(); i++)
		{
			if (CoroStack[i]->IsActive)
			{
				Flag = false;
				break;
			}
		}
		return Flag;
	}
};

#pragma endregion

