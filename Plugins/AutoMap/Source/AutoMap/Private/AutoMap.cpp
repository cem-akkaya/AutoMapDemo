// Copyright Epic Games, Inc. All Rights Reserved.

#include "AutoMap.h"

#include "AutoMapBoundsActor.h"
#include "AutoMapPinBase.h"
#include "AutoMapSubsystem.h"
#include "LevelEditor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#define LOCTEXT_NAMESPACE "FAutoMapModule"

void FAutoMapModule::StartupModule()
{
	FLevelEditorModule& LevelEditorModule = 
	FModuleManager::LoadModuleChecked<FLevelEditorModule> 
	("LevelEditor");
	TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());

	MenuExtender->AddMenuExtension(
	  "Tools", 
	  EExtensionHook::After,
	  nullptr,
	  FMenuExtensionDelegate::CreateRaw(this, &FAutoMapModule::AddMenu)
	);
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
}

void FAutoMapModule::AddMenu(FMenuBuilder& MenuBuilder){
	MenuBuilder.AddSubMenu(
	  LOCTEXT("AutoMapMenu", "Auto Map"),
	  LOCTEXT("MenuTooltipKey", "Auto Map Plugin Menu"),
	  FNewMenuDelegate::CreateRaw(this, &FAutoMapModule::FillMenu),
	  false,
	  FSlateIcon(FAppStyle::GetAppStyleSetName(), "SceneOutliner.World")
	);
}

void FAutoMapModule::FillMenu(FMenuBuilder& MenuBuilder) {
	MenuBuilder.AddMenuEntry(
		LOCTEXT("SpawnMapBoundActor", "Spawn Map Bounds Actors"),
		LOCTEXT("SpawnMapBoundActorTooltipKey", "Click to spawn map bound actors to place."),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "SurfaceDetails.ClockwiseRotation"),
		FUIAction(FExecuteAction::CreateRaw(this, &FAutoMapModule::OnSpawnMapBoundsActorButtonClicked))
	);

	MenuBuilder.AddMenuEntry(
		LOCTEXT("GenerateMapKey", "Generate Map"),
		LOCTEXT("GenerateMapTooltipKey", "Click to generate the map"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "ProjectSettings.TabIcon"),
		FUIAction(FExecuteAction::CreateRaw(this, &FAutoMapModule::OnGenerateMapButtonClicked))
	);

	MenuBuilder.AddMenuEntry(
	LOCTEXT("AddPinKey", "Add Pin Actor"),
	LOCTEXT("AddPinKeyTooltipKey", "Click to add pin definition actor"),
	FSlateIcon(FAppStyle::GetAppStyleSetName(), "ProjectSettings.TabIcon"),
	FUIAction(FExecuteAction::CreateRaw(this, &FAutoMapModule::OnAddPinButtonClicked))
);
}

void FAutoMapModule::OnSpawnMapBoundsActorButtonClicked()
{
	if (UWorld* World = GEditor->GetEditorWorldContext().World()) {

		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClass(World, AAutoMapBoundsActor::StaticClass(), OutActors);
		if (!OutActors.IsEmpty())
		{
			for (AActor* FoundBoundsActor : OutActors)
			{
				FoundBoundsActor->SetActorLabel(*FString::FromInt(UKismetMathLibrary::RandomIntegerInRange(0, 999999999)));
				FoundBoundsActor->Rename(*FString::FromInt(UKismetMathLibrary::RandomIntegerInRange(0, 999999999)));
				FoundBoundsActor->Destroy();
			}
			GEditor->ForceGarbageCollection();
			UE_LOG(LogTemp, Warning, TEXT("AutoMap-> Deleted Previous Map Bounds Actors"));
		}

		for (int32 i = 0; i < 2; i++)
		{
			FVector Location(i*100, 0.0f, 0.0f);
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

void FAutoMapModule::OnAddPinButtonClicked()
{
	if (UWorld* World = GEditor->GetEditorWorldContext().World()) {

		FVector Location(0, 0.0f, 0.0f);
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;
		
		AActor* SpawnedActor = World->SpawnActor<AAutoMapPinBase>(Location, Rotation, SpawnInfo);

		FViewport* activeViewport = GEditor->GetActiveViewport();
		FEditorViewportClient* editorViewClient = (activeViewport != nullptr) ? (FEditorViewportClient*)activeViewport->GetClient() : nullptr;
		if( editorViewClient )
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

void FAutoMapModule::OnGenerateMapButtonClicked()
{
	//Get Subsystem
	if (GEditor->GetEditorWorldContext().World()) {
		if (UAutoMapSubsystem* AutoMapSubsystem = GEngine->GetEngineSubsystem<UAutoMapSubsystem>())
		{
			AutoMapSubsystem->SnapshotMap();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AutoMap-> No Subsystem Found"));
		}
	}
}

void FAutoMapModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAutoMapModule, AutoMap)