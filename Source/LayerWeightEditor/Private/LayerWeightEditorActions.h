// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AssetTypeActions_Base.h"

/**
 * 
 */
class LAYERWEIGHTEDITOR_API FLayerWeightEditorActions : public FAssetTypeActions_Base
{
public:
	FLayerWeightEditorActions(EAssetTypeCategories::Type Type);

	virtual FText GetName() const override;

	virtual FColor GetTypeColor() const override;

	virtual UClass* GetSupportedClass() const override;

	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;

	virtual uint32 GetCategories() override;

private:
	EAssetTypeCategories::Type LayerWeightEditorCategory;
};
