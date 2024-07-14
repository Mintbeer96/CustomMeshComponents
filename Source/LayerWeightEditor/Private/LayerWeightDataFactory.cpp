// Fill out your copyright notice in the Description page of Project Settings.


#include "LayerWeightDataFactory.h"

#include "EdGraphSchema_LayerWeightEditor.h"
#include "LayerWeightDataAsset.h"
#include "Kismet2/BlueprintEditorUtils.h"

ULayerWeightDataFactory::ULayerWeightDataFactory()
{
	SupportedClass = ULayerWeightDataAsset::StaticClass();
	bCreateNew = true;
}

UObject* ULayerWeightDataFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	ULayerWeightDataAsset* AssetsObject = NewObject<ULayerWeightDataAsset>(InParent, InClass, InName, Flags | RF_Transactional);
	//@TODO: Add more feature like niagara here.
	
	return AssetsObject;
}