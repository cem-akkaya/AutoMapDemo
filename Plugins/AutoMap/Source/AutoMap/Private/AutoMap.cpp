// Copyright Epic Games, Inc. All Rights Reserved.

#include "AutoMap.h"

#define LOCTEXT_NAMESPACE "FAutoMapModule"

class FAssetRegistryModule;

void FAutoMapModule::StartupModule()
{

}


void FAutoMapModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAutoMapModule, AutoMap)