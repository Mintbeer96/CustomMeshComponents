#include "ComputeShaderManager.h"
#include "Renderer/Private/SceneRendering.h"

IMPLEMENT_GLOBAL_SHADER(FInstanceBufferGenCS, "/CustomPrimitiveCSDir/InstanceBufferGenCS.usf", "MainCS", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(FDrawIndirectArgsGenCS, "/CustomPrimitiveCSDir/DrawIndirectArgsGenCS.usf", "MainCS", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(FInstanceDataGenCS, "/CustomPrimitiveCSDir/DrawIndirectDataGenCS.usf", "MainCS", SF_Compute);
// IMPLEMENT_GLOBAL_SHADER(FHZBCullingCS, "/CustomPrimitiveCSDir/HZBCullingCS.usf", "MainCS", SF_Compute);
// This will tell the engine to create the shader and where the shader entry point is.
//                        ShaderType              ShaderPath             Shader function name     ShaderType

#define BUFFER_SAMPLE_X_COUNT 16
#define BUFFER_SAMPLE_Y_COUNT 16
#define BUFFER_SAMPLE_Z_COUNT 32

void FInstanceDataGenCS::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters,
	FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	//Currently hardcode 1 for thread count.
	OutEnvironment.SetDefine(TEXT("BUFFER_SAMPLE_X"), BUFFER_SAMPLE_X_COUNT);
	OutEnvironment.SetDefine(TEXT("BUFFER_SAMPLE_Y"), BUFFER_SAMPLE_Y_COUNT);
	// OutEnvironment.SetDefine(TEXT("BUFFER_SAMPLE_Z"), 1);
}

FInstanceDataGenCS::FInstanceDataGenCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer): FGlobalShader(Initializer)
{
	OutputArgsParm.Bind(Initializer.ParameterMap, TEXT("OutputArgs"));
	OutputPositionBufferParm.Bind(Initializer.ParameterMap,TEXT("OutputPositionBuffer"));
	InputTextureParm.Bind(Initializer.ParameterMap,TEXT("InputTexture"));
	InputTexSamplerParm.Bind(Initializer.ParameterMap,TEXT("InputTexSampler"));
	ThresholdParm.Bind(Initializer.ParameterMap,TEXT("Threshold"));
}

void FInstanceDataGenCS::SetOutput(FRHICommandList& RHICmdList, FRHIUnorderedAccessView* InOutputPositionBufferUAV, FRHIUnorderedAccessView* InOutputArgsUAV)
{
	FRHIComputeShader* ComputeShaderRHI = RHICmdList.GetBoundComputeShader();
	if (OutputPositionBufferParm.IsUAVBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, OutputPositionBufferParm.GetUAVIndex(), InOutputPositionBufferUAV);
	}
	
	if (OutputArgsParm.IsUAVBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, OutputArgsParm.GetUAVIndex(), InOutputArgsUAV);
	}
}


void FInstanceDataGenCS::SetParameters(FRHICommandList& RHICmdList, float InThreshold, const FTexture* InInputTexture)
{
	FRHIComputeShader* ComputeShaderRHI = RHICmdList.GetBoundComputeShader();
	FRHISamplerState* CSSamplerState = TStaticSamplerState<SF_Point, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	SetTextureParameter(RHICmdList, ComputeShaderRHI, InputTextureParm, InputTexSamplerParm, CSSamplerState,InInputTexture->TextureRHI);
	SetShaderValue(RHICmdList, ComputeShaderRHI, ThresholdParm, InThreshold);
}

void FInstanceDataGenCS::UnbindBuffers(FRHICommandList& RHICmdList)
{
	FRHIComputeShader* ComputeShaderRHI = RHICmdList.GetBoundComputeShader();
	
	if (OutputPositionBufferParm.IsUAVBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, OutputPositionBufferParm.GetUAVIndex(), nullptr);
	}
	
	if (OutputArgsParm.IsUAVBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, OutputArgsParm.GetUAVIndex(), nullptr);
	}
}


