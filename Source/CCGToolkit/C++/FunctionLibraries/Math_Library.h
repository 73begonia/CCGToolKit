// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Math_Library.generated.h"

/**
 * 
 */
UCLASS()
class CCGTOOLKIT_API UMath_Library : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float PercentageOf_Float(float PercentageOf, float Total);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float PercentageOf_Int(int32 PercentageOf, int32 Total);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static int32 WeightedIntInRange(int32 MaxResult);

	template <typename T>
	static void ShuffleArray(TArray<T>& myArray);

	template <typename T>
	static void SetArrayElement(T item, TArray<T>& item_array, int32 index);

	template<typename TEnum>
	static FString GetEnumValueAsString(const FString& enumName, TEnum enumValue);
};