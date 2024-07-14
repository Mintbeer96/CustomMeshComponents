// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComputeShaderManager.h"
#include "MeshDrawShaderBindings.h"
#include "MeshMaterialShader.h"
#include "PrimitiveSceneProxy.h"
#include "Components/ActorComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/MapBuildDataRegistry.h"

#include "CustomPrimitiveComponent.generated.h"
class FCustomVertexFactoryParameters;
// class FCustomVertexBuffer;

class FCustomVertexFactory;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CUSTOMPRIMITIVE_API UCustomPrimitiveComponent : public UMeshComponent
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere)
	UTexture2D* DepthTextureDebug;
	
public:
	/**	If true, each tick the component will render its point cloud */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyMesh")
	bool bIsVisible;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyMesh")
	bool bIsDrawIndirect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyMesh")
	int32 InstanceCount = 1;
	
	/** Changes the visibility setting */
	UFUNCTION(BlueprintCallable, Category = "MyMesh")
	void SetIsVisible(bool bNewVisibility);
	
	UPROPERTY(EditAnywhere, Category = "MyMesh")
	UStaticMesh* StaticMesh;
	
	/** The material to render with */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyMesh")
	UMaterialInterface* Material;

	/** Uniquely identifies this component's built map data. */
	UPROPERTY()
	FGuid MapBuildDataId;
	
	UPROPERTY()
	FGuid StateId;

	UPROPERTY(EditAnywhere, Category = "MyMesh")
	UTexture2D* TextureInput;

	UPROPERTY(EditAnywhere, Category = "MyMesh")
	UTextureRenderTarget2D* RT;

	const FMeshMapBuildData* GetMeshMapBuildData() const;

private:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void PostInitProperties() override;
	virtual void PostLoad() override;
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual int32 GetNumMaterials() const override { return 1; }
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	virtual void GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials = false) const override;

	/** World space bounds of the point cloud */
	UPROPERTY()
	FBoxSphereBounds WorldBounds;


};


// /**
//  * Uniform buffer to hold parameters for point cloud rendering
//  */
BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT( FCustomVertexFactoryParameters, )
	SHADER_PARAMETER(FIntVector4, VertexFetch_Parameters)
	SHADER_PARAMETER_SRV(Buffer<float2>, VertexFetch_TexCoordBuffer)
	SHADER_PARAMETER_SRV(Buffer<float4>, VertexFetch_InstanceOriginBuffer)
	SHADER_PARAMETER_SRV(Buffer<float4>, VertexFetch_InstanceTransformBuffer)
END_GLOBAL_SHADER_PARAMETER_STRUCT()
typedef TUniformBufferRef<FCustomVertexFactoryParameters> FCustomVertexFactoryBufferRef;

class CUSTOMPRIMITIVE_API FCustomVertexFactory: public FVertexFactory
{
	DECLARE_VERTEX_FACTORY_TYPE(FCustomVertexFactory);

public:
	FCustomVertexFactory(ERHIFeatureLevel::Type InFeatureLevel, const char* InDebugName)
		: FVertexFactory(InFeatureLevel)
	{
	}

	/**
	 * Constructs render resources for this vertex factory.
	 */
	virtual void InitRHI() override;

	/**
	 * Release render resources for this vertex factory.
	 */
	virtual void ReleaseRHI() override;

	/**
	 * Should we cache the material's shadertype on this platform with this vertex factory?
	 */
	static bool ShouldCompilePermutation(const FVertexFactoryShaderPermutationParameters& Parameters);
	
	static void ModifyCompilationEnvironment( const FVertexFactoryShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment );

	inline const FUniformBufferRHIRef GetVertexFactoryUniformBuffer() const
	{
		return UniformBuffer;
	}

	/**
	 * Set parameters for this vertex factory instance.
	 */
	void SetParameters(const FCustomVertexFactoryParameters& InUniformParameters);
	
	inline const FUniformBufferRHIRef GetPointCloudVertexFactoryUniformBuffer() const
	{
		return UniformBuffer;
	}



	FRHIUniformBuffer* GetUniformBuffer() const
	{
		return UniformBuffer.GetReference();
	}

private:
	/** Buffers to read from */
	FUniformBufferRHIRef UniformBuffer;

public:
	FVertexBuffer* VertexBuffer = nullptr;
	FVertexBuffer* TangentVertexBuffer = nullptr;
	// FVertexBuffer* TexCoordBuffer = nullptr;


	int32 NumCustomDataFloats = 0;
};



class CUSTOMPRIMITIVE_API FCustomPrimitiveSceneProxy:public FPrimitiveSceneProxy
{
public:
	FCustomPrimitiveSceneProxy(UCustomPrimitiveComponent* Component);
	virtual ~FCustomPrimitiveSceneProxy();

	SIZE_T GetTypeHash() const override;

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override;
	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;