void FDrawIndirectArgsGenCS::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	//Currently hardcode 1 for thread count.
	OutEnvironment.SetDefine(TEXT("ARGS_GEN_THREAD_COUNT"), 1);
}

FDrawIndirectArgsGenCS::FDrawIndirectArgsGenCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
	DrawIndirectArgsParm.Bind(Initializer.ParameterMap, TEXT("DrawIndirectArgs"));
	NumIndicesPerInstanceParm.Bind(Initializer.ParameterMap, TEXT("NumIndicesPerInstance"));
	InstanceArgsParm.Bind(Initializer.ParameterMap, TEXT("InstanceArgs"));
	CountBeforeCullingParm.Bind(Initializer.ParameterMap, TEXT("CountBeforeCulling"));
}

void FDrawIndirectArgsGenCS::SetOutput(FRHICommandList& RHICmdList, FRHIUnorderedAccessView* DrawIndirectArgsUAV)
{
	FRHIComputeShader* ComputeShaderRHI = RHICmdList.GetBoundComputeShader();
	if (DrawIndirectArgsParm.IsUAVBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, DrawIndirectArgsParm.GetUAVIndex(), DrawIndirectArgsUAV);
	}
}

void FDrawIndirectArgsGenCS::SetParameters(FRHICommandList& RHICmdList, int32 NumIndicesPerInstance, FRHIUnorderedAccessView* InstanceCountUAV, FRHIUnorderedAccessView* CountBeforeCullingUAV)
{
	FRHIComputeShader* ComputeShaderRHI = RHICmdList.GetBoundComputeShader();

	SetShaderValue(RHICmdList, ComputeShaderRHI, NumIndicesPerInstanceParm, NumIndicesPerInstance);
	//SetShaderValue(RHICmdList, ComputeShaderRHI, InstanceCountParm, 10);
	if (InstanceArgsParm.IsUAVBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, InstanceArgsParm.GetUAVIndex(), InstanceCountUAV);
	}

	if (CountBeforeCullingParm.IsUAVBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, CountBeforeCullingParm.GetUAVIndex(), CountBeforeCullingUAV);
	}


}

void FDrawIndirectArgsGenCS::UnbindBuffers(FRHICommandList& RHICmdList)
{
	FRHIComputeShader* ComputeShaderRHI = RHICmdList.GetBoundComputeShader();
	
	if (DrawIndirectArgsParm.IsUAVBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, DrawIndirectArgsParm.GetUAVIndex(), nullptr);
	}

	if(InstanceArgsParm.IsUAVBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, InstanceArgsParm.GetUAVIndex(), nullptr);
	}

	if(CountBeforeCullingParm.IsUAVBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, CountBeforeCullingParm.GetUAVIndex(), nullptr);
	}
}

void FInstanceBufferGenCS::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters,
	FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	//Currently hardcode 1 for thread count.
	OutEnvironment.SetDefine(TEXT("BUFFER_GEN_THREAD_COUNT"), 256);
}

FInstanceBufferGenCS::FInstanceBufferGenCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
: FGlobalShader(Initializer)
{
	InstanceOriginBufferParm.Bind(Initializer.ParameterMap, TEXT("InstanceOriginBuffer"));
	InstanceTransformBufferParm.Bind(Initializer.ParameterMap, TEXT("InstanceTransformBuffer"));
	InstanceArgsParm.Bind(Initializer.ParameterMap, TEXT("CSInstanceArgs"));
	InstanceCountParm.Bind(Initializer.ParameterMap, TEXT("GenCSInstanceCount"));
	WorldToClipParm.Bind(Initializer.ParameterMap, TEXT("WorldToClip"));
	PossibleInstancePosParm.Bind(Initializer.ParameterMap, TEXT("PossibleInstancePos"));
	ViewLocationParm.Bind(Initializer.ParameterMap, TEXT("ViewLocation"));
	ComponentLocationParm.Bind(Initializer.ParameterMap, TEXT("ComponentLocation"));

	DepthTextureParm.Bind(Initializer.ParameterMap, TEXT("DepthTexture"));
	DepthTexSamplerParm.Bind(Initializer.ParameterMap, TEXT("DepthTexSampler"));
}

