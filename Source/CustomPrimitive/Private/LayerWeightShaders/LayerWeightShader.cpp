#include "LayerWeightShader.h"
IMPLEMENT_GLOBAL_SHADER(FLayerWeightEditorShaderCS, "/CustomPrimitiveCSDir/LayerWeightEditorShaderCS.usf", "LWKernel", SF_Compute);

FLayerWeightEditorShaderCS::FLayerWeightEditorShaderCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
: FGlobalShader(Initializer)
{
	WorldToClipParm.Bind(Initializer.ParameterMap, TEXT("WorldToClipFloat"));
	TempOutput.Bind(Initializer.ParameterMap, TEXT("TempOutput"));
}

void FLayerWeightEditorShaderCS::SetParameters(FRHICommandList& RHICmdList)
{
	FRHIComputeShader* ComputeShaderRHI = RHICmdList.GetBoundComputeShader();

	SetShaderValue(RHICmdList, ComputeShaderRHI, WorldToClipParm, 0);
	
	
}

void FLayerWeightEditorShaderCS::SetOutput(FRHICommandList& RHICmdList)
{
	FRHIComputeShader* ComputeShaderRHI = RHICmdList.GetBoundComputeShader();
	
	/** Buffer holds instance transform. */
	FRWBuffer InstanceOriginBuffer;
	TResourceArray<float> OriginInitData;
	OriginInitData.SetNumZeroed(1);
	InstanceOriginBuffer.Initialize(sizeof(float), 1,  EPixelFormat::PF_A32B32G32R32F, 
	BUF_Static | BUF_UnorderedAccess, TEXT("LW_EDITOR_InstanceOriginBuffer"), &OriginInitData);
	if (TempOutput.IsUAVBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, TempOutput.GetUAVIndex(), InstanceOriginBuffer.UAV);
	}

}


void FLayerWeightEditorShaderCS::UnbindBuffers(FRHICommandList& RHICmdList)
{
	FRHIComputeShader* ComputeShaderRHI = RHICmdList.GetBoundComputeShader();
	if (TempOutput.IsUAVBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, TempOutput.GetUAVIndex(), nullptr);
	}
}
// 	