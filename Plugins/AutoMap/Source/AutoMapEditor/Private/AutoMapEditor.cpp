#include "AutoMapEditor.h"

#include "AutoMapBoundsActor.h"
#include "AutoMapEditorSubsystem.h"
#include "AutoMapPinBase.h"
#include "AutoMapSubsystem.h"
#include "AutoMapWorldRegionGenerator.h"
#include "LevelEditor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#define LOCTEXT_NAMESPACE "FAutoMapEditorModule"

void FAutoMapEditorModule::StartupModule()
{
	FLevelEditorModule& LevelEditorModule =
		FModuleManager::LoadModuleChecked<FLevelEditorModule>
		("LevelEditor");
	TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());

	MenuExtender->AddMenuExtension(
		"Tools",
		EExtensionHook::After,
		nullptr,
		FMenuExtensionDelegate::CreateRaw(this, &FAutoMapEditorModule::AddMenu)
	);
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
}

void FAutoMapEditorModule::ShutdownModule()
{
}


void FAutoMapEditorModule::AddMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddSubMenu(
		LOCTEXT("AutoMapMenu", "Auto Map"),
		LOCTEXT("MenuTooltipKey", "Auto Map Plugin Menu"),
		FNewMenuDelegate::CreateRaw(this, &FAutoMapEditorModule::FillMenu),
		false,
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "SceneOutliner.World")
	);
}

