// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPrimitvieViewExtension.h"
#include "CustomPrimitiveSubsystem.h"
#include "LevelEditorViewport.h"
#include "Renderer/Private/PostProcess/SceneRenderTargets.h"

FCustomPrimitvieViewExtension::FCustomPrimitvieViewExtension(const FAutoRegister& AutoReg, UCustomPrimitiveSubsystem* InWorldSubsystem)
: FSceneViewExtensionBase(AutoReg), WorldSubsystem(InWorldSubsystem)
{
}

FCustomPrimitvieViewExtension::~FCustomPrimitvieViewExtension()
{
}

void FCustomPrimitvieViewExtension::SetupViewFamily(FSceneViewFamily& InViewFamily)
{
}


void FCustomPrimitvieViewExtension::PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView)
{
}

void FCustomPrimitvieViewExtension::PostRenderBasePass_RenderThread(FRHICommandListImmediate& RHICmdList,
                                                                    FSceneView& InView)
{
	if(WorldSubsystem)
	{
		TArray<const FSceneView*> Views;
		Views.Add(&InView);

		FSceneRenderTargets& SceneContext = FSceneRenderTargets::Get(RHICmdList);
		FRHITexture2D* DepthRHITexture = SceneContext.GetSceneDepthSurface();
			
		for (UCustomPrimitiveComponent* Component : WorldSubsystem->Components)
		{
		
			if (Component->StaticMesh) {
				WorldSubsystem->AddDrawIndirect(Component->StaticMesh->RenderData->LODResources[0].IndexBuffer.GetNumIndices(),
					Component->InstanceCount, 0);
				FComputeShaderTask Task;
				Task.Comp = Component;
				Task.FeatureLevel = SceneContext.GetCurrentFeatureLevel();
				Task.Views = Views;
				
				WorldSubsystem->RequestUpdateBuffer(Task, DepthRHITexture);
			}
		}
	}
}
