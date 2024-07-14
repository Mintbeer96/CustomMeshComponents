// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPrimitiveComponent.h"

#include "CommonRenderResources.h"
#include "CustomPrimitiveSubsystem.h"
#include "Editor.h"
#include "LevelEditorViewport.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Rendering/StaticLightingSystemInterface.h"
IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FCustomVertexFactoryParameters, "CustomVF");
IMPLEMENT_VERTEX_FACTORY_PARAMETER_TYPE(FCustomVertexFactory, SF_Vertex, FCustomVertexFactoryShaderParameters);
IMPLEMENT_VERTEX_FACTORY_TYPE(FCustomVertexFactory, "/CustomPrimitiveVSDir/CustomVertexFactory.ush", true, false, true, false, true);
//IMPLEMENT_VERTEX_FACTORY_PARAMETER_TYPE(FCustomVertexFactory, SF_Vertex, FCustomVertexFactoryShaderParameters);
// IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FCustomVertexFactoryParameters, "CustomVF");

// Sets default values for this component's properties
UCustomPrimitiveComponent::UCustomPrimitiveComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bTickInEditor = true;
	// ...
	// Load our material we use for rendering
	
	Material = UMaterial::GetDefaultMaterial(MD_Surface);
	if (Material == nullptr)
	{
		Material = GEngine->WireframeMaterial;
	}
	//
	// PointCloud.AddZeroed(6);
	// for(int i = 0;i<6;i++)
	// {
	// 	PointCloud[i]+=FVector(i*100, 0, 0);
	// }
}

void UCustomPrimitiveComponent::PostInitProperties()
{
	Super::PostInitProperties();
	// Create a new guid in case this is a newly created component
	// If not, this guid will be overwritten when serialized
	FPlatformMisc::CreateGuid(StateId);

	// Initialize MapBuildDataId to something unique.
	MapBuildDataId = FGuid::NewGuid();


}


void UCustomPrimitiveComponent::PostLoad()
{
	Super::PostLoad();
	
}

void UCustomPrimitiveComponent::SetIsVisible(bool bNewVisibility)
{
	if (bNewVisibility != bIsVisible)
	{
		bIsVisible = bNewVisibility;
		
		MarkRenderStateDirty();
	}
}

void UCustomPrimitiveComponent::TickComponent(float DeltaTime, ELevelTick TickType,
											   FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(StaticMesh)
	{
		FCustomPrimitiveSceneProxy* TempSceneProxy = (FCustomPrimitiveSceneProxy*)SceneProxy;
		if(TempSceneProxy)
		{
			UCustomPrimitiveSubsystem* Subsystem = GetWorld()->GetSubsystem<UCustomPrimitiveSubsystem>();

			if (Subsystem)
			{
				Subsystem->Components.AddUnique(this);
			}
		}
	}
}


FPrimitiveSceneProxy* UCustomPrimitiveComponent::CreateSceneProxy()
{
	//SCOPE_CYCLE_COUNTER(STAT_PointCloud_CreateSceneProxy);

	// Use the fast version if supported
	if (RHISupportsManualVertexFetch(GMaxRHIShaderPlatform))
	{
		SceneProxy = new FCustomPrimitiveSceneProxy(this);
		return SceneProxy;
	}
	return nullptr;
}

void UCustomPrimitiveComponent::GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials,
	bool bGetDebugMaterials) const
{
	if (Material != nullptr)
	{
		OutMaterials.Add(Material);
	}
}

FBoxSphereBounds UCustomPrimitiveComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	return FBoxSphereBounds(FBox(FVector(-10000,-10000,-10000), FVector(10000,10000,10000)));
}


// void UCustomPrimitiveComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
// {
// 	if(PropertyChangedEvent.GetPropertyName()==GET_MEMBER_NAME_CHECKED(UCustomPrimitiveComponent, StaticMesh))
// 	{
// 		if(StaticMesh)
// 		{
// 			if(SceneProxy)
// 			{
// 				FCustomPrimitiveSceneProxy* TempSceneProxy = (FCustomPrimitiveSceneProxy*)SceneProxy;
// 				TempSceneProxy->UpdateStaticMesh(StaticMesh);
// 				MarkRenderStateDirty();
// 			}
// 		}
// 	}
// 	
// }


