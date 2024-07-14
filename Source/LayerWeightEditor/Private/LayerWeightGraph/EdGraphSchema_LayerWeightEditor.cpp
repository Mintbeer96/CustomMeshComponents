// Fill out your copyright notice in the Description page of Project Settings.


#include "EdGraphSchema_LayerWeightEditor.h"

#include "LayerWeightEdGraphNode.h"
#include "LayerWeightEditorTypeCustomization.h"
#include "ToolMenu.h"
#include "ToolMenuSection.h"
#include "Classes/EditorStyleSettings.h"
#include "Framework/Commands/GenericCommands.h"

//UEdGraphSchema_LayerWeightEditor::UEdGraphSchema_LayerWeightEditor(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//}
namespace 
{
	// Maximum distance a drag can be off a node edge to require 'push off' from node
	const int32 NodeDistance = 60;
}

TSharedPtr<FLayerWeightSchemaAction_NewNode> AddNewNodeAction(TArray<TSharedPtr<FLayerWeightSchemaAction_NewNode> >& NewActions,
	const FText& Category, const FText& MenuDesc, const FName& InternalName, const FText& Tooltip, FText Keywords = FText())
{
	TSharedPtr<FLayerWeightSchemaAction_NewNode> NewAction = TSharedPtr<FLayerWeightSchemaAction_NewNode>(
		new FLayerWeightSchemaAction_NewNode(Category, MenuDesc, InternalName, Tooltip, 0, Keywords));
	NewActions.Add(NewAction);
	return NewAction;
}

TArray<TSharedPtr<FLayerWeightSchemaAction_NewNode>> UEdGraphSchema_LayerWeightEditor::GetGraphContextActions(
	const UEdGraph* CurrentGraph, TArray<UObject*>& SelectedObjects, const UEdGraphPin* FromPin,
	UEdGraph* OwnerOfTemporaries) const
{
	TArray<TSharedPtr<FLayerWeightSchemaAction_NewNode> > NewActions;
	// Add InOut Node
	{
		const FText MenuDesc = NSLOCTEXT("LayerWeightEdGraphSchema", "LayerWeightInOutNode","In Out Node");
        const FText TooltipDesc = NSLOCTEXT("LayerWeightEdGraphSchema", "LayerWeightInOutNodeHint", "Add a node contains in and out.");
        TSharedPtr<FLayerWeightSchemaAction_NewNode> FunctionCallAction = AddNewNodeAction(NewActions, NSLOCTEXT
        	("LayerWeightEdGraphSchema", "Function Menu Title", "Functions"), MenuDesc, TEXT("LayerWeightInOutNode"), TooltipDesc);
        UEdGraphNode* ResultGraphNode = NewObject<ULayerWeightEdGraphNode_InOut>(OwnerOfTemporaries);
        FunctionCallAction->NodeTemplate = ResultGraphNode;
	}
	
	return NewActions;
}

void UEdGraphSchema_LayerWeightEditor::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	const TSharedPtr<FLayerWeightEditorTypeCustomization> Action = MakeShareable(new FLayerWeightEditorTypeCustomization);
	ContextMenuBuilder.AddAction(Action);

	//Temporarily add here, need to handle all actions in a for loop
	//In Niagara they try to cast graph first.
	//const UNiagaraGraph* NiagaraGraph = CastChecked<UNiagaraGraph>(ContextMenuBuilder.CurrentGraph);
	
	const TSharedPtr<FLayerWeightSchemaAction_NewNode> CreateNodeAction = MakeShareable(new FLayerWeightSchemaAction_NewNode);
	TArray<TSharedPtr<FLayerWeightSchemaAction_NewNode> > NewActions = GetGraphContextActions(ContextMenuBuilder.CurrentGraph,
		ContextMenuBuilder.SelectedObjects, ContextMenuBuilder.FromPin, ContextMenuBuilder.OwnerOfTemporaries);
	for (int32 i = 0; i < NewActions.Num(); i++)
	{
		ContextMenuBuilder.AddAction(NewActions[i]);
	}
	
	Super::GetGraphContextActions(ContextMenuBuilder);
}

