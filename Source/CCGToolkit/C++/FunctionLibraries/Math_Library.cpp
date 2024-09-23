// Fill out your copyright notice in the Description page of Project Settings.


#include "Math_Library.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

float UMath_Library::PercentageOf_Float(float PercentageOf, float Total)
{
	return PercentageOf/(PercentageOf+Total);
}

float UMath_Library::PercentageOf_Int(int32 PercentageOf, int32 Total)
{
	if (float(PercentageOf)>0)
	{
		return float(PercentageOf)/float(PercentageOf+Total);
	}
	else
	{
		return 0;
	}
}

int32 UMath_Library::WeightedIntInRange(int32 MaxResult)
{
	float RandomNumber;
	RandomNumber = UKismetMathLibrary::RandomFloatInRange(0,100);
	TArray<int32> Array = {0,65,80,90,100};
	for (int32 i =0;i<Array.Num();i++)
	{
		if (UKismetMathLibrary::InRange_FloatFloat(RandomNumber, Array[i], Array[i+1], true, true ))
		{
			return UKismetMathLibrary::Clamp(i, 0, MaxResult);
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

template <typename T>
void UMath_Library::ShuffleArray(TArray<T>& myArray)
{
	if (myArray.Num() > 0)
	{
		int32 LastIndex = myArray.Num() - 1;
		for (int32 i = 0; i <= LastIndex; ++i)
		{
			int32 Index = FMath::RandRange(i, LastIndex);
			if (i != Index)
			{
				myArray.Swap(i, Index);
			}
		}
	}
}

template <typename T>
void UMath_Library::SetArrayElement(T item, TArray<T>& item_array, int32 index)
{
	if (item_array.Num() - 1 < index)
		item_array.SetNum(index);

	item_array[index] = item;
	//item_array.Insert(item, index);
}

template<typename TEnum>
FString UMath_Library::GetEnumValueAsString(const FString& enumName, TEnum enumValue)
{	
	UEnum* enumPtr = FindObject<UEnum>((UObject*)ANY_PACKAGE, *enumName, true);
	if (!enumPtr)
	{
		return FString("InvaildEnumName");
	}
	return enumPtr->GetNameStringByIndex((int32)enumValue);
}	