const FMeshMapBuildData* UCustomPrimitiveComponent::GetMeshMapBuildData() const
{
	AActor* Owner = GetOwner();

	if (Owner)
	{
		ULevel* OwnerLevel = Owner->GetLevel();

#if WITH_EDITOR
		if (FStaticLightingSystemInterface::GetPrimitiveMeshMapBuildData(this))
		{
			return FStaticLightingSystemInterface::GetPrimitiveMeshMapBuildData(this);
		}
#endif

		if (OwnerLevel && OwnerLevel->OwningWorld)
		{
			ULevel* ActiveLightingScenario = OwnerLevel->OwningWorld->GetActiveLightingScenario();
			UMapBuildDataRegistry* MapBuildData = NULL;

			if (ActiveLightingScenario && ActiveLightingScenario->MapBuildData)
			{
				MapBuildData = ActiveLightingScenario->MapBuildData;
			}
			else if (OwnerLevel->MapBuildData)
			{
				MapBuildData = OwnerLevel->MapBuildData;
			}

			if (MapBuildData)
			{
				return MapBuildData->GetMeshBuildData(MapBuildDataId);
			}
		}
	}
	
	return NULL;
}


FPrimitiveViewRelevance FCustomPrimitiveSceneProxy::GetViewRelevance(const FSceneView* View) const
{
	FPrimitiveViewRelevance Result;
	Result.bDrawRelevance = IsShown(View) && bIsVisible;
	Result.bShadowRelevance = IsShadowCast(View);
	Result.bDynamicRelevance = true;
	Result.bRenderInMainPass = true;
	Result.bUsesLightingChannels = false;
	Result.bRenderCustomDepth = false;

	MaterialRelevance.SetPrimitiveViewRelevance(Result);
	return Result;
}

void FCustomPrimitiveSceneProxy::CreateRenderThreadResources()
{
	VertexFactory.InitResource();
	
	FCustomVertexFactoryParameters UniformParameters;
	if(StaticMesh)
	{
		int32 ColorIndexMask = 0;
		const int32 NumTexCoords = StaticMesh->RenderData->LODResources[0].GetNumTexCoords();
		const int32 LightMapCoordinateIndex =  StaticMesh->LightMapCoordinateIndex;
		const int32 EffectiveBaseVertexIndex = 0;
		//const int32 EffectivePreSkinBaseVertexIndex = RHISupportsAbsoluteVertexID(GMaxRHIShaderPlatform) ? 0 : PreSkinBaseVertexIndex;
	
		UniformParameters.VertexFetch_Parameters = {ColorIndexMask, NumTexCoords, LightMapCoordinateIndex, EffectiveBaseVertexIndex};
		UniformParameters.VertexFetch_TexCoordBuffer = StaticMesh->RenderData->LODResources[0].VertexBuffers.StaticMeshVertexBuffer.GetTexCoordsSRV();

	}
	else
	{
		UniformParameters.VertexFetch_Parameters = {0, 0, 0, 0};
		UniformParameters.VertexFetch_TexCoordBuffer = GNullColorVertexBuffer.VertexBufferSRV;
		
	}
	
	//根据Component给进来的参数 生成一个srv供Origin和Transform使用。
	TResourceArray<FVector4> InstanceOriginResources;
	TResourceArray<FVector4> InstanceTransformResources;
	for(int i = 0;i<InstanceCount;i++)
	{
		InstanceOriginResources.Add(FVector4(0, i*150, 0, 1));
		InstanceTransformResources.Add(FVector4(1, 0, 0, 0));
		InstanceTransformResources.Add(FVector4(0, 1, 0, 0));
		InstanceTransformResources.Add(FVector4(0, 0, 1, 0));
	}
		
	FRHIResourceCreateInfo OriginInfo(&InstanceOriginResources);
	auto OutOriginBufferRHI = RHICreateVertexBuffer(InstanceOriginResources.GetResourceDataSize(), BUF_Static | BUF_ShaderResource, OriginInfo);
	OriginSRV = RHICreateShaderResourceView(OutOriginBufferRHI, 16, PF_A32B32G32R32F);
	
	
	FRHIResourceCreateInfo TransformInfo(&InstanceTransformResources);
	auto OutTransformBufferRHI = RHICreateVertexBuffer(InstanceTransformResources.GetResourceDataSize(), BUF_Static | BUF_ShaderResource, TransformInfo);
	TransformSRV = RHICreateShaderResourceView(OutTransformBufferRHI, 16, PF_A32B32G32R32F);
	if(bIsDrawIndirect)
	{
		UCustomPrimitiveSubsystem* Subsystem = GetScene().GetWorld()->GetSubsystem<UCustomPrimitiveSubsystem>();
		if(Subsystem)
		{
			UniformParameters.VertexFetch_InstanceOriginBuffer = Subsystem->GetComputeShaderManager()->GetInstanceOriginBufferSRV();
			UniformParameters.VertexFetch_InstanceTransformBuffer = Subsystem->GetComputeShaderManager()->GetInstanceTransformBufferSRV();
		}
		else{
			UniformParameters.VertexFetch_InstanceOriginBuffer = ComputeShaderManager->GetInstanceOriginBufferSRV();
			UniformParameters.VertexFetch_InstanceTransformBuffer = ComputeShaderManager->GetInstanceTransformBufferSRV();
		}
	}
	else
	{
		UniformParameters.VertexFetch_InstanceOriginBuffer = OriginSRV;
		UniformParameters.VertexFetch_InstanceTransformBuffer = TransformSRV;
	}
	VertexFactory.SetParameters(UniformParameters);
}

