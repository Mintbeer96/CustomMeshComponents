// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "LayerWeightDataFactory.generated.h"

/**
 * 
 */
UCLASS()
class LAYERWEIGHTEDITOR_API ULayerWeightDataFactory : public UFactory
{
	GENERATED_BODY()
public:
	ULayerWeightDataFactory();
	
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
		UObject* Context, FFeedbackContext* Warn) override;
};
