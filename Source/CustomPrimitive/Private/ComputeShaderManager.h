#pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "ShaderPermutation.h"

// NumIndicesPerInstance
// InstanceCount;
// StartIndexLocation
// BaseVertexLocation
// StartInstanceLocation

#define MAX_COMPUTEINSTANCE_COUNT 256

class CUSTOMPRIMITIVE_API FInstanceDataGenCS: public FGlobalShader
{
	//Declare this class as a global shader
	DECLARE_GLOBAL_SHADER(FInstanceDataGenCS);
public:	
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return RHISupportsComputeShaders(Parameters.Platform);
	}
	
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

	FInstanceDataGenCS() {}
	FInstanceDataGenCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);
	
	void SetOutput(FRHICommandList& RHICmdList, FRHIUnorderedAccessView* InOutputPositionBufferUAV, FRHIUnorderedAccessView* InOutputArgsUAV);
	void SetParameters(FRHICommandList& RHICmdList, float InThreshold, const FTexture* InInputTexture);
	void UnbindBuffers(FRHICommandList& RHICmdList);
	
protected:
	
	LAYOUT_FIELD(FRWShaderParameter, OutputArgsParm)
	LAYOUT_FIELD(FRWShaderParameter, OutputPositionBufferParm)
	
	LAYOUT_FIELD(FShaderResourceParameter, InputTextureParm)
	LAYOUT_FIELD(FShaderResourceParameter, InputTexSamplerParm)
	LAYOUT_FIELD(FShaderParameter, ThresholdParm)
	
};

class CUSTOMPRIMITIVE_API FDrawIndirectArgsGenCS: public FGlobalShader
{
	//Declare this class as a global shader
	DECLARE_GLOBAL_SHADER(FDrawIndirectArgsGenCS);
	
public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return RHISupportsComputeShaders(Parameters.Platform);
	}
	
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

	FDrawIndirectArgsGenCS() {}
	FDrawIndirectArgsGenCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	void SetOutput(FRHICommandList& RHICmdList, FRHIUnorderedAccessView* DrawIndirectArgsUAV);
	void SetParameters(FRHICommandList& RHICmdList, int32 NumIndicesPerInstance, FRHIUnorderedAccessView* InstanceCountUAV
		, FRHIUnorderedAccessView* CountBeforeCullingUAV);
	void UnbindBuffers(FRHICommandList& RHICmdList);

protected:
	LAYOUT_FIELD(FRWShaderParameter, DrawIndirectArgsParm)
	LAYOUT_FIELD(FShaderParameter, NumIndicesPerInstanceParm)
	LAYOUT_FIELD(FRWShaderParameter, InstanceArgsParm)
	LAYOUT_FIELD(FRWShaderParameter, CountBeforeCullingParm)
};
//
// class CUSTOMPRIMITIVE_API FHZBCullingCS: public FGlobalShader
// {
// 	//Declare this class as a global shader
// 	DECLARE_GLOBAL_SHADER(FHZBCullingCS);
// public:
// 	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
// 	{
// 		return RHISupportsComputeShaders(Parameters.Platform);
// 	}
// 	
// 	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters,
// 		FShaderCompilerEnvironment& OutEnvironment);
//
// 	FHZBCullingCS() {}
// 	FHZBCullingCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);
//
// 	void SetOutput(FRHICommandList& RHICmdList, FRHIUnorderedAccessView* OutInstanceOriginUAV,
// 		FRHIUnorderedAccessView* OutInstanceTransformUAV, FRHIUnorderedAccessView* InstanceCountUAV);
// 	void SetParameters(FRHICommandList& RHICmdList, FRHIUnorderedAccessView* InstanceCountUAV,FRHIShaderResourceView* PossiblePosSRV, 
// 		const TArray<const FSceneView*>& Views, const FVector ComponentLoc);
// 	void UnbindBuffers(FRHICommandList& RHICmdList);
//
// protected:
// 	//In Instance Position
//
// 	//In Depth Texture
//
// 	//InOut Result Buffer
//
// 	//InOut
// 	LAYOUT_FIELD(FRWShaderParameter, DrawIndirectArgsParm)
// 	LAYOUT_FIELD(FShaderParameter, NumIndicesPerInstanceParm)
// 	LAYOUT_FIELD(FRWShaderParameter, InstanceArgsParm)
// 	LAYOUT_FIELD(FRWShaderParameter, CountBeforeCullingParm)
// };


class CUSTOMPRIMITIVE_API FInstanceBufferGenCS: public FGlobalShader
{
	//Declare this class as a global shader
	DECLARE_GLOBAL_SHADER(FInstanceBufferGenCS);
	
public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return RHISupportsComputeShaders(Parameters.Platform);
	}
	
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters,
		FShaderCompilerEnvironment& OutEnvironment);

	FInstanceBufferGenCS() {}
	FInstanceBufferGenCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	void SetOutput(FRHICommandList& RHICmdList, FRHIUnorderedAccessView* OutInstanceOriginUAV,
		FRHIUnorderedAccessView* OutInstanceTransformUAV, FRHIUnorderedAccessView* InstanceCountUAV);
	void SetParameters(FRHICommandList& RHICmdList, FRHIUnorderedAccessView* InstanceCountUAV,FRHIShaderResourceView* PossiblePosSRV, 
		const TArray<const FSceneView*>& Views, const FVector ComponentLoc,FRHITexture2D* DepthResource);
	void UnbindBuffers(FRHICommandList& RHICmdList);

