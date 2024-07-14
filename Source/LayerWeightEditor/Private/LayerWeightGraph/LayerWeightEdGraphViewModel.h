#pragma once

#include "CoreMinimal.h"
#include "EditorUndoClient.h"
#include "GraphEditor.h"

class FLayerWeightEdGraphViewModel : public TSharedFromThis<FLayerWeightEdGraphViewModel>, public FEditorUndoClient
{
public:
	FLayerWeightEdGraphViewModel();
	virtual ~FLayerWeightEdGraphViewModel();

	TSharedRef<FUICommandList> GetCommands(){return Commands;}
	
	void Initialize();

	void SetGraphEdPtr(TSharedPtr<SGraphEditor> UpdateGraphEdPtr);

	void SetGraph(UEdGraph* InGraph)
	{
		this->EdGraph = InGraph;
	}
	
	UEdGraph* GetGraph(){return EdGraph;}

	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override { PostUndo(bSuccess); }
	
private:
	void SetupCommands();

	void SelectAllNodes();
	void DeleteSelectedNodes();
	bool CanDeleteNodes();
	void CutSelectedNodes();
	bool CanCutNodes();
	void CopySelectedNodes();
	bool CanCopyNodes();
	void PasteNodes();
	bool CanPasteNodes();
	void DuplicateNodes();
	bool CanDuplicateNodes();
	void RenameNode();
	bool CanRenameNode();

	void PasteNodesHere(const FVector2D& Location);

	FGraphPanelSelectionSet GetSelectedNodes();

	UEdGraph* EdGraph;

	/** Currently focused graph */
	TWeakPtr<SGraphEditor> UpdateGraphEdPtr;
	
	TSharedRef<FUICommandList> Commands;
};
