// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomPrimitiveSubsystem.h"
#include "SceneViewExtension.h"

/**
 * 
 */
class CUSTOMPRIMITIVE_API FCustomPrimitvieViewExtension : public FSceneViewExtensionBase
{
public:
	FCustomPrimitvieViewExtension(const FAutoRegister& AutoReg, UCustomPrimitiveSubsystem* InWorldSubsystem);
	~FCustomPrimitvieViewExtension();

	// FSceneViewExtensionBase interface begin : 
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override;
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {}
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual void PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily) override {}
	virtual void PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) override;
	// FSceneViewExtensionBase interface end
	
	virtual void PostRenderBasePass_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) override;;

private:
	UCustomPrimitiveSubsystem* WorldSubsystem;
};