void FInstanceBufferGenCS::SetOutput(FRHICommandList& RHICmdList, FRHIUnorderedAccessView* OutInstanceOriginUAV,
                                     FRHIUnorderedAccessView* OutInstanceTransformUAV, FRHIUnorderedAccessView* InstanceCountUAV)
{
	FRHIComputeShader* ComputeShaderRHI = RHICmdList.GetBoundComputeShader();
	if (InstanceOriginBufferParm.IsUAVBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, InstanceOriginBufferParm.GetUAVIndex(), OutInstanceOriginUAV);
	}
	if (InstanceTransformBufferParm.IsUAVBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, InstanceTransformBufferParm.GetUAVIndex(), OutInstanceTransformUAV);
	}
	if (InstanceArgsParm.IsUAVBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, InstanceArgsParm.GetUAVIndex(), InstanceCountUAV);
	}
}

void FInstanceBufferGenCS::SetParameters(FRHICommandList& RHICmdList, FRHIUnorderedAccessView* InstanceCountUAV,
	FRHIShaderResourceView* PossiblePosSRV, const TArray<const FSceneView*>& Views, const FVector ComponentLoc,
	  FRHITexture2D* DepthResource)
{
	FRHIComputeShader* ComputeShaderRHI = RHICmdList.GetBoundComputeShader();
	if(InstanceCountParm.IsBound())
		RHICmdList.SetUAVParameter(ComputeShaderRHI, InstanceCountParm.GetUAVIndex(), InstanceCountUAV);
	
	const FSceneView* View = Views[0];
	// UE_LOG(LogTemp, Warning,TEXT("View Location : %s \n"),  *View->ViewLocation.ToString())
	//
	// UE_LOG(LogTemp, Warning,TEXT("View Projection Matrix : %s \n"),  *View->ViewMatrices.GetViewProjectionMatrix().ToString())
	FMatrix WorldToClip = View->ViewMatrices.GetViewProjectionMatrix();
	
	SetShaderValue(RHICmdList, ComputeShaderRHI, WorldToClipParm, WorldToClip);

	FVector4 ViewLoc = View->ViewMatrices.GetViewOrigin();

	SetShaderValue(RHICmdList, ComputeShaderRHI, ViewLocationParm, View->ViewLocation);
	SetShaderValue(RHICmdList, ComputeShaderRHI, ComponentLocationParm, ComponentLoc);

	//SceneOcclusion.cpp
	FRHISamplerState* CSSamplerState = TStaticSamplerState<SF_Point, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();

	SetTextureParameter(RHICmdList, ComputeShaderRHI, DepthTextureParm, DepthTexSamplerParm, CSSamplerState
		, DepthResource);
	
	RHICmdList.SetShaderResourceViewParameter(ComputeShaderRHI, PossibleInstancePosParm.GetBaseIndex(), PossiblePosSRV);
	
}

void FInstanceBufferGenCS::UnbindBuffers(FRHICommandList& RHICmdList)
{
	FRHIComputeShader* ComputeShaderRHI = RHICmdList.GetBoundComputeShader();
	
	if (InstanceOriginBufferParm.IsBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, InstanceOriginBufferParm.GetUAVIndex(), nullptr);
	}
	if (InstanceTransformBufferParm.IsBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, InstanceTransformBufferParm.GetUAVIndex(), nullptr);
	}
	if (InstanceArgsParm.IsBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, InstanceArgsParm.GetUAVIndex(), nullptr);
	}
	
	if (InstanceCountParm.IsBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, InstanceCountParm.GetUAVIndex(), nullptr);
	}
	
}

FComputeShaderManager::FComputeShaderManager()
{
	GenInfo = FArgsGenInfo(0, 0, 0);
}

FComputeShaderManager::~FComputeShaderManager()
{
	ReleaseRHI();
}