	virtual uint32 GetMemoryFootprint(void) const override
	{
		return(sizeof(*this) + GetAllocatedSize());
	}

	uint32 GetAllocatedSize(void) const
	{
		return FPrimitiveSceneProxy::GetAllocatedSize();
	}
	mutable bool bAlreadySubmit = false;
	/**
	 *	Called when the rendering thread adds the proxy to the scene.
	 *	This function allows for generating renderer-side resources.
	 *	Called in the rendering thread.
	 */
	virtual void CreateRenderThreadResources() override;

	UCustomPrimitiveComponent* Component;
	
	UFUNCTION(CallInEditor, Category = asdfasdfsaf)
	void RequestUpdateBufferInSubSystem();

	class FCustomPrimitiveLCI final : public FLightCacheInterface
	{
	public:
		/** Initialization constructor. */
		FCustomPrimitiveLCI(const UCustomPrimitiveComponent* InComponent)
			: FLightCacheInterface()
		{
			const FMeshMapBuildData* MapBuildData = InComponent->GetMeshMapBuildData();

			if (MapBuildData)
			{
				SetLightMap(MapBuildData->LightMap);
				SetShadowMap(MapBuildData->ShadowMap);
				SetResourceCluster(MapBuildData->ResourceCluster);
				IrrelevantLights = MapBuildData->IrrelevantLights;
			}
		}

		// FLightCacheInterface
		virtual FLightInteraction GetInteraction(const FLightSceneProxy* LightSceneProxy) const override;

	private:
		TArray<FGuid> IrrelevantLights;
	};

	//光照相关接口，需要重写，不然没有光照效果。
	virtual void GetLightRelevance(const FLightSceneProxy* LightSceneProxy, bool& bDynamic, bool& bRelevant, bool& bLightMapped, bool& bShadowMapped) const override;

	
	void UpdateStaticMesh(UStaticMesh* InStaticMesh);

	FComputeShaderManager* GetComputeShaderManager(){return ComputeShaderManager;};
private:
	
	bool bIsVisible;

	bool bIsDrawIndirect;
	
	/** The material from the component to render with */
	UMaterialInterface* Material;
	
	UStaticMesh* StaticMesh;
	
	FMaterialRelevance MaterialRelevance;
	FCustomVertexFactory VertexFactory;

	// FLightCacheInterface
	TUniquePtr<FCustomPrimitiveLCI> ComponentLightInfo;

	int32 InstanceCount = 0;

	UTexture2D* Tex;

	UTextureRenderTarget2D* RT;
	
protected:
	FComputeShaderManager* ComputeShaderManager = nullptr;

	FShaderResourceViewRHIRef OriginSRV;
	FShaderResourceViewRHIRef TransformSRV;
};



/**
 * Shader parameters for the vector field visualization vertex factory.
 */
class FCustomVertexFactoryShaderParameters :
	public FVertexFactoryShaderParameters
{
	DECLARE_INLINE_TYPE_LAYOUT(FCustomVertexFactoryShaderParameters, NonVirtual);
public:
	void Bind(const FShaderParameterMap& ParameterMap)
	{
		//VertexFetch_InstanceOriginBufferParameter.Bind(ParameterMap, TEXT("VertexFetch_InstanceOriginBuffer"));
		//VertexFetch_InstanceTransformBufferParameter.Bind(ParameterMap, TEXT("VertexFetch_InstanceTransformBuffer"));
		// VertexFetch_InstanceLightmapBufferParameter.Bind(ParameterMap, TEXT("VertexFetch_InstanceLightmapBuffer"));
	}

	void GetElementShaderBindings(
		const class FSceneInterface* Scene,
		const class FSceneView* View,
		const class FMeshMaterialShader* Shader,
		const EVertexInputStreamType InputStreamType,
		ERHIFeatureLevel::Type FeatureLevel,
		const class FVertexFactory* InVertexFactory,
		const struct FMeshBatchElement& BatchElement,
		class FMeshDrawSingleShaderBindings& ShaderBindings,
		FVertexInputStreamArray& VertexStreams) const
	{		
		FCustomVertexFactory* VertexFactory = (FCustomVertexFactory*)InVertexFactory;

		ShaderBindings.Add(Shader->GetUniformBufferParameter<FCustomVertexFactoryParameters>(), VertexFactory->GetVertexFactoryUniformBuffer());

		//ShaderBindings.Add(VertexFetch_InstanceLightmapBufferParameter, VertexFactory->GetInstanceLightmapSRV());

	}

private:
	//LAYOUT_FIELD(FShaderResourceParameter, VertexFetch_InstanceOriginBufferParameter);
	//LAYOUT_FIELD(FShaderResourceParameter, VertexFetch_InstanceTransformBufferParameter);
	// LAYOUT_FIELD(FShaderResourceParameter, VertexFetch_InstanceLightmapBufferParameter);
};



