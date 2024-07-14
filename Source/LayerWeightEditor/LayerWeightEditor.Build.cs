using UnrealBuildTool;

public class LayerWeightEditor : ModuleRules
{
    public LayerWeightEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicIncludePaths.AddRange(
            new string[] {
                // ... add public include paths required here ...
                "LayerWeightEditor/Public",
                
            }
        );
				
		
        PrivateIncludePaths.AddRange(
            new string[] {
                // ... add other private include paths required here ...
                "LayerWeightEditor/Private",
                "LayerWeightEditor/Private/LayerWeightGraph",
                "LayerWeightEditor/Private/Widgets",
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "UnrealEd"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "CustomPrimitive", // 我的GAMEPLAY_PRIMARY_MODULE
                "AssetTools",
                "ToolMenus",
                "UnrealEd",
                "GraphEditor",
                "ApplicationCore",
                "EditorStyle",
                

                
            }
        );
    }
}