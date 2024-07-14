#include "LayerWeightEdGraphViewModel.h"

#include "EdGraphUtilities.h"
#include "LayerWeightEdGraphNode.h"
#include "Framework/Commands/GenericCommands.h"
#include "Windows/WindowsPlatformApplicationMisc.h"

FLayerWeightEdGraphViewModel::FLayerWeightEdGraphViewModel(): Commands(MakeShareable(new FUICommandList()))
{
	UEditorEngine* Editor = (UEditorEngine*)GEngine;
	if (Editor != NULL)
	{
		Editor->RegisterForUndo(this);
	}
}

FLayerWeightEdGraphViewModel::~FLayerWeightEdGraphViewModel()
{
	UEditorEngine* Editor = (UEditorEngine*)GEngine;
	if (Editor)
	{
		Editor->UnregisterForUndo(this);
	}
}

//Get rid of SGraphEditor later 
void FLayerWeightEdGraphViewModel::Initialize()
{
	SetupCommands();
}

void FLayerWeightEdGraphViewModel::SetGraphEdPtr(TSharedPtr<SGraphEditor> InUpdateGraphEdPtr)
{
	UpdateGraphEdPtr = InUpdateGraphEdPtr;
}

void FLayerWeightEdGraphViewModel::PostUndo(bool bSuccess)
{
	// The graph may have been deleted as a result of the undo operation so make sure it's valid
	// before using it.
	// UEdGraph* Graph = GetGraph();
	// if (Graph != nullptr)
	// {
	EdGraph->NotifyGraphChanged();
	// }
}

void FLayerWeightEdGraphViewModel::SetupCommands()
{
	Commands->MapAction(
		FGenericCommands::Get().SelectAll,
		FExecuteAction::CreateRaw(this, &FLayerWeightEdGraphViewModel::SelectAllNodes));

	Commands->MapAction(
		FGenericCommands::Get().Delete,
		FExecuteAction::CreateRaw(this, &FLayerWeightEdGraphViewModel::DeleteSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FLayerWeightEdGraphViewModel::CanDeleteNodes));
	
	Commands->MapAction(
		FGenericCommands::Get().Cut,
		FExecuteAction::CreateRaw(this, &FLayerWeightEdGraphViewModel::CutSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FLayerWeightEdGraphViewModel::CanCutNodes));
	
	Commands->MapAction(
		FGenericCommands::Get().Copy,
		FExecuteAction::CreateRaw(this, &FLayerWeightEdGraphViewModel::CopySelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FLayerWeightEdGraphViewModel::CanCopyNodes));

	Commands->MapAction(
		FGenericCommands::Get().Paste,
		FExecuteAction::CreateRaw(this, &FLayerWeightEdGraphViewModel::PasteNodes),
		FCanExecuteAction::CreateRaw(this, &FLayerWeightEdGraphViewModel::CanPasteNodes));
	
	Commands->MapAction(
		FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateRaw(this, &FLayerWeightEdGraphViewModel::DuplicateNodes),
		FCanExecuteAction::CreateRaw(this, &FLayerWeightEdGraphViewModel::CanDuplicateNodes));

	Commands->MapAction(
		FGenericCommands::Get().Rename,
		FExecuteAction::CreateRaw(this, &FLayerWeightEdGraphViewModel::RenameNode),
		FCanExecuteAction::CreateRaw(this, &FLayerWeightEdGraphViewModel::CanRenameNode));
}

void FLayerWeightEdGraphViewModel::SelectAllNodes()
{
	if(UpdateGraphEdPtr.Pin())
		UpdateGraphEdPtr.Pin()->SelectAllNodes();
	
}

void FLayerWeightEdGraphViewModel::DeleteSelectedNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	const FScopedTransaction Transaction(FGenericCommands::Get().Delete->GetDescription());
	CurrentGraphEditor->GetCurrentGraph()->Modify();

	const FGraphPanelSelectionSet SelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*NodeIt))
		{
			if (Node->CanUserDeleteNode())
			{
				Node->Modify();
				Node->DestroyNode();
			}
		}
	}

	// Update UI
	UpdateGraphEdPtr.Pin()->NotifyGraphChanged();
}


