// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LayerWeightEdGraphNode_Base.generated.h"

/**
 * 
 */
UCLASS()
class LAYERWEIGHTEDITOR_API ULayerWeightEdGraphNode_Base : public UEdGraphNode
{
	GENERATED_BODY()
	
public:
	virtual void PostLoad() override;
	
	/** Begin UEdGraphNode Interface */
#if WITH_EDITOR
	FORCEINLINE virtual void AllocateDefaultPins() override {};
	
	FORCEINLINE virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override {return Super::GetNodeTitle(TitleType);}
	FORCEINLINE virtual FLinearColor GetNodeTitleColor() const override {return GetNodeTitleColor();}
	FORCEINLINE virtual FText GetTooltipText() const override {return GetTooltipText();}
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	
#endif
	/** End UEdGraphNode Interface */

	FORCEINLINE virtual FText GetNodeDisplayName() const
	{return NSLOCTEXT("LayerWeightEdGraph", "BaseNode", "BaseNode");}
};