FLightInteraction FCustomPrimitiveSceneProxy::FCustomPrimitiveLCI::GetInteraction(
	const FLightSceneProxy* LightSceneProxy) const
{
	// ask base class
	ELightInteractionType LightInteraction = GetStaticInteraction(LightSceneProxy, IrrelevantLights);

	if (LightInteraction != LIT_MAX)
	{
		return FLightInteraction(LightInteraction);
	}

	// Use dynamic lighting if the light doesn't have static lighting.
	return FLightInteraction::Dynamic();
	
}


void FCustomPrimitiveSceneProxy::GetLightRelevance(const FLightSceneProxy* LightSceneProxy, bool& bDynamic,
	bool& bRelevant, bool& bLightMapped, bool& bShadowMapped) const
{
	// Attach the light to the primitive's static meshes.
	bDynamic = true;
	bRelevant = false;
	bLightMapped = true;
	bShadowMapped = true;

	
	if (ComponentLightInfo)
	{
		ELightInteractionType InteractionType = ComponentLightInfo->GetInteraction(LightSceneProxy).GetType();

		if (InteractionType != LIT_CachedIrrelevant)
		{
			bRelevant = true;
		}

		if (InteractionType != LIT_CachedLightMap && InteractionType != LIT_CachedIrrelevant)
		{
			bLightMapped = false;
		}

		if (InteractionType != LIT_Dynamic)
		{
			bDynamic = false;
		}

		if (InteractionType != LIT_CachedSignedDistanceFieldShadowMap2D)
		{
			bShadowMapped = false;
		}
	}
	else
	{
		bRelevant = true;
		bLightMapped = false;
		bShadowMapped = false;
	}

}

void FCustomPrimitiveSceneProxy::UpdateStaticMesh(UStaticMesh* InStaticMesh)
{
	if(InStaticMesh)
	{
		StaticMesh = InStaticMesh;
		const int32 NumTexCoords = StaticMesh->RenderData->LODResources[0].GetNumTexCoords();
		VertexFactory.VertexBuffer = &StaticMesh->RenderData->LODResources[0].VertexBuffers.PositionVertexBuffer;
		VertexFactory.TangentVertexBuffer = &StaticMesh->RenderData->LODResources[0].VertexBuffers.StaticMeshVertexBuffer.TangentsVertexBuffer;
		// VertexFactory.TexCoordBuffer = &StaticMesh->RenderData->LODResources[0].VertexBuffers.StaticMeshVertexBuffer.TexCoordVertexBuffer;
		
	}
}