void FComputeShaderManager::InitRHI()
{
}

void FComputeShaderManager::ReleaseRHI()
{
	DrawIndirectBuffer.Release();
	InstanceTransformBuffer.Release();
	InstanceOriginBuffer.Release();
	InstanceArgsBuffer.Release();

	PreCullingCountBuffer.Release();
	DataGenPositionBuffer.Release();
}

uint32 FComputeShaderManager::AddDrawIndirect(uint32 NumIndicesPerInstance, uint32 InInstanceCount,
                                              uint32 StartIndexLocation)
{
	//const FArgsGenInfo Info(NumIndicesPerInstance, StartIndexLocation);
	GenInfo.NumIndicesPerInstance = NumIndicesPerInstance;
	//Hardcode for now.
	GenInfo.InstanceCount = InInstanceCount;
	//Currently no use.
	GenInfo.StartIndexLocation = StartIndexLocation;

	if(OldInstanceCount!= InInstanceCount)
	{
		bFlag = false;
		OldInstanceCount = InInstanceCount;
	}
	
	TResourceArray<uint32> InitData;
	InitData.Add(NumIndicesPerInstance);
	InitData.Add(InInstanceCount);
	InitData.Add(StartIndexLocation);
	InitData.Add(0);
	InitData.Add(0);
	if(!bFlag)
	{
		DrawIndirectBuffer.Initialize(sizeof(uint32), 5, PF_R32_UINT,
			BUF_Static | BUF_DrawIndirect, TEXT("MyCustomDrawIndirectArgs"), &InitData);
		bFlag = true;
	}
	return INDEX_NONE;
}

void FComputeShaderManager::InitInstanceBufferGenResource()
{
	TResourceArray<FVector4> OriginInitData;
	OriginInitData.SetNumZeroed(MAX_COMPUTEINSTANCE_COUNT);
	InstanceOriginBuffer.Initialize(sizeof(FVector4), MAX_COMPUTEINSTANCE_COUNT,  EPixelFormat::PF_A32B32G32R32F, 
			BUF_Static | BUF_VertexBuffer | BUF_UnorderedAccess, TEXT("MyCustomOriginBuffer"), &OriginInitData);

	TResourceArray<FVector4> TransformInitData;
	TransformInitData.SetNumZeroed(MAX_COMPUTEINSTANCE_COUNT * 3);
	InstanceTransformBuffer.Initialize(sizeof(FVector4), MAX_COMPUTEINSTANCE_COUNT * 3,  EPixelFormat::PF_A32B32G32R32F,
			BUF_Static | BUF_VertexBuffer | BUF_UnorderedAccess, TEXT("MyCustomTransformBuffer"), &TransformInitData);

	TResourceArray<uint32> CountResource;
	CountResource.SetNumZeroed(InstanceArgsCount);
	InstanceArgsBuffer.Initialize(sizeof(uint32), InstanceArgsCount, EPixelFormat::PF_R32_UINT,
			BUF_Static | BUF_UnorderedAccess, TEXT("MyCustomInstanceCount"), &CountResource);

	TResourceArray<uint32> InstanceDataArgsResource;
	InstanceDataArgsResource.SetNumZeroed(1);
	PreCullingCountBuffer.Initialize(sizeof(uint32), 1, EPixelFormat::PF_R32_UINT,
			BUF_Static | BUF_UnorderedAccess, TEXT("MyInstanceDataArgsResource"), &InstanceDataArgsResource);

	TResourceArray<FVector4> DataGenPositionResource;
	DataGenPositionResource.SetNumZeroed(MAX_COMPUTEINSTANCE_COUNT);
	DataGenPositionBuffer.Initialize(sizeof(FVector4), MAX_COMPUTEINSTANCE_COUNT, EPixelFormat::PF_A32B32G32R32F,
			BUF_Static | BUF_UnorderedAccess, TEXT("MyDataGenPositionResource"), &DataGenPositionResource);
	
}

