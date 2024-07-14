// Fill out your copyright notice in the Description page of Project Settings.
#include "LayerWeightEdGraphNode.h"

#include "SLayerWeightEdGraphNode.h"

ULayerWeightEdGraphNode::ULayerWeightEdGraphNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Value = 0;
	bCanRenameNode = true;
}

void ULayerWeightEdGraphNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, LayerWeightEdGraphNodePinCategory, LayerWeightEdGraphNodePinSubCategory, nullptr, TEXT(""));
}

FText ULayerWeightEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	//@TODO: Get this work properly to rename the node.
	if(!DisplayName.IsEmpty())
		return DisplayName;
	return FText::FromString(GetName());
}

TSharedPtr<SGraphNode> ULayerWeightEdGraphNode::CreateVisualWidget()
{
	return SNew(SLayerWeightEdGraphNode, this);
	//return Super::CreateVisualWidget();
}

FLinearColor ULayerWeightEdGraphNode::GetNodeTitleColor() const
{
	return FLinearColor::Red;
}

FText ULayerWeightEdGraphNode::GetTooltipText() const
{
	return NSLOCTEXT("LayerWeightEdGraphNode", "StartNodeTip", "Graph Start ToolTip");
}

void ULayerWeightEdGraphNode::OnRenameNode(const FString& NewName)
{
	this->DisplayName = FText::FromString(NewName);
}

#if WITH_EDITOR
void ULayerWeightEdGraphNode_InOut::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, LayerWeightEdGraphNodePinCategory, LayerWeightEdGraphNodePinSubCategory, nullptr, TEXT("In"));
	CreatePin(EGPD_Output, LayerWeightEdGraphNodePinCategory, LayerWeightEdGraphNodePinSubCategory, nullptr, TEXT("Out"));
}

FText ULayerWeightEdGraphNode_InOut::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return NSLOCTEXT("LayerWeightEdGraphNode_InOut", "InOutNodeTitle", "InOutNode");
}

FLinearColor ULayerWeightEdGraphNode_InOut::GetNodeTitleColor() const
{
	return FLinearColor::Green;
}

FText ULayerWeightEdGraphNode_InOut::GetTooltipText() const
{
	return NSLOCTEXT("LayerWeightEdGraphNode_InOut", "InOutNodeTip", "In Out Node Tip");
}
#endif