void UEdGraphSchema_LayerWeightEditor::GetContextMenuActions(UToolMenu* Menu,
                                                             UGraphNodeContextMenuContext* Context) const
{
	if (Context->Node)
	{
		FToolMenuSection& Section = Menu->AddSection(LayerWeightEdGraphNodeContextMenuSection, NSLOCTEXT("LayerWeightEdGraph",
			"LayerWeightEdGraph.NodeContextMenuSection", "LayerWeightEdNodeMenu"));
		Section.AddMenuEntry(FGenericCommands::Get().Copy);
		Section.AddMenuEntry(FGenericCommands::Get().Cut);
		Section.AddMenuEntry(FGenericCommands::Get().Paste);
		Section.AddMenuEntry(FGenericCommands::Get().Delete);
	}
	if (Context->Pin)
	{
		FToolMenuSection& Section = Menu->AddSection(LayerWeightEdGraphPinContextMenuSection, NSLOCTEXT("LayerWeightEdGraph",
			"LayerWeightEdGraph.PinContextMenuSection", "LayerWeightEdPinMenu"));
		Section.AddMenuEntry(FGenericCommands::Get().Delete);
	}
	Super::GetContextMenuActions(Menu, Context);
}

const FPinConnectionResponse UEdGraphSchema_LayerWeightEditor::CanCreateConnection(const UEdGraphPin* A,
	const UEdGraphPin* B) const
{
	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT("OK!"));
}

FConnectionDrawingPolicy* UEdGraphSchema_LayerWeightEditor::CreateConnectionDrawingPolicy(int32 InBackLayerID,
	int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements,
	UEdGraph* InGraphObj) const
{
	return new FLayerWeightConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect,
	                                            InDrawElements, InGraphObj);
}

UEdGraphNode* FLayerWeightSchemaAction_NewNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
                                                              const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = nullptr;
	
	if (NodeTemplate != nullptr)
	{
		ULayerWeightEdGraphNode_Base* GraphNodeTemplate = Cast<ULayerWeightEdGraphNode_Base>(NodeTemplate);
		if(!GraphNodeTemplate)
		{
			return ResultNode;
		}
		const FScopedTransaction Transaction(NSLOCTEXT("LayerWeightEd", "LayerWeightEdNewNode", "LayerWeightEd:New Node"));
		ParentGraph->Modify();

		NodeTemplate->SetFlags(RF_Transactional);

		// set outer to be the graph so it doesn't go away
		NodeTemplate->Rename(NULL, ParentGraph, REN_NonTransactional);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);

		// For input pins, new node will generally overlap node being dragged off
		// Work out if we want to visually push away from connected node
		int32 XLocation = Location.X;
		if (FromPin && FromPin->Direction == EGPD_Input)
		{
			UEdGraphNode* PinNode = FromPin->GetOwningNode();
			const float XDelta = FMath::Abs(PinNode->NodePosX - Location.X);

			if (XDelta < NodeDistance)
			{
				// Set location to edge of current node minus the max move distance
				// to force node to push off from connect node enough to give selection handle
				XLocation = PinNode->NodePosX - NodeDistance;
			}
		}

		NodeTemplate->NodePosX = XLocation;
		NodeTemplate->NodePosY = Location.Y;
		NodeTemplate->SnapToGrid(GetDefault<UEditorStyleSettings>()->GridSnapSize);

		ResultNode = NodeTemplate;
	}
	return ResultNode;
}

UEdGraphNode* FLayerWeightSchemaAction_NewNode::PerformAction(UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins,
                                                              const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = NULL;

	if (FromPins.Num() > 0)
	{
		ResultNode = PerformAction(ParentGraph, FromPins[0], Location, bSelectNewNode);

		if (ResultNode)
		{
			// Try autowiring the rest of the pins
			for (int32 Index = 1; Index < FromPins.Num(); ++Index)
			{
				ResultNode->AutowireNewNode(FromPins[Index]);
			}
		}
	}
	else
	{
		ResultNode = PerformAction(ParentGraph, NULL, Location, bSelectNewNode);
	}

	return ResultNode;
}

void FLayerWeightSchemaAction_NewNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	// These don't get saved to disk, but we want to make sure the objects don't get GC'd while the action array is around
	Collector.AddReferencedObject( NodeTemplate );
}


FLayerWeightConnectionDrawingPolicy::FLayerWeightConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID,
	float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraph)
	: FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements)
	, Graph(CastChecked<UEdGraph>(InGraph))
{
	ArrowImage = nullptr;
	ArrowRadius = FVector2D::ZeroVector;
}

void FLayerWeightConnectionDrawingPolicy::DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, /*inout*/ FConnectionParams& Params)
{
	FConnectionDrawingPolicy::DetermineWiringStyle(OutputPin, InputPin, Params);
	if (HoveredPins.Contains(InputPin) && HoveredPins.Contains(OutputPin))
	{
		Params.WireThickness = Params.WireThickness * 5;
	}

	if (Graph)
	{
		const UEdGraphSchema_LayerWeightEditor* LWSchema = Cast<UEdGraphSchema_LayerWeightEditor>(Graph->GetSchema());
		if (LWSchema && OutputPin)
		{
			Params.WireColor = FLinearColor::White;
		}
	}
}

