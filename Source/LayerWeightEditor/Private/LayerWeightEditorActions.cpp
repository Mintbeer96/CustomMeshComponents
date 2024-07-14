// Fill out your copyright notice in the Description page of Project Settings.


#include "LayerWeightEditorActions.h"
// Fill out your copyright notice in the Description page of Project Settings.


#include "LayerWeightEditorActions.h"
#include "LayerWeightDataAsset.h"
#include "LayerWeightEditorToolKit.h"

FLayerWeightEditorActions::FLayerWeightEditorActions(EAssetTypeCategories::Type Type)
{
	LayerWeightEditorCategory = Type;
}

FText FLayerWeightEditorActions::GetName() const
{
	return NSLOCTEXT("LayerWeightEditorModule","LayerWeightEditor.DisplayName", "Layer Weight Editor Asset");
}

FColor FLayerWeightEditorActions::GetTypeColor() const
{
	return FColor::Emerald;
}

UClass* FLayerWeightEditorActions::GetSupportedClass() const
{
	return ULayerWeightDataAsset::StaticClass();
}

uint32 FLayerWeightEditorActions::GetCategories()
{
	return LayerWeightEditorCategory;
}

void FLayerWeightEditorActions::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type ToolKitModeType = EditWithinLevelEditor ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	
	for (auto ObjectIterator = InObjects.CreateConstIterator(); ObjectIterator; ++ObjectIterator)
	{
		if (ULayerWeightDataAsset* OurAsset = Cast<ULayerWeightDataAsset>(*ObjectIterator))
		{
			const TSharedRef<FLayerWeightEditorToolKit> LayerWeightEditorToolKit = MakeShareable(new FLayerWeightEditorToolKit());
			LayerWeightEditorToolKit->InitializeAssetEditor(ToolKitModeType, EditWithinLevelEditor, OurAsset);
		}
	}
}
