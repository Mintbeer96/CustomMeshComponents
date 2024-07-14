// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "LayerWeightEdGraphViewModel.h"
#include "Toolkits/AssetEditorToolkit.h"
static const FName GraphEditorTabName{ "LayerWeightDataGraphEditor" };
static const FName PropertyEditorTabName{ "LayerWeightDataAssetEditor" };
/**
 * 
 */
class LAYERWEIGHTEDITOR_API FLayerWeightEditorToolKit : public FAssetEditorToolkit
{
	
	virtual FName GetToolkitFName() const override { return FName("LayerWeightEditorToolKit"); }
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor::Green; }

public:
	void OnNodeTitleCommitted(const FText& Text, ETextCommit::Type Arg, UEdGraphNode* EdGraphNode);
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	
	void InitializeAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* InAssets);

private:
	TSharedRef<SDockTab> SpawnDetailTab(const FSpawnTabArgs& SpawnTabArgs) ;

	TSharedPtr<SGraphEditor> GraphEditor;
	TSharedPtr<IDetailsView> AssetPropertyView;

	TSharedPtr<FLayerWeightEdGraphViewModel> CurrentViewModel;
	
	UEdGraph* EdGraph = nullptr;
	
	UEdGraphNode* CreateDebugNode(UEdGraph* ParentGraph, const FVector2D NodeLocation) const;

	void OnSelectedNodesChanged(const TSet<UObject*>& NewSelection);
	void OnNodeDoubleClicked(UEdGraphNode* Node);
};