FCustomPrimitiveSceneProxy::FCustomPrimitiveSceneProxy(UCustomPrimitiveComponent* Component):
FPrimitiveSceneProxy(Component)
, bIsVisible(Component->bIsVisible)
, bIsDrawIndirect(Component->bIsDrawIndirect)
, InstanceCount(Component->InstanceCount)
, Component(Component)
, Material(Component->Material)
, ComponentLightInfo(nullptr)
, MaterialRelevance(Component->GetMaterialRelevance(GetScene().GetFeatureLevel()))
, StaticMesh(Component->StaticMesh)
, VertexFactory(GetScene().GetFeatureLevel(), "FCustomVertexFactory")
, Tex(Component->TextureInput)
, RT(Component->RT)
{
	ComponentLightInfo = MakeUnique<FCustomPrimitiveLCI>(Component);
	check(ComponentLightInfo);
	if(StaticMesh)
	{
		UpdateStaticMesh(StaticMesh);
		
	}
	ComputeShaderManager = new FComputeShaderManager();
	ComputeShaderManager->InitInstanceBufferGenResource();
}

FCustomPrimitiveSceneProxy::~FCustomPrimitiveSceneProxy()
{
	VertexFactory.ReleaseResource();
	OriginSRV.SafeRelease();
	TransformSRV.SafeRelease();
}

SIZE_T FCustomPrimitiveSceneProxy::GetTypeHash() const
{
	static size_t UniquePointer;
	return reinterpret_cast<size_t>(&UniquePointer);
}