FGraphPanelSelectionSet FLayerWeightEdGraphViewModel::GetSelectedNodes()
{
	FGraphPanelSelectionSet CurrentSelection;
	if (TSharedPtr<SGraphEditor> FocusedGraphEd = UpdateGraphEdPtr.Pin())
	{
		CurrentSelection = FocusedGraphEd->GetSelectedNodes();
	}

	return CurrentSelection;
}

bool FLayerWeightEdGraphViewModel::CanDeleteNodes()
{
	// If any of the nodes can be deleted then we should allow deleting
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanUserDeleteNode())
		{
			return true;
		}
	}

	return false;
}
bool FLayerWeightEdGraphViewModel::CanCutNodes()
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FLayerWeightEdGraphViewModel::CutSelectedNodes()
{
	// Collect nodes which can not be delete or duplicated so they can be reselected.
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	TSet<UObject*> CanBeDuplicatedAndDeleted;
	TSet<UObject*> CanNotBeDuplicatedAndDeleted;
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* SelectedGraphNode = Cast<UEdGraphNode>(*SelectedIter);
		if (SelectedGraphNode != nullptr)
		{
			if (SelectedGraphNode->CanDuplicateNode() && SelectedGraphNode->CanUserDeleteNode())
			{
				CanBeDuplicatedAndDeleted.Add(*SelectedIter);
				
			}
			else
			{
				CanNotBeDuplicatedAndDeleted.Add(*SelectedIter);
			}
		}
	}

	// Select the nodes which can be copied and deleted, copy and delete them, and then restore the ones which couldn't be copied or deleted.
	for(auto NodeObject:CanBeDuplicatedAndDeleted)
	{
		UEdGraphNode* SelectedGraphNode = Cast<UEdGraphNode>(NodeObject);
		if(!SelectedGraphNode)
		{
			continue;
		}
		UpdateGraphEdPtr.Pin()->SetNodeSelection(SelectedGraphNode, true);
	}
	
	CopySelectedNodes();
	DeleteSelectedNodes();

	for(auto NodeObject:CanNotBeDuplicatedAndDeleted)
	{
		UEdGraphNode* SelectedGraphNode = Cast<UEdGraphNode>(NodeObject);
		if(!SelectedGraphNode)
		{
			continue;
		}
		UpdateGraphEdPtr.Pin()->SetNodeSelection(SelectedGraphNode, true);
	}
	
	// Update UI
	UpdateGraphEdPtr.Pin()->NotifyGraphChanged();
}

void FLayerWeightEdGraphViewModel::CopySelectedNodes()
{
	TSet<UObject*> NodesToCopy;
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* SelectedGraphNode = Cast<UEdGraphNode>(*SelectedIter);
		if (SelectedGraphNode != nullptr)
		{
			if (SelectedGraphNode->CanDuplicateNode())
			{
				SelectedGraphNode->PrepareForCopying();
				NodesToCopy.Add(*SelectedIter);
			}
		}
	}

	FString ExportedText;
	FEdGraphUtilities::ExportNodesToText(NodesToCopy, ExportedText);
	FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

bool FLayerWeightEdGraphViewModel::CanCopyNodes() 
{
	UEdGraph* Graph = GetGraph();
	if (Graph != nullptr)
	{
		const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
		for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
		{
			UEdGraphNode* SelectedGraphNode = Cast<UEdGraphNode>(*SelectedIter);
			if (SelectedGraphNode != nullptr && SelectedGraphNode->CanDuplicateNode())
			{
				return true;
			}
		}
	}
	return false;
}

void FLayerWeightEdGraphViewModel::PasteNodes()
{
	if(UpdateGraphEdPtr.Pin())
	{
		PasteNodesHere(UpdateGraphEdPtr.Pin()->GetPasteLocation());
	}
}

bool FLayerWeightEdGraphViewModel::CanPasteNodes()
{
	UEdGraph* Graph = GetGraph();
	if (Graph == nullptr)
	{
		return false;
	}

	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

	return FEdGraphUtilities::CanImportNodesFromText(Graph, ClipboardContent);
}

