// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FAutoMapModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
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
