#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FLayerWeightEditorModule : public IModuleInterface
{
public:
    void RegisterAssetsAction() const;
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