void FLayerWeightEdGraphViewModel::DuplicateNodes()
{
	CopySelectedNodes();
	PasteNodes();
}

bool FLayerWeightEdGraphViewModel::CanDuplicateNodes()
{
	return CanCopyNodes();
}

void FLayerWeightEdGraphViewModel::RenameNode()
{
	UEdGraph* Graph = GetGraph();
	if (Graph != nullptr)
	{
		const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
		
		if (SelectedNodes.Num() > 0)
		{
			for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes);
				SelectedIter; ++SelectedIter)
			{
				ULayerWeightEdGraphNode* SelectedOverviewNode = Cast<ULayerWeightEdGraphNode>(*SelectedIter);
				if (SelectedOverviewNode != nullptr)
				{
					// SelectedOverviewNode->O();
					SelectedOverviewNode->RequestRename();
					return;
				}
			}
		}
	}
}

bool FLayerWeightEdGraphViewModel::CanRenameNode()
{
	UEdGraph* Graph = GetGraph();
	if (Graph != nullptr)
	{
		const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
		
		if (SelectedNodes.Num() > 0)
		{
			for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter;
				++SelectedIter)
			{
				UEdGraphNode* SelectedOverviewNode = Cast<UEdGraphNode>(*SelectedIter);
				if (SelectedOverviewNode != nullptr && SelectedOverviewNode->GetCanRenameNode())
				{
					// SelectedOverviewNode->O();
					return true;
				}
			}
		}
	}
	return false;
}

void FLayerWeightEdGraphViewModel::PasteNodesHere(const FVector2D& Location)
{
	UEdGraph* Graph = GetGraph();
	if (Graph != nullptr)
	{
		const FScopedTransaction Transaction(FGenericCommands::Get().Paste->GetDescription());;
		EdGraph->Modify();

		UpdateGraphEdPtr.Pin()->ClearSelectionSet();

		// Grab the text to paste from the clipboard.
		FString TextToImport;
		FPlatformApplicationMisc::ClipboardPaste(TextToImport);

		// Import the nodes
		TSet<UEdGraphNode*> PastedNodes;
		FEdGraphUtilities::ImportNodesFromText(EdGraph, TextToImport, PastedNodes);
		
		//Average position of nodes so we can move them while still maintaining relative distances to each other
		FVector2D AvgNodePosition(0.0f, 0.0f);
	
		// Number of nodes used to calculate AvgNodePosition
		int32 AvgCount = 0;

		for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
		{
			UEdGraphNode* EdNode = *It;
			//UAIGraphNode* AINode = Cast<UAIGraphNode>(EdNode);
			if (EdNode)
			{
				AvgNodePosition.X += EdNode->NodePosX;
				AvgNodePosition.Y += EdNode->NodePosY;
				++AvgCount;
			}
		}

		if (AvgCount > 0)
		{
			float InvNumNodes = 1.0f / float(AvgCount);
			AvgNodePosition.X *= InvNumNodes;
			AvgNodePosition.Y *= InvNumNodes;
		}

		// TArray<FNiagaraSystemViewModel::FEmitterHandleToDuplicate> EmitterHandlesToDuplicate;
		for (UEdGraphNode* PastedNode : PastedNodes)
		{
			
			// Select the newly pasted stuff
			UpdateGraphEdPtr.Pin()->SetNodeSelection(PastedNode, true);

			PastedNode->NodePosX = (PastedNode->NodePosX - AvgNodePosition.X) + Location.X;
			PastedNode->NodePosY = (PastedNode->NodePosY - AvgNodePosition.Y) + Location.Y;

			PastedNode->SnapToGrid(16);

			// Give new node a different Guid from the old one
			PastedNode->CreateNewGuid();
		}
		
		// Update UI
		UpdateGraphEdPtr.Pin()->NotifyGraphChanged();

		UObject* GraphOwner = EdGraph->GetOuter();
		if (GraphOwner)
		{
			GraphOwner->PostEditChange();
			GraphOwner->MarkPackageDirty();
		}
		// GetSystemViewModel()->DuplicateEmitters(EmitterHandlesToDuplicate);
	}
}
