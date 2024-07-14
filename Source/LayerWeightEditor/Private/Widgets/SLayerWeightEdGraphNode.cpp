#include "SLayerWeightEdGraphNode.h"

void SLayerWeightEdGraphNode::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime,
	const float InDeltaTime)
{
	if (GraphNode != nullptr)
	{
		if (LayerWeightEdGraphNode->IsRenamePending() && !SGraphNode::IsRenamePending())
		{
			SGraphNode::RequestRename();
			LayerWeightEdGraphNode->RenameStarted();
		}
	}
}
