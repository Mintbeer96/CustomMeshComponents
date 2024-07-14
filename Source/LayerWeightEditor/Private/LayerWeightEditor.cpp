#include "LayerWeightEditor.h"

#include "IAssetTools.h"
#include "LayerWeightEditorActions.h"

#define LOCTEXT_NAMESPACE "LayerWeightEditorModule"

void FLayerWeightEditorModule::RegisterAssetsAction() const
{
	IAssetTools& AssetToolModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	const auto Category = AssetToolModule.RegisterAdvancedAssetCategory(FName(TEXT("LayerWeightEditorAsset")), LOCTEXT("LayerWeightEditor.DisplayName", "Layer Weight Editor Asset"));
	const TSharedPtr<FLayerWeightEditorActions> AssetsTypeAction = MakeShareable(new FLayerWeightEditorActions(Category));
	AssetToolModule.RegisterAssetTypeActions(AssetsTypeAction.ToSharedRef());
}

void FLayerWeightEditorModule::StartupModule()
{
	RegisterAssetsAction();
}

void FLayerWeightEditorModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FLayerWeightEditorModule, LayerWeightEditor)