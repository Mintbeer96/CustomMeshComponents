// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPrimitiveSubsystem.h"

#include "CustomPrimitvieViewExtension.h"
#include "SceneViewExtension.h"

UCustomPrimitiveSubsystem::UCustomPrimitiveSubsystem():Super()
{
	
}

void UCustomPrimitiveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Mgr = new FComputeShaderManager();
	Mgr->InitInstanceBufferGenResource();
	
	// TickDelegate = FTickerDelegate::CreateUObject(this, &UCustomPrimitiveSubsystem::HandleTicker);
	// FTicker::GetCoreTicker().AddTicker(TickDelegate, 0.01f);

	// if(!CustomPrimitvieViewExtension)
	// {
	CustomPrimitvieViewExtension = FSceneViewExtensions::NewExtension<FCustomPrimitvieViewExtension>(this);
	// }

}



bool UCustomPrimitiveSubsystem::RequestUpdateBuffer(FComputeShaderTask Task, FRHITexture2D* DepthResource)
{
	if(Task.Comp.Get()->IsValidLowLevel())
	{
		
		ENQUEUE_RENDER_COMMAND(CSM_IssueDrawIndirectTask)(
		[this, Task, DepthResource](FRHICommandListImmediate& RHICmdList)
		{
			if(Mgr){
				if(Task.Comp->TextureInput)
				{
					Mgr->IssueInstanceDataGenTask(RHICmdList,Task.FeatureLevel, 0.25f, Task.Comp->TextureInput->Resource);
					Mgr->IssueInstanceBufferGenTask(RHICmdList, Task.FeatureLevel, Task.Views, Task.Comp->GetComponentLocation(), DepthResource);
					Mgr->IssueDrawIndirectTask(RHICmdList, Task.FeatureLevel);
					Mgr->TransitBufferToIndirectArgs(RHICmdList);
				}
			}
		});
	}
	return true;
}

void UCustomPrimitiveSubsystem::AddDrawIndirect(uint32 NumIndicesPerInstance, uint32 InInstanceCount,
	uint32 StartIndexLocation)
{
	Mgr->AddDrawIndirect(NumIndicesPerInstance, InInstanceCount, StartIndexLocation );
}