//SceneProxy
void FCustomPrimitiveSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const
{
	if (!bIsVisible)
	{
		return;
	}

	if(!StaticMesh)
	{
		return;
	}
	
	const bool bIsWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;
	FMaterialRenderProxy* MaterialProxy = Material->GetRenderProxy();
	if (bIsWireframe)
	{
		FMaterialRenderProxy* WireframeMaterialInstance = new FColoredMaterialRenderProxy(
				GEngine->WireframeMaterial->GetRenderProxy(),
				FLinearColor(0, 0.5f, 1.f)
		);
		Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);
		MaterialProxy = WireframeMaterialInstance;
	}
	// Nothing to render with
	if (MaterialProxy == nullptr)
	{
		return;
	}

	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		if (VisibilityMap & (1 << ViewIndex))
		{
			if (bIsWireframe)
			{
				// Draw bounds around the points
				RenderBounds(Collector.GetPDI(ViewIndex), ViewFamily.EngineShowFlags, GetBounds(), IsSelected());
			}
			
			// Create a mesh batch for this chunk of point cloud
			FMeshBatch& MeshBatch = Collector.AllocateMesh();
			MeshBatch.LCI = ComponentLightInfo.Get();
			MeshBatch.CastShadow = true;
			MeshBatch.bUseAsOccluder = true;
			MeshBatch.VertexFactory = &VertexFactory;
			MeshBatch.MaterialRenderProxy = MaterialProxy;
			MeshBatch.ReverseCulling = IsLocalToWorldDeterminantNegative();
			MeshBatch.DepthPriorityGroup = SDPG_World;
			
			ERHIFeatureLevel::Type FeatureLevel = GetScene().GetFeatureLevel();
			
			// Set up index buffer
			//MeshBatch.Type = PointCloudIndexBuffer.IsTriList() ? PT_TriangleList : PT_QuadList;
			MeshBatch.Type = PT_TriangleList;
			FMeshBatchElement& BatchElement = MeshBatch.Elements[0];
			
			BatchElement.FirstIndex = 0;
			
			if(StaticMesh)
			{
				
				UCustomPrimitiveSubsystem* Subsystem = GetScene().GetWorld()->GetSubsystem<UCustomPrimitiveSubsystem>();
				bool bIsInitialized = false;
				if(Subsystem)
				{
					bIsInitialized = Subsystem->GetComputeShaderManager()->GetDrawIndirectBuffer().Buffer.IsValid();
				}
				if(!bIsDrawIndirect || !bIsInitialized)
				{
					//其中StaticMesh是通过如下方式：
					// --StaticMeshRender.cpp[941]--
					// NumPrimitives = Section.NumTriangles;
					//来获得Primitives的数量的。如果Primitives的数量为0，那么就代表要使用IndirectDrawArgs。
					BatchElement.NumPrimitives = StaticMesh->RenderData->LODResources[0].IndexBuffer.GetNumIndices()/3;
					
					BatchElement.NumInstances = InstanceCount;
					
					//最后一个顶点的Index。就是BaseVertexIndex + VerticesCount - 1
					//例如在PointCloudSceneProxy中，我们可以看到这个MaxIndex就是PointCount * 4 - 1
					//这是因为一个Quad有4个顶点，所以是*4。主要是这个顶点数据不是由Mesh产生的。如果是Mesh的，那么直接通过
					//BatchElement.MaxVertexIndex = VertexBuffers.PositionVertexBuffer.GetNumVertices() - 1;
					//这样就可以直接获取这个数量。
					BatchElement.MaxVertexIndex = StaticMesh->RenderData->LODResources[0].IndexBuffer.GetNumIndices()-1;
				}
				else
				{
					if(ComputeShaderManager)
					{
						// We need check that in the future. Right now we do not need it.f
						// if (IndirectArgsOffsets.IsValidIndex(SectionIndex))
						{
							BatchElement.NumPrimitives = 0;
							BatchElement.IndirectArgsOffset = 0;
					
							if(Subsystem)
							{
								BatchElement.IndirectArgsBuffer = Subsystem->GetComputeShaderManager()->GetDrawIndirectBuffer().Buffer;
							}
							else{
								BatchElement.IndirectArgsBuffer = ComputeShaderManager->GetDrawIndirectBuffer().Buffer;
							}
						}
					}
				}
			
				BatchElement.IndexBuffer = &StaticMesh->RenderData->LODResources[0].IndexBuffer;
				
			}

			//第一个顶点的Index。
			BatchElement.MinVertexIndex = 0;
			
			BatchElement.PrimitiveUniformBuffer = GetUniformBuffer();
			MeshBatch.bCanApplyViewModeOverrides = false;
			Collector.AddMesh(ViewIndex, MeshBatch);
		}
	}
}