void FAutoMapEditorModule::FillMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		LOCTEXT("SpawnMapBoundActor", "Spawn Map Bounds Actors"),
		LOCTEXT("SpawnMapBoundActorTooltipKey", "Click to spawn map bound actors to place."),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "SurfaceDetails.ClockwiseRotation"),
		FUIAction(FExecuteAction::CreateRaw(this, &FAutoMapEditorModule::OnSpawnMapBoundsActorButtonClicked))
	);

	MenuBuilder.AddMenuEntry(
		LOCTEXT("GenerateMapKey", "Generate Map"),
		LOCTEXT("GenerateMapTooltipKey", "Click to generate the map"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "ProjectSettings.TabIcon"),
		FUIAction(FExecuteAction::CreateRaw(this, &FAutoMapEditorModule::OnGenerateMapButtonClicked))
	);

	MenuBuilder.AddMenuEntry(
		LOCTEXT("AddPinKey", "Add Pin Actor"),
		LOCTEXT("AddPinKeyTooltipKey", "Click to add pin definition actor"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "ViewportActorPreview.Pinned"),
		FUIAction(FExecuteAction::CreateRaw(this, &FAutoMapEditorModule::OnAddPinButtonClicked))
	);

	MenuBuilder.AddMenuEntry(
		LOCTEXT("DefineMapLocations", "Define Map Locations"),
		LOCTEXT("DefineMapLocationsTooltipKey", "Click to define map locations"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Download"),
		FUIAction(FExecuteAction::CreateRaw(this, &FAutoMapEditorModule::OnDefineMapLocationsButtonClicked))
	);

	MenuBuilder.AddMenuEntry(
		LOCTEXT("SpawnRegionDefinitionActor", "Define Region"),
		LOCTEXT("SpawnRegionDefinitionActorTooltipKey", "Click to spawn region definition actor"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentReference.PickAsset"),
		FUIAction(FExecuteAction::CreateRaw(this, &FAutoMapEditorModule::OnSpawnMapRegionButtonClicked))
	);
	MenuBuilder.AddMenuEntry(
	LOCTEXT("ReGenerateAllRegionDefinitions", "Re-Generate All Region Definitions"),
	LOCTEXT("ReGenerateAllRegionDefinitionsTooltipKey", "Click to Regenerate All Definitions"),
	FSlateIcon(FAppStyle::GetAppStyleSetName(), "SoftwareCursor_CardinalCross"),
	FUIAction(FExecuteAction::CreateRaw(this, &FAutoMapEditorModule::OnRegenerateRegionsClicked))
);
}

void FAutoMapEditorModule::OnSpawnMapBoundsActorButtonClicked()
{
	if (UWorld* World = GEditor->GetEditorWorldContext().World())
	{
		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClass(World, AAutoMapBoundsActor::StaticClass(), OutActors);
		if (!OutActors.IsEmpty())
		{
			for (AActor* FoundBoundsActor : OutActors)
			{
				FoundBoundsActor->SetActorLabel(
					*FString::FromInt(UKismetMathLibrary::RandomIntegerInRange(0, 999999999)));
				FoundBoundsActor->Rename(*FString::FromInt(UKismetMathLibrary::RandomIntegerInRange(0, 999999999)));
				FoundBoundsActor->Destroy();
			}
			GEditor->ForceGarbageCollection();
			UE_LOG(LogTemp, Warning, TEXT("AutoMap-> Deleted Previous Map Bounds Actors"));
		}

		for (int32 i = 0; i < 2; i++)
		{
			FVector Location(i * 100, 0.0f, 0.0f);
			FRotator Rotation(0.0f, 0.0f, 0.0f);
			FActorSpawnParameters SpawnInfo;

			FString ActorName = "A";
			if (i == 1)
			{
				ActorName = "B";
			}
			SpawnInfo.Name = FName("Auto Map Bounds" + ActorName);
			AActor* SpawnedActor = World->SpawnActor<AAutoMapBoundsActor>(Location, Rotation, SpawnInfo);
			SpawnedActor->SetActorLabel("Auto Map Bounds " + ActorName);
			UE_LOG(LogTemp, Warning, TEXT("AutoMap-> Spawned Map Bounds Actors"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AutoMap-> No Maps Detected"));
	}
}

void FAutoMapEditorModule::OnAddPinButtonClicked()
{
	if (UWorld* World = GEditor->GetEditorWorldContext().World())
	{
		FVector Location(0, 0.0f, 0.0f);
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;

		AActor* SpawnedActor = World->SpawnActor<AAutoMapPinBase>(Location, Rotation, SpawnInfo);

		FViewport* activeViewport = GEditor->GetActiveViewport();
		FEditorViewportClient* editorViewClient = (activeViewport != nullptr)
			                                          ? static_cast<FEditorViewportClient*>(activeViewport->GetClient())
			                                          : nullptr;
		if (editorViewClient)
		{
			FVector EditorCameraLocation = editorViewClient->GetViewLocation();
			FRotator EditorCameraRotation = editorViewClient->GetViewRotation();
			FVector EditorCameraDirection = FRotationMatrix(EditorCameraRotation).GetUnitAxis(EAxis::X);
			GEditor->MoveActorInFrontOfCamera(*SpawnedActor, EditorCameraLocation, EditorCameraDirection);
		}
		UE_LOG(LogTemp, Warning, TEXT("AutoMap-> Spawned Map Pin Actor"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AutoMap-> No Maps Detected"));
	}
}

void FAutoMapEditorModule::OnDefineMapLocationsButtonClicked()
{
	FString AssetPath = FPaths::ProjectPluginsDir() + TEXT(
		"/Script/Engine.DataTable'/AutoMap/Data/AutoMapRegions.AutoMapRegions'");
	UObject* Asset = StaticLoadObject(UDataTable::StaticClass(), nullptr, *AssetPath);
	if (Asset)
	{
		GEditor->EditObject(Asset);
	}
}

void FAutoMapEditorModule::OnSpawnMapRegionButtonClicked()
{
	if (UWorld* World = GEditor->GetEditorWorldContext().World())
	{
		FVector Location(0, 0.0f, 0.0f);
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;

		AActor* SpawnedActor = World->SpawnActor<AAutoMapWorldRegionGenerator>(Location, Rotation, SpawnInfo);

		FViewport* activeViewport = GEditor->GetActiveViewport();
		FEditorViewportClient* editorViewClient = (activeViewport != nullptr)
			                                          ? static_cast<FEditorViewportClient*>(activeViewport->GetClient())
			                                          : nullptr;
		if (editorViewClient)
		{
			FVector EditorCameraLocation = editorViewClient->GetViewLocation();
			FRotator EditorCameraRotation = editorViewClient->GetViewRotation();
			FVector EditorCameraDirection = FRotationMatrix(EditorCameraRotation).GetUnitAxis(EAxis::X);
			GEditor->MoveActorInFrontOfCamera(*SpawnedActor, EditorCameraLocation, EditorCameraDirection);
		}
		UE_LOG(LogTemp, Warning, TEXT("AutoMap-> Region Generator Actor"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AutoMap-> No Maps Detected"));
	}
}

void FAutoMapEditorModule::OnGenerateMapButtonClicked()
{
	//Get Subsystem
	if (GEditor->GetEditorWorldContext().World())
	{
        if (UAutoMapEditorSubsystem* AutoMapEditorSubsystemSubsystem = GEditor->GetEditorSubsystem<UAutoMapEditorSubsystem>())
		{
			AutoMapEditorSubsystemSubsystem->SnapshotMap();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AutoMap-> No Subsystem Found"));
		}
	}
}

void FAutoMapEditorModule::OnRegenerateRegionsClicked()
{
	
	if (UWorld* World = GEditor->GetEditorWorldContext().World())
	{
		TArray<AActor*> TempActors;
		TArray<AAutoMapPinBase*> OutPins;
		
		UGameplayStatics::GetAllActorsOfClass(World, AAutoMapPinBase::StaticClass(), TempActors);

		for (AActor* Actor : TempActors)
		{
			if (AAutoMapPinBase* AutoMapPin = static_cast<AAutoMapPinBase*>(Actor))
			{
				if (AutoMapPin->PinType == Location)
				{
					OutPins.Add(AutoMapPin);
				}
			}
			if (!OutPins.IsEmpty())
			{
				for (auto OutPin : OutPins)
				{
					OutPin->Destroy();
				}
				UE_LOG(LogTemp, Warning, TEXT("AutoMap-> Deleting Previous Regions Data"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("AutoMap-> No Region Generators Found"));
			}
		}
		TempActors.Empty();
		
		TArray<AAutoMapWorldRegionGenerator*> OutRegions;
			
		UGameplayStatics::GetAllActorsOfClass(World, AAutoMapWorldRegionGenerator::StaticClass(), TempActors);
		for (AActor* Actor : TempActors)
		{
			if (AAutoMapWorldRegionGenerator* RegionGenerator = static_cast<AAutoMapWorldRegionGenerator*>(Actor))
			{
				OutRegions.Add(RegionGenerator);
			}
			if (!OutRegions.IsEmpty())
			{
				for (auto Region : OutRegions)
				{
					Region->InitializeSpline(Region->PinSpawnDensity);;
					UE_LOG(LogTemp, Warning, TEXT("AutoMap-> Successfully Regenerated Regions For AutoMapSubsystem"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("AutoMap-> No Region Generators Found"));

			}
		}
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AutoMap-> No Maps Detected"));
	}
}
#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAutoMapEditorModule, AutoMapEditor)