protected:
	LAYOUT_FIELD(FRWShaderParameter, InstanceOriginBufferParm)
	LAYOUT_FIELD(FRWShaderParameter, InstanceTransformBufferParm)
	LAYOUT_FIELD(FRWShaderParameter, InstanceArgsParm)
	LAYOUT_FIELD(FRWShaderParameter, InstanceCountParm)
	LAYOUT_FIELD(FShaderParameter, WorldToClipParm)
	LAYOUT_FIELD(FShaderResourceParameter, PossibleInstancePosParm)
	LAYOUT_FIELD(FShaderParameter, ViewLocationParm);
	LAYOUT_FIELD(FShaderParameter, ComponentLocationParm);

	LAYOUT_FIELD(FShaderResourceParameter, DepthTextureParm)
	LAYOUT_FIELD(FShaderResourceParameter, DepthTexSamplerParm)
		
};


//这个结构体用于传递数据，比起散装的数据，这种结构体使得代码更紧凑。
struct FArgsGenInfo
{
	FArgsGenInfo(){}
	
	FArgsGenInfo(uint32 InNumIndicesPerInstance, uint32 InStartIndexLocation, uint32 InInstanceCount)
	: NumIndicesPerInstance(InNumIndicesPerInstance)
	, InstanceCount(InInstanceCount)
	, StartIndexLocation(InStartIndexLocation)
	{
		
	}
	
	uint32 NumIndicesPerInstance = 0;
	uint32 InstanceCount = 0;
	uint32 StartIndexLocation = 0;
	// BaseVertexLocation
	// StartInstanceLocation

	bool operator==(const FArgsGenInfo& Rhs) const
	{
		return NumIndicesPerInstance == Rhs.NumIndicesPerInstance
			&& StartIndexLocation == Rhs.StartIndexLocation
			&& InstanceCount == Rhs.InstanceCount;
	}
	
};

class FComputeShaderManager
{
public:
	FComputeShaderManager();
	~FComputeShaderManager();

	void InitRHI();
	void ReleaseRHI();

	bool bFlag = false;
	/**
	 * Issue a drawindirect task. Using unpacked params.
	 */
	uint32 AddDrawIndirect(uint32 NumIndicesPerInstance, uint32 InstanceCount, uint32 StartIndexLocation);

	// /**
	//  * Issue a drawindirect task. Using compact struct.
	//  * So this function should be responsible for calling the function to dispatch the compute shader.
	//  */
	// uint32 AddDrawIndirect(FArgsGenInfo ArgsGenInfo);

	void InitInstanceBufferGenResource();
	
	void IssueInstanceBufferGenTask(FRHICommandList& RHICmdList, ERHIFeatureLevel::Type FeatureLevel,
		const TArray<const FSceneView*>& Views, const FVector CompLocation, FRHITexture2D* DepthResource);

	void IssueInstanceDataGenTask(FRHICommandList& RHICmdList, ERHIFeatureLevel::Type FeatureLevel, float InThreshold, FTexture* InInputTexture);
	/**
	 * Do the actually issuing task.hg
	 */
	void IssueDrawIndirectTask(FRHICommandList& RHICmdList, ERHIFeatureLevel::Type FeatureLevel);

	void TransitBufferToIndirectArgs(FRHICommandList& RHICmdList);
	
	FRWBuffer& GetDrawIndirectBuffer(){return DrawIndirectBuffer;}

	FRHIShaderResourceView* GetInstanceOriginBufferSRV()
	{
		return InstanceOriginBuffer.SRV;	
	};
	
	FRHIShaderResourceView* GetInstanceTransformBufferSRV()
	{
		return InstanceTransformBuffer.SRV;
	};
	
	bool bHasInit = false;
	
protected:
	int32 OldInstanceCount = -1;
	
	FArgsGenInfo GenInfo;

	/** Buffer holds draw indirect args. */
	FRWBuffer DrawIndirectBuffer;
	
	/** Buffer holds instance transform. */
	FRWBuffer InstanceOriginBuffer;
	
	/** Buffer holds instance transform args. */
	FRWBuffer InstanceTransformBuffer;

	/** Buffer holds instance related args. */
	FRWBuffer InstanceArgsBuffer;

	/** Buffer holds instance DataGen args. */
	FRWBuffer PreCullingCountBuffer;

	/** Buffer holds instance DataGen args. */
	FRWBuffer DataGenPositionBuffer;
	
	/** Num of instance arguments, currently we only have 1. */
	uint32 InstanceArgsCount = 1;

};


