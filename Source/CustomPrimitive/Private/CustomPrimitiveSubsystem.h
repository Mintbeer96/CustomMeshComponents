// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComputeShaderManager.h"
#include "CustomPrimitiveComponent.h"
#include "EditorSubsystem.h"
#include "Containers/Ticker.h"
#include "Subsystems/Subsystem.h"
#include "UObject/Object.h"
#include "CustomPrimitiveSubsystem.generated.h"

class FCustomPrimitvieViewExtension;

struct FComputeShaderTask
{
	FComputeShaderTask() :
	Comp(nullptr),
	Views(TArray<const FSceneView*>()),
	FeatureLevel(ERHIFeatureLevel::SM5)
	{
	}
	bool operator==(const FComputeShaderTask& a) const
	{
		bool bSameView = true;
		for(auto View:a.Views)
		{
			bSameView &= this->Views.Contains(View);
		}
		
		return Comp.Get() == a.Comp.Get() && FeatureLevel == a.FeatureLevel && bSameView;
	}
	
	TSoftObjectPtr<UCustomPrimitiveComponent> Comp;
	FTexture* Resourcce;
	ERHIFeatureLevel::Type FeatureLevel;
	TArray<const FSceneView*> Views;
};


UCLASS()
class CUSTOMPRIMITIVE_API UCustomPrimitiveSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UCustomPrimitiveSubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	FComputeShaderManager* GetComputeShaderManager(){return Mgr;};
	
	//Currently we might only have one buffer task. 
	bool RequestUpdateBuffer(FComputeShaderTask Task, FRHITexture2D* DepthResource);

	void AddDrawIndirect(uint32 NumIndicesPerInstance, uint32 InInstanceCount,
											  uint32 StartIndexLocation);

	TSharedPtr<FCustomPrimitvieViewExtension, ESPMode::ThreadSafe> CustomPrimitvieViewExtension;

	TArray<UCustomPrimitiveComponent*> Components;
private:

	FComputeShaderManager* Mgr;
};
