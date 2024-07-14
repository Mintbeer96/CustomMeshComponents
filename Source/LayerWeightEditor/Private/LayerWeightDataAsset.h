// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LayerWeightDataAsset.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class LAYERWEIGHTEDITOR_API ULayerWeightDataAsset : public UObject
{
	GENERATED_BODY()

	virtual void PostLoad() override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layer Editor Data")
	FName AssetsName;

	ULayerWeightDataAsset();

	virtual void PostInitProperties() override;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	UEdGraph* EdGraph;
#endif
};
 