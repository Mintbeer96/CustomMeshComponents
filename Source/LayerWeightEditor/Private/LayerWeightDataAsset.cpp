// Fill out your copyright notice in the Description page of Project Settings.


#include "LayerWeightDataAsset.h"
#include "EdGraphSchema_LayerWeightEditor.h"
#include "LayerWeightEdGraphNode.h"
#include "Kismet2/BlueprintEditorUtils.h"

void ULayerWeightDataAsset::PostLoad()
{
	UObject::PostLoad();
}

ULayerWeightDataAsset::ULayerWeightDataAsset()
{
	
}

//We would like to init editoronly data here.
void ULayerWeightDataAsset::PostInitProperties()
{
	Super::PostInitProperties();
	//This sentence ensures that only triggered when first created.
	if (HasAnyFlags(RF_ClassDefaultObject | RF_NeedLoad) == false)
	{
#if WITH_EDITORONLY_DATA && WITH_EDITOR
		EdGraph = NewObject<UEdGraph>(this, "LayerWeightOverview", RF_Transactional);
		EdGraph->Schema = UEdGraphSchema_LayerWeightEditor::StaticClass();
	
		// EdGraph->AddNode(EdGraphNode);
		EdGraph->AddToRoot();
		//We add our init node here.
		TArray<ULayerWeightEdGraphNode*> OverviewNodes;
		EdGraph->GetNodesOfClass<ULayerWeightEdGraphNode>(OverviewNodes);
		if(OverviewNodes.Num()<=0)
		{
			FGraphNodeCreator<ULayerWeightEdGraphNode> StartNodeCreator(*EdGraph);
			ULayerWeightEdGraphNode* StartNode = StartNodeCreator.CreateNode(false);
			StartNode->DisplayName = FText::FromString("Start Node");
			
			// FVector2D NewLocation = EdGraph->GetGoodPlaceForNewNode();
			// StartNode->NodePosX = 0;
			// StartNode->NodePosY = 0;
			StartNodeCreator.Finalize();
			EdGraph->NotifyGraphChanged();
		}
#endif
	}
}