void FComputeShaderManager::IssueInstanceBufferGenTask(FRHICommandList& RHICmdList,
                                                       ERHIFeatureLevel::Type FeatureLevel
                                                       , const TArray<const FSceneView*>& Views,
                                                       const FVector CompLocation
                                                       , FRHITexture2D* DepthResource)
{
	TShaderMapRef<FInstanceBufferGenCS> InstanceBufferGenCS(GetGlobalShaderMap(FeatureLevel));
	RHICmdList.SetComputeShader(InstanceBufferGenCS.GetComputeShader());
	InstanceBufferGenCS->SetOutput(RHICmdList, InstanceOriginBuffer.UAV,
		InstanceTransformBuffer.UAV, InstanceArgsBuffer.UAV);
	InstanceBufferGenCS->SetParameters(RHICmdList, PreCullingCountBuffer.UAV, DataGenPositionBuffer.SRV,  Views, CompLocation, DepthResource);
	RHICmdList.Transition(FRHITransitionInfo(PreCullingCountBuffer.UAV, ERHIAccess::Unknown, ERHIAccess::UAVCompute));
	RHICmdList.Transition(FRHITransitionInfo(DataGenPositionBuffer.UAV, ERHIAccess::Unknown, ERHIAccess::UAVCompute));
	DispatchComputeShader(RHICmdList, InstanceBufferGenCS.GetShader(), 1, 1, 1);
	InstanceBufferGenCS->UnbindBuffers(RHICmdList);
}

void FComputeShaderManager::IssueInstanceDataGenTask(FRHICommandList& RHICmdList, ERHIFeatureLevel::Type FeatureLevel, float InThreshold, FTexture* InInputTexture)
{
	TShaderMapRef<FInstanceDataGenCS> InstanceDataGenCS(GetGlobalShaderMap(FeatureLevel));
	RHICmdList.SetComputeShader(InstanceDataGenCS.GetComputeShader());
	InstanceDataGenCS->SetOutput(RHICmdList, DataGenPositionBuffer.UAV,PreCullingCountBuffer.UAV);
	InstanceDataGenCS->SetParameters(RHICmdList, InThreshold, InInputTexture);
	RHICmdList.Transition(FRHITransitionInfo(PreCullingCountBuffer.UAV, ERHIAccess::Unknown, ERHIAccess::UAVCompute));
	RHICmdList.Transition(FRHITransitionInfo(DataGenPositionBuffer.UAV, ERHIAccess::Unknown, ERHIAccess::UAVCompute));
	DispatchComputeShader(RHICmdList, InstanceDataGenCS.GetShader(), 1, 1, 1);
	InstanceDataGenCS->UnbindBuffers(RHICmdList);
}


void FComputeShaderManager::IssueDrawIndirectTask(FRHICommandList& RHICmdList, ERHIFeatureLevel::Type FeatureLevel)
{
	TShaderMapRef<FDrawIndirectArgsGenCS> DrawIndirectArgsGenCS(GetGlobalShaderMap(FeatureLevel));
	RHICmdList.SetComputeShader(DrawIndirectArgsGenCS.GetComputeShader());
	DrawIndirectArgsGenCS->SetOutput(RHICmdList, DrawIndirectBuffer.UAV);
	DrawIndirectArgsGenCS->SetParameters(RHICmdList, GenInfo.NumIndicesPerInstance, InstanceArgsBuffer.UAV, PreCullingCountBuffer.UAV);
	RHICmdList.Transition(FRHITransitionInfo(DrawIndirectBuffer.UAV, ERHIAccess::Unknown, ERHIAccess::UAVCompute));
	DispatchComputeShader(RHICmdList, DrawIndirectArgsGenCS.GetShader(), 1, 1, 1);
	DrawIndirectArgsGenCS->UnbindBuffers(RHICmdList);
}

void FComputeShaderManager::TransitBufferToIndirectArgs(FRHICommandList& RHICmdList)
{
	RHICmdList.Transition(FRHITransitionInfo(DrawIndirectBuffer.UAV, ERHIAccess::Unknown, ERHIAccess::IndirectArgs));
}
