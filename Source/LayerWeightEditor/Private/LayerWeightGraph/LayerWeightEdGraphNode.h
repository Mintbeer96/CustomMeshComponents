// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LayerWeightEdGraphNode_Base.h"
#include "EdGraph/EdGraphNode.h"
#include "LayerWeightEdGraphNode.generated.h"

static FName LayerWeightEdGraphNodePinCategory {"LayerWeightGraphNodePinCategory"};
static FName LayerWeightEdGraphNodePinSubCategory {"LayerWeightGraphNodePinSubCategory"};
/**
 * 
 */
UCLASS()
class LAYERWEIGHTEDITOR_API ULayerWeightEdGraphNode : public ULayerWeightEdGraphNode_Base
{
	GENERATED_UCLASS_BODY()
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Value;
	/** Begin UEdGraphNode Interface */
	
	// 分配默认的引脚
	virtual void AllocateDefaultPins() override;
	// 节点的Title
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	// 节点Title的颜色
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
	
	virtual FLinearColor GetNodeTitleColor() const override;
	// Tooltip内容
	virtual FText GetTooltipText() const override;
	
	/** End UEdGraphNode Interface */
	virtual void OnRenameNode(const FString& NewName) override;

	void RequestRename() { bRenamePending = true; }
	void RenameStarted() { bRenamePending = false; }
	bool IsRenamePending() const { return bRenamePending; }
	
	bool CanDuplicateNode() const override {return false;}
	bool CanUserDeleteNode() const override {return false;}
	
	
	/** The display name for the title bar of this node. */
	UPROPERTY()
	FText DisplayName;
	
private:
	bool bRenamePending = false;
};

UCLASS()
class LAYERWEIGHTEDITOR_API ULayerWeightEdGraphNode_InOut : public ULayerWeightEdGraphNode_Base
{
	GENERATED_BODY()

public:

#if WITH_EDITOR
	/** Begin UEdGraphNode Interface */
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	/** End UEdGraphNode Interface */

	virtual FText GetNodeDisplayName() const override
	{return NSLOCTEXT("LayerWeightEdGraph", "InOutNode", "InOutNode");}
#endif
};