//VertexFactory
void FCustomVertexFactory::InitRHI()
{
	FVertexStream Stream;
	// No streams should currently exist.
	check( Streams.Num() == 0 );

	// Position stream for per vertex local position
	// Mesh数据。从VertexBuffer来初始化VertexFactory的PositionVertexStream。
	// 更新数据的方式需要更优雅的解决方法，最好的方式应该是使用中间数据层，就像SkinCache一样，
	// 应该只给VertexFactory暴露Position Buffer。目前可以存在VertexFactroy里。这样方便测试。
	
	FVertexStream PositionVertexStream;
	PositionVertexStream.VertexBuffer = VertexBuffer;
	PositionVertexStream.Stride = sizeof(FVector);
	PositionVertexStream.Offset = 0;
	PositionVertexStream.VertexStreamUsage = EVertexStreamUsage::Default;
	
	FVertexStream TangentXVertexStream;
	TangentXVertexStream.VertexBuffer = TangentVertexBuffer;
	TangentXVertexStream.Stride = 8;
	TangentXVertexStream.Offset = 0;
	TangentXVertexStream.VertexStreamUsage = EVertexStreamUsage::Default;
	
	FVertexStream TangentZVertexStream;
	TangentZVertexStream.VertexBuffer = TangentVertexBuffer;
	TangentZVertexStream.Stride = 8;
	TangentZVertexStream.Offset = 4;
	TangentZVertexStream.VertexStreamUsage = EVertexStreamUsage::Default;
	//
	// FVertexStream TexCoordVertexStream;
	// TexCoordVertexStream.VertexBuffer = TexCoordBuffer;
	// TexCoordVertexStream.Stride = sizeof(TStaticMeshVertexUVsTypeSelector<EStaticMeshVertexUVType::Default>::UVsTypeT);
	// TexCoordVertexStream.Offset = 0;
	// TexCoordVertexStream.VertexStreamUsage = EVertexStreamUsage::Default;

	//这里的顺序会影响在VertexFactoryInput里的顺序，需要注意的是，这个顺序其实是可以人为指定的。例如如下函数，是可以指定
	//InAttributeIndex的，那么attribute X 是取决于这个index。如果是按顺序，像LocalVertexFactory一样一个个往下累加，那么这个顺序就
	//依次向下拍排列。
	const FVertexElement VertexPositionElement(Streams.Add(PositionVertexStream), 0, VET_Float3, 0, PositionVertexStream.Stride, false);
	const FVertexElement VertexTangentXElement(Streams.Add(TangentXVertexStream), 0, VET_PackedNormal, 1, TangentXVertexStream.Stride, false);
	const FVertexElement VertexTangentZElement(Streams.Add(TangentZVertexStream), 0, VET_PackedNormal, 2, TangentZVertexStream.Stride, false);
	// const FVertexElement TexCoordVertexElement(Streams.Add(TexCoordVertexStream), 0, VET_Half2, 3, TexCoordVertexStream.Stride, false);
	
	// Vertex declaration
	FVertexDeclarationElementList Elements;
	Elements.Add(VertexPositionElement);
	Elements.Add(VertexTangentXElement);
	Elements.Add(VertexTangentZElement);
	// Elements.Add(TexCoordVertexElement);
	
	InitDeclaration(Elements);
	
}

//不要忘了
void FCustomVertexFactory::ReleaseRHI()
{
	UniformBuffer.SafeRelease();
	FVertexFactory::ReleaseRHI();
}

bool FCustomVertexFactory::ShouldCompilePermutation(const FVertexFactoryShaderPermutationParameters& Parameters)
{
	// We exclusively use manual fetch, so we need that supported
	return RHISupportsManualVertexFetch(Parameters.Platform);
}

void FCustomVertexFactory::ModifyCompilationEnvironment(const FVertexFactoryShaderPermutationParameters& Parameters,
	FShaderCompilerEnvironment& OutEnvironment)
{
	FVertexFactory::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.SetDefine(TEXT("USE_MYINSTANCING"),TEXT("1"));
}

void FCustomVertexFactory::SetParameters(const FCustomVertexFactoryParameters& InUniformParameters)
{
	UniformBuffer = FCustomVertexFactoryBufferRef::CreateUniformBufferImmediate(InUniformParameters, UniformBuffer_MultiFrame);
}

//
// void FCustomVertexFactory::InitVertexResource_RenderThread(FRHICommandListImmediate& RHICmdList)
// {
// 	int NumOfVerts = 6;
// 	
// 	TResourceArray<MyVertex> ResourceArray;
// 	FRHIResourceCreateInfo CreateInfo;
// 	CreateInfo.ResourceArray = &ResourceArray;
// 	ResourceArray.AddZeroed(NumOfVerts);
// 	// // ResourceArray.Append(ResourceArray);
// 	// StructuredVertexBuffer = RHICreateStructuredBuffer(sizeof(MyVertex), ResourceArray.GetResourceDataSize(), BUF_UnorderedAccess | BUF_ShaderResource, CreateInfo);
// 	// //StructuredVertexUAV = RHICreateUnorderedAccessView(StructuredVertexBuffer, true, false);
// 	// StructuredVertexSRV = RHICreateShaderResourceView(StructuredVertexBuffer);
// 	//设置顶点输入流的格式
// 	//我们此时没有使用VertexBuffer来获取数据，所以这么写就好了
// 	SetDeclaration(GEmptyVertexDeclaration.VertexDeclarationRHI);
// }
//
