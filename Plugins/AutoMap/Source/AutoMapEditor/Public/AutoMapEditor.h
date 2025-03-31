#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FAutoMapEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    // Add New Menu Items
    void AddMenu(FMenuBuilder& MenuBuilder);
    void FillMenu(FMenuBuilder& MenuBuilder);

    // Menu Actions
    void OnGenerateMapButtonClicked();
    void OnSpawnMapBoundsActorButtonClicked();
    void OnAddPinButtonClicked();
    void OnDefineMapLocationsButtonClicked();
    void OnSpawnMapRegionButtonClicked();
};
