// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConnectionDrawingPolicy.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraphSchema_LayerWeightEditor.generated.h"
struct FLayerWeightSchemaAction_NewNode;

static FName LayerWeightEdGraphNodeContextMenuSection {"LayerWeightEdGraphNodeContextMenuSection"};
static FName LayerWeightEdGraphPinContextMenuSection {"LayerWeightEdGraphPinContextMenuSection"};
/**
 * 
 */
UCLASS()
class LAYERWEIGHTEDITOR_API UEdGraphSchema_LayerWeightEditor : public UEdGraphSchema
{
	GENERATED_BODY()
	
public:
	TArray<TSharedPtr<FLayerWeightSchemaAction_NewNode> > GetGraphContextActions(const UEdGraph* CurrentGraph, TArray<UObject*>& SelectedObjects, const UEdGraphPin* FromPin, UEdGraph* OwnerOfTemporaries) const;

	/** Begin UEdGraphSchema Interface */
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual void GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID,
		float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const override;
	/** End UEdGraphSchema Interface*/
};

USTRUCT()
struct LAYERWEIGHTEDITOR_API FLayerWeightSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();

	/** Template of node we want to create */
	UPROPERTY()
	class UEdGraphNode* NodeTemplate;

	UPROPERTY()
	FName InternalName;

	FLayerWeightSchemaAction_NewNode() 
		: FEdGraphSchemaAction()
		, NodeTemplate(nullptr)
	{}

	FLayerWeightSchemaAction_NewNode(FText InNodeCategory, FText InMenuDesc, FName InInternalName, FText InToolTip, const int32 InGrouping, FText InKeywords = FText())
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping, InKeywords)
		, NodeTemplate(nullptr), InternalName(InInternalName)
	{}

	//~ Begin FEdGraphSchemaAction Interface
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;
	//~ End FEdGraphSchemaAction Interface

	template <typename NodeType>
	static NodeType* SpawnNodeFromTemplate(class UEdGraph* ParentGraph, NodeType* InTemplateNode, const FVector2D Location, bool bSelectNewNode = true)
	{
		FLayerWeightSchemaAction_NewNode Action;
		Action.NodeTemplate = InTemplateNode;

		return Cast<NodeType>(Action.PerformAction(ParentGraph, NULL, Location, bSelectNewNode));
	}

};

//Policy that controls how to draw connection pins.
class FLayerWeightConnectionDrawingPolicy : public FConnectionDrawingPolicy
{
public:
	FLayerWeightConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraph);
	virtual void DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, /*inout*/ FConnectionParams& Params) override;

private:
	class UEdGraph* Graph;
};
