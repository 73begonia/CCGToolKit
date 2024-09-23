// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EvaluationRules_FunctionLibrary.generated.h"

class ACard3D;
/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UEvaluationRules_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static int32 RuleCalculateActiveCardPoints(int32 Add, int32 PtsPerActiveCard, TArray<ACard3D*> CardsActive, bool IncludePlayValue, int32& RuleTotal);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static int32 RuleCalculateCardsInHandPoints(int32 Add, int32 PtsPerCardnHand, int32 CardsInHand, int32& RuleTotal);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static int32 RuleCalculateHealthPoints(int32 Add, int32 PtsPerHealthPoint, int32 PlayerHealth, int32& RuleTotal);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool RuleCalculateAttackPoints(int32 Add, int32 PtsForDamage, int32 PtsForRemovingCardFromPlay, int32 Attack, int32 Health, int32 CardValue, bool IncludeCardValue, int32& Total, int32& RuleTotal);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static int32 RuleCalculateAdditionalAbilityPointOffset(ACard3D* TalkingCard, ACard3D* RecievingCard);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool RuleCalculateDamagetoPlayerPoints(int32 Add, int32 PtsForPlayerDamage, int32 PtsForRemovingPlayer, int32 CardAttack, int32 PlayerHealth, int32& Total, int32& RuleTotal);

	UFUNCTION(BlueprintCallable)
	static bool RuleCalculateOwnedCardAbilityPointOffset(ACard3D* TalkingCard, ACard3D* RecievingCard, int32& Points);
};
