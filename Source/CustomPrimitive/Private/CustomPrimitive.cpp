// Copyright Epic Games, Inc. All Rights Reserved.

#include "CustomPrimitive.h"

#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FCustomPrimitiveModule"

void FCustomPrimitiveModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	const FString VSDirectory = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("CustomPrimitive"))->GetBaseDir(), TEXT("Shaders/Private/VertexShaders"));
	AddShaderSourceDirectoryMapping("/CustomPrimitiveVSDir", VSDirectory);
	
	const FString CSDirectory = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("CustomPrimitive"))->GetBaseDir(), TEXT("Shaders/Private/ComputeShaders"));
	AddShaderSourceDirectoryMapping("/CustomPrimitiveCSDir", CSDirectory);
}

void FCustomPrimitiveModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCustomPrimitiveModule, CustomPrimitive)