#pragma once
#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "ShaderPermutation.h"

class CUSTOMPRIMITIVE_API FLayerWeightEditorShaderCS: public FGlobalShader
{
	//Declare this class as a global shader
	DECLARE_GLOBAL_SHADER(FLayerWeightEditorShaderCS);
public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return RHISupportsComputeShaders(Parameters.Platform);
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters,
	                                         FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}

	//These two is needed.
	FLayerWeightEditorShaderCS() {}
	FLayerWeightEditorShaderCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	void SetParameters(FRHICommandList& RHICmdList);
	void SetOutput(FRHICommandList& RHICmdList);
	
	void UnbindBuffers(FRHICommandList& RHICmdList);
	
	
protected:
	LAYOUT_FIELD(FShaderParameter, WorldToClipParm)
	LAYOUT_FIELD(FRWShaderParameter, TempOutput)
};
