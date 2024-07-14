// Fill out your copyright notice in the Description page of Project Settings.


#include "LayerWeightEditorDataFactory.h"

#include "EdGraphSchema_LayerWeightEditor.h"
#include "LayerWeightDataAsset.h"
#include "Kismet2/BlueprintEditorUtils.h"

ULayerWeightEditorDataFactory::ULayerWeightEditorDataFactory()
{
	SupportedClass = ULayerWeightDataAsset::StaticClass();
	bCreateNew = true;
}

bool ULayerWeightEditorDataFactory::CanCreateNew() const
{
	return true;
}

bool ULayerWeightEditorDataFactory::ShouldShowInNewMenu() const
{
	return true;
}

UObject* ULayerWeightEditorDataFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	ULayerWeightDataAsset* AssetsObject = NewObject<ULayerWeightDataAsset>(InParent, InClass, InName, Flags | RF_Transactional);
	//@TODO: Add more feature like niagara here.
	
	return AssetsObject;
}