﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "LayerWeightEditorDataFactory.generated.h"

/**
 * 
 */
UCLASS()
class LAYERWEIGHTEDITOR_API ULayerWeightEditorDataFactory : public UFactory
{
	GENERATED_BODY()
public:
	ULayerWeightEditorDataFactory();

	virtual bool CanCreateNew() const override;
	virtual bool ShouldShowInNewMenu() const override;
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
