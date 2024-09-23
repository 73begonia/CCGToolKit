// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CardGameEnums.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStateEnterSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStateExitSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStateUpdateSignature);
/**
 *
 */
UCLASS()
class CCGTOOLKIT_API UCardGameEnums : public UObject
{
	GENERATED_BODY()

};

#pragma region EffectsEnumsPart

UENUM(BlueprintType)
enum class EBoardPlayerEffects : uint8
{
	DecreasedHealth,
	IncreasedHealth,
	Death
};

UENUM(BlueprintType)
enum class ECardEffects : uint8
{
	OnCardPlaced,
	OnAttack,
	DecreasedHealth,
	IncreasedHealth,
	OnAbilityUsed,
	OnDeath
};

#pragma endregion


#pragma region EGameplayPart
UENUM(BlueprintType)
enum class E3DCardMovementState : uint8
{
	Placing,
	MoveToHomeDestination,
	MovingToDestination,
	PlacedOnBoard,
	Attacking,
	Selected,
	Graveyard
};

UENUM(BlueprintType)
enum class EAbilityTrigger : uint8
{
	None,
	OnDrop,
	StartOfTurn,
	EndOfTurn,
	TakesDamage,
	SentToGraveyard,
	DeathByLifeExpectancy
};

UENUM(BlueprintType)
enum class EAbilityType : uint8
{
	None,
	DrawCards,
	IncreaseAttack,
	Clone,
	IncreasePlayerHealth,
	RetaliationDamage,
	DamageAllCardsOnBoard,
	SpawnRandomCardFromDeck,
	GiveTurnPointsToAllActiveCards,
	IncreaseTurnPoints,
	DiscardCardsInHand,
	PickupFirstCreatureInTheGraveyardToBoard,
	PickupFirstCreatureInTheGraveyardToHand,
	PossessOpponentsCard,
	SwapCardsInHand
};

UENUM(BlueprintType)
enum class ECardClass : uint8
{
	None,
	Warrior,
	Mage,
	Spy,
	Obelisk
};

UENUM(BlueprintType)
enum class ECardPlayConditions : uint8
{
	None,
	PlayAtStartOfTurnAIOnly,
	PlayAtEndOfTurnAIOnly,
	PlayWhenLowHealthSelf,
	PlayWhenHighHealthSelf,
	PlayWhenCreaturesAreOnTheBoardSelf,
	PlayWhenCreaturesAreOnTheBoardSelfAIOnly,
	PlayWhenCreaturesAreOnTheBoardOpponent,
	PlayWhenCreaturesAreOnTheBoardOpponentAIOnly,
	PlayWhenThereAreCardsInTheGraveyard,
	PlayWhenThereAreCardsInTheGraveyardAIOnly,
	PlayWhenLowPlayableCardInHandAIOnly,
};

UENUM(BlueprintType)
enum class ECardRarity : uint8
{
	Common,
	Uncommon,
	Rare,
	UltraRare,
	Legendary
};

UENUM(BlueprintType)
enum class ECardSet : uint8
{
	Empty,
	BasicSet,
	DebugCardSet,
	EmptySet
};

UENUM(BlueprintType)
enum class ECardType : uint8
{
	None,
	HERO,
	Creature,
	Spell,
	Trap,
	Aura,
	Booster
};

UENUM(BlueprintType)
enum class ECardUseState : uint8
{
	RemainInPlay,
	SingleUseSendToGraveyardBeforeAbility,
	SingleUseSendToGraveyardAfterAbility,
	EndOfTurn,
	TakesDamage,
	SentToGraveyard,
	DeathByLifeExpectancy
};

UENUM(BlueprintType)
enum class ECardWidgetMovementState : uint8
{
	Pending,
	MoveToHandDestination,
	MoveToPreviewDestination,
	ReturnFromPreviewDestination
};

UENUM(BlueprintType)
enum class ECostType : uint8
{
	Mana,
	TurnPoint
};

UENUM(BlueprintType)
enum class EDamageType : uint8
{
	Basic,
	Poison,
	Stun
};

UENUM(BlueprintType)
enum class EGameState : uint8
{
	Startup,
	MainMenu,
	LoadingScreen,
	Playing,
	StoreComingSoon,
	DeckBuildingMenuComingSoon
};

UENUM(BlueprintType)
enum class EGameTurn : uint8
{
	TurnActive,
	TurnInactive,
	Waiting
};

