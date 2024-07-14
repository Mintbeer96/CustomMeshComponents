// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LayerWeightEditorTypeCustomization.generated.h"

/**
 * 
 */
USTRUCT()
struct  LAYERWEIGHTEDITOR_API FLayerWeightEditorTypeCustomization : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY()
public:

	FLayerWeightEditorTypeCustomization():FEdGraphSchemaAction(
		NSLOCTEXT("LayerWeightEdGraph", "LayerWeightEdGraph.ExtraOpsCategory", "特殊操作菜单"),
		NSLOCTEXT("LayerWeightEdGraph", "Print Hello Action", "打印Hello World"),
		NSLOCTEXT("LayerWeightEdGraph", "Print Hello Tooltip", "打印Hello World的提示"),
		0
	){}

	// 核心函数
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override;
};
