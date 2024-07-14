#pragma once
#include "LayerWeightEdGraphNode.h"
#include "SGraphNode.h"

class SLayerWeightEdGraphNode  : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SLayerWeightEdGraphNode){}
	SLATE_END_ARGS()

		SLayerWeightEdGraphNode()
	{
	}

	~SLayerWeightEdGraphNode()
	{
	}

	void Construct(const FArguments& InArgs, ULayerWeightEdGraphNode* InNode)
	{
		GraphNode = InNode;
		LayerWeightEdGraphNode = InNode;
		UpdateGraphNode();
	}
	ULayerWeightEdGraphNode* LayerWeightEdGraphNode;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
};