UENUM(BlueprintType)
enum class EInteractionConditions : uint8
{
	None,
	CardCanOnlyRecieveInteractionOnOwnedPlacement,
	CardCanOnlyRecieveInteractionOnAnOppenentsPlacement,
	CanOnlyAttackCreatures,
	CannotAttackIfPlayerOwnsCard,
	CanRecieveSelfInteractionAIOnly,
	CanRecieveSelfInteractionIfDoesNotRemoveCardFromPlayAIOnly
};

UENUM(BlueprintType)
enum class EManaAccount : uint8
{
	None,
	Placement,
	Attack
};

UENUM(BlueprintType)
enum class EPlayers : uint8
{
	Self,
	Opponent,
	SelfAndOpponent
};

UENUM(BlueprintType)
enum class EPlayerStateEnum : uint8
{
	PendingAction,
	CardInteraction,
	PlacingCard
};

UENUM(BlueprintType)
enum class ETurnState : uint8
{
	TurnActive,
	TurnInactive,
	UpdateCurrentState
};

#pragma endregion


#pragma region EMenuEnumsPart

UENUM(BlueprintType)
enum class EArenaList : uint8
{
	Empty,
	Arena_Classic,
	Arena_Doubles,
	Arena_EaglesNest,
	Arena_FlatGrounds,
	Arena_GreatWall,
	Arena_RainyDay,
	Arena_StoneGarden
};

UENUM(BlueprintType)
enum class ELibraryCardSelectableState : uint8
{
	None,
	Selectable,
	PreviewSelectable,
	Locked,
	NotSelectable,
	CannotAddToDeck
};

UENUM(BlueprintType)
enum class EMainMenuState : uint8
{
	MainMenu,
	PlayOptions,
	DeckSelection,
	CreateGame,
	ArenaSelection,
	JoinOrFindGame,
	DeckBuilder,
	Store,
	GameOptions
};

#pragma endregion


#pragma region ESystemPart

UENUM(BlueprintType)
enum class EAITurnState : uint8
{
	Initialization,
	Execution,
	Termination
};

UENUM(BlueprintType)
enum class EAlphabeticalSorting : uint8
{
	AToZ,
	ZToA,
};

UENUM(BlueprintType)
enum class ECardRowOptions : uint8
{
	None,
	Create,
	Save,
	Edit,
	Load,
	Clear,
	Cancel,
	Delete,
	Select
};

UENUM(BlueprintType)
enum class EDirection : uint8
{
	Left,
	CenterLeft,
	CenterRight,
	Right
};

UENUM(BlueprintType)
enum class EEndGameResults : uint8
{
	Victory,
	Defeat,
	Draw
};

UENUM(BlueprintType)
enum class EErrors : uint8
{
	ValidDrop,
	NotaValidPlacement,
	CastFailedtoPlayer,
	PlacementFullORNotvalidPlayerPlacement,
	PlacementFull,
	NotEnoughMana,
	MaxCardsallowedforplacement,
	PlacementDoesNotSupportCardType,
	IncorrectPlayerPlacementfoCard,
	InvalidPlacement,
	NoSupportedDropLocationFound,
	CardNotFoundInHandServer,
	Oneisinvalid
};

UENUM(BlueprintType)
enum class EManaCostSorting : uint8
{
	LowesttoHighest,
	HighesttoLowest
};

UENUM(BlueprintType)
enum class EPlatform : uint8
{
	Windows,
	iOS,
	Android,
	HTML5,
	Mac,
	Linux
};

UENUM(BlueprintType)
enum class EPriority : uint8
{
	None,
	Low,
	Medium,
	High
};

UENUM(BlueprintType)
enum class ESortingTypeFilter : uint8
{
	Name,
	ManaCost
};


#pragma endregion

#pragma region AIPart

UENUM(BlueprintType)
enum class EAIDifficulty : uint8
{
	Easy,
	Normal,
	Hard
};

UENUM(BlueprintType)
enum class EAIPersonalityFocus : uint8
{
	None,
	Increase_Player_Health,
	Damage_Cards,
	Damage_Opponent_Player,
	Playing_Cards,
	Picking_up_cards,
	Opponent_Hindrance
};

UENUM(BlueprintType)
enum class EAIRisk : uint8
{
	Low_Risk,
	Average_Risk,
	High_Risk
};

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Initialization,
	Execution,
	Termination
};

UENUM(BlueprintType)
enum class EPlayType : uint8
{
	AttackCard,
	AttackPlayer
};

#pragma endregion

UENUM(BlueprintType)
enum class EPositioning_Profiles : uint8
{
	Fanned,
	Horizontal
};

UENUM(BlueprintType)
enum class EViewState : uint8
{
	Default,
	CardInPreview,
	ViewHand,
	HideHand,
	TestProfile
};

UENUM(BlueprintType)
enum EStackAction { Push, Pop};

