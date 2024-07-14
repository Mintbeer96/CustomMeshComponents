// Fill out your copyright notice in the Description page of Project Settings.


#include "LayerWeightEditorTypeCustomization.h"

UEdGraphNode* FLayerWeightEditorTypeCustomization::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
	const FVector2D Location, bool bSelectNewNode)
{
	if (FromPin)
	{
		FMessageDialog::Open(EAppMsgType::Ok, NSLOCTEXT("LayerWeightEdGraph", "Print Action With Pin", "Hello World，并且带了个节点"));
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, NSLOCTEXT("LayerWeightEdGraph", "Print Action Without Pin", "Hello World"));
	}
	return nullptr;
}
