#pragma once

#include "CoreMinimal.h"
#include "Shader.h"
#include "GlobalShader.h"
#include "Engine/EngineTypes.h"

struct FLayerWeightShaderPermutationParameters : public FShaderPermutationParameters
{
	//const FNiagaraShaderScript* Script;

	// FNiagaraShaderPermutationParameters(EShaderPlatform InPlatform, const FNiagaraShaderScript* InScript)
	FLayerWeightShaderPermutationParameters(EShaderPlatform InPlatform)
		: FShaderPermutationParameters(InPlatform)
	{}
};

/**
 * A shader meta type for niagara-linked shaders.
 */
class FLayerWeightShaderType : public FShaderType
{

	
}; 