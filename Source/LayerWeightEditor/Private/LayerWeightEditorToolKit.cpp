// Fill out your copyright notice in the Description page of Project Settings.

#include "LayerWeightEditorToolKit.h"

#include "EdGraphSchema_LayerWeightEditor.h"
#include "GraphEditor.h"
#include "LayerWeightDataAsset.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphSchema.h"
#include "LayerWeightEdGraphNode.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "LayerWeightEditor"
FText FLayerWeightEditorToolKit::GetBaseToolkitName() const
{
	return LOCTEXT("LayerWeightEditor", "Layer Weight Asset Editor"); 
}

FString FLayerWeightEditorToolKit::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("LayerWeightEditor", "Layer Weight Asset Editor").ToString(); 
}


TSharedRef<SDockTab> FLayerWeightEditorToolKit::SpawnDetailTab(const FSpawnTabArgs& SpawnTabArgs)
{
	//加载属性编辑器模块
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	const FDetailsViewArgs DetailsViewArgs;
	//创建属性编辑器的Slate
	AssetPropertyView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	//将对象传入，这样就是自动生成对象的属性面板
	AssetPropertyView->SetObject(GetEditingObject());
	return SNew(SDockTab)
	[
		AssetPropertyView.ToSharedRef()
	];
}

UEdGraphNode* FLayerWeightEditorToolKit::CreateDebugNode(UEdGraph* ParentGraph, const FVector2D NodeLocation) const
{
	check(ParentGraph != nullptr)
	UEdGraphNode* ResultGraphNode = NewObject<ULayerWeightEdGraphNode>(ParentGraph);
	ParentGraph->Modify();
	ResultGraphNode->SetFlags(RF_Transactional);

	ResultGraphNode->Rename(nullptr, ParentGraph, REN_NonTransactional);
	ResultGraphNode->CreateNewGuid();
	ResultGraphNode->NodePosX = NodeLocation.X;
	ResultGraphNode->NodePosY = NodeLocation.Y;

	ResultGraphNode->AllocateDefaultPins();
	return ResultGraphNode;
}

void FLayerWeightEditorToolKit::OnNodeDoubleClicked(UEdGraphNode* Node)
{
}

void FLayerWeightEditorToolKit::OnSelectedNodesChanged(const TSet<UObject*>& NewSelection)
{
	TArray<UObject*> Selection;

	for (UObject* SelectionEntry : NewSelection)
	{
		Selection.Add(SelectionEntry);
	}
	if (Selection.Num() == 0) 
	{
		AssetPropertyView->SetObject(EdGraph);
	}
	else
	{
		AssetPropertyView->SetObjects(Selection);
	}
}

void FLayerWeightEditorToolKit::OnNodeTitleCommitted(const FText& Text, ETextCommit::Type Arg,
	UEdGraphNode* EdGraphNode)
{
	if (EdGraphNode)
	{
		static const FText TranscationTitle = FText::FromString(FString(TEXT("Rename Node")));
		const FScopedTransaction Transaction(TranscationTitle);
		EdGraphNode->Modify();
		EdGraphNode->OnRenameNode(Text.ToString());
	}
}

//
void FLayerWeightEditorToolKit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	TSharedRef<FWorkspaceItem> AppMenuGroup = TabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("LayerWeightEditorMenu.EditorGroup", "LayerWeightEditor"));
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FLayerWeightEditorToolKit::OnSelectedNodesChanged);
	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FLayerWeightEditorToolKit::OnNodeDoubleClicked);
	InEvents.OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &FLayerWeightEditorToolKit::OnNodeTitleCommitted);
	
	CurrentViewModel = MakeShared<FLayerWeightEdGraphViewModel>();
	CurrentViewModel->Initialize();
	
	GraphEditor = SNew(SGraphEditor)
	//Here is how viewmodel interact with the graph editor.
				.AdditionalCommands(CurrentViewModel->GetCommands())
				.GraphToEdit(EdGraph)
				.GraphEvents(InEvents);
	

	CurrentViewModel->SetGraphEdPtr(GraphEditor);
	CurrentViewModel->SetGraph(EdGraph);
	
	
	InTabManager->RegisterTabSpawner(PropertyEditorTabName, FOnSpawnTab::CreateRaw(this, &FLayerWeightEditorToolKit::SpawnDetailTab)).SetGroup(AppMenuGroup);
	InTabManager->RegisterTabSpawner(GraphEditorTabName, FOnSpawnTab::CreateLambda([&](const FSpawnTabArgs& SpawnTabArgs)
	{
		return SNew(SDockTab)
		[
			GraphEditor.ToSharedRef()
		];
	}))
	.SetGroup(AppMenuGroup);
}

void FLayerWeightEditorToolKit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
	InTabManager->UnregisterTabSpawner(PropertyEditorTabName);
	InTabManager->UnregisterTabSpawner(GraphEditorTabName);
}

void FLayerWeightEditorToolKit::InitializeAssetEditor(const EToolkitMode::Type Mode,
                                                      const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* InAssets)
{
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_LayerWeightPropertyTab_Layout_v1")
	->AddArea
	(
		FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
		->Split
		(
			FTabManager::NewStack()
			->SetSizeCoefficient(0.1f)
			->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
			->SetHideTabWell(true)
		)
		->Split
		(
			FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
			->Split
			(
				FTabManager::NewStack()
				->AddTab(GraphEditorTabName, ETabState::OpenedTab)
				->SetHideTabWell(true)
			)
			->Split
			(
				FTabManager::NewStack()
				->AddTab(PropertyEditorTabName, ETabState::OpenedTab)
				->SetHideTabWell(true)
				->SetSizeCoefficient(0.25f)
			)
		)
	);

	EdGraph = Cast<ULayerWeightDataAsset>(InAssets)->EdGraph;
	check(EdGraph != nullptr)
	InitAssetEditor(Mode, InitToolkitHost, FName("LayerWeightDataAssetEditor"), StandaloneDefaultLayout, true, true, InAssets);
	//RegenerateMenusAndToolbars();
}
#undef LOCTEXT_NAMESPACE 
