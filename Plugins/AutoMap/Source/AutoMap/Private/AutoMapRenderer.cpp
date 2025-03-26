// Fill out your copyright notice in the Description page of Project Settings.


#include "AutoMapRenderer.h"

#include "EditorReimportHandler.h"
#include "ImageUtils.h"
#include "Components/SceneCaptureComponent2D.h"
#include "EditorFramework/AssetImportData.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetRenderingLibrary.h"


// Sets default values
AAutoMapRenderer::AAutoMapRenderer(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SnapshotCamera = CreateDefaultSubobject<USceneCaptureComponent2D>("Capture Component");
	SetRootComponent(SnapshotCamera);

	
	static ConstructorHelpers::FObjectFinder<UTexture> FinalColorTexRef(*(FPaths::ProjectPluginsDir() / TEXT("/Script/Engine.Texture2D'/AutoMap/AutoMapFinalColorSnapshot.AutoMapFinalColorSnapshot'")));
	static ConstructorHelpers::FObjectFinder<UTexture> AlbedoTexRef(*(FPaths::ProjectPluginsDir() / TEXT("/Script/Engine.Texture2D'/AutoMap/AutoMapAlbedoSnapshot.AutoMapAlbedoSnapshot'")));
	static ConstructorHelpers::FObjectFinder<UTexture> NormalTexRef(*(FPaths::ProjectPluginsDir() / TEXT("/Script/Engine.Texture2D'/AutoMap/AutoMapNormalSnapshot.AutoMapNormalSnapshot'")));
	static ConstructorHelpers::FObjectFinder<UTexture> DepthTexRef(*(FPaths::ProjectPluginsDir() / TEXT("/Script/Engine.Texture2D'/AutoMap/AutoMapDepthSnapshot.AutoMapDepthSnapshot'")));

	FinalColorTex = FinalColorTexRef.Object;
	AlbedoTex = AlbedoTexRef.Object;
	NormalTex = NormalTexRef.Object;
	DepthTex = DepthTexRef.Object;
}

void AAutoMapRenderer::PrepareRenderer(float RenderWidth, int32 TexResolution)
{
	Resolution = TexResolution;
	
	//Capture Common Settings
	SnapshotCamera->bCaptureEveryFrame = false;
	SnapshotCamera->MaxViewDistanceOverride = -1.0f;
	SnapshotCamera->bAutoActivate = true;
	SnapshotCamera->bCaptureOnMovement = false;
	SnapshotCamera->bMainViewCamera = true;
	SnapshotCamera->bAlwaysPersistRenderingState = true;
	SnapshotCamera->ProjectionType = ECameraProjectionMode::Orthographic;
	SnapshotCamera->bEnableClipPlane = false;
	SnapshotCamera->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;
	SnapshotCamera->OrthoWidth = RenderWidth;

	//Start Capture
	GetRenders();
}

void AAutoMapRenderer::GetRenders()
{
	CaptureFinalColor();
	CaptureAlbedo();
	CaptureNormal();
	CaptureDepth();
	this->Destroy();
}

void AAutoMapRenderer::CaptureFinalColor()
{
	// Set Texture
	UTextureRenderTarget2D* FinalColorRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GEditor->GetEditorWorldContext().World(), Resolution, Resolution, RTF_RGBA8);
	SnapshotCamera->TextureTarget = FinalColorRenderTarget;
	
	// Capture
	SnapshotCamera->CaptureSource = SCS_FinalColorLDR;
	SnapshotCamera->CaptureScene();

	// Save
	FImage RenderImage;
	if (FImageUtils::GetRenderTargetImage(SnapshotCamera->TextureTarget, RenderImage))
	{
		TArray64<uint8> RenderData;
		FImageUtils::SaveImageByExtension(*(FPaths::ProjectPluginsDir() / TEXT("AutoMap/Content/AutoMapFinalColorSnapshot.png")), RenderImage);

		if (FinalColorTex)
		{
			FinalColorTex->UpdateResource();
			ReImportImages(FinalColorTex);
		}
		
		UE_LOG(LogTemp, Display, TEXT("AutoMap-> Created Final Color Render"));
	}
}

void AAutoMapRenderer::CaptureAlbedo()
{
	// Set Texture
	UTextureRenderTarget2D* AlbedoRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GEditor->GetEditorWorldContext().World(), Resolution, Resolution, RTF_RGBA8);
	SnapshotCamera->TextureTarget = AlbedoRenderTarget;
	
	// Capture
	SnapshotCamera->CaptureSource = SCS_BaseColor;
	SnapshotCamera->CaptureScene();

	// Save
	FImage RenderImage;
	if (FImageUtils::GetRenderTargetImage(SnapshotCamera->TextureTarget, RenderImage))
	{
		TArray64<uint8> RenderData;
        FImageUtils::SaveImageByExtension(*(FPaths::ProjectPluginsDir() / TEXT("AutoMap/Content/AutoMapAlbedoSnapshot.png")), RenderImage);

		if (AlbedoTex)
		{
			AlbedoTex->UpdateResource();
			ReImportImages(AlbedoTex);
		}
		
		UE_LOG(LogTemp, Display, TEXT("AutoMap-> Created Albedo Render"));
	}
}

void AAutoMapRenderer::CaptureNormal()
{
	// Set Texture
	UTextureRenderTarget2D* NormalRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GEditor->GetEditorWorldContext().World(), Resolution, Resolution, RTF_RGBA8);
	SnapshotCamera->TextureTarget = NormalRenderTarget;
	
	// Capture
	SnapshotCamera->CaptureSource = SCS_Normal;
	SnapshotCamera->CaptureScene();

	// Save
	FImage RenderImage;
	if (FImageUtils::GetRenderTargetImage(SnapshotCamera->TextureTarget, RenderImage))
	{
		TArray64<uint8> RenderData;
		FImageUtils::SaveImageByExtension(*(FPaths::ProjectPluginsDir() / TEXT("AutoMap/Content/AutoMapNormalSnapshot.png")), RenderImage);

		if (NormalTex)
		{
			NormalTex->UpdateResource();
			ReImportImages(NormalTex);
		}
		
		UE_LOG(LogTemp, Display, TEXT("AutoMap-> Created Normal Render"));
	}
}

void AAutoMapRenderer::CaptureDepth()
{
	// Set Texture
	UTextureRenderTarget2D* DepthRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GEditor->GetEditorWorldContext().World(), Resolution, Resolution, RTF_R8);
	SnapshotCamera->TextureTarget = DepthRenderTarget;
	
	// Capture
	SnapshotCamera->CaptureSource = SCS_DeviceDepth;
	SnapshotCamera->CaptureScene();

	// Save
	FImage RenderImage;
	if (FImageUtils::GetRenderTargetImage(SnapshotCamera->TextureTarget, RenderImage))
	{
		TArray64<uint8> RenderData;
		FImageUtils::SaveImageByExtension(*(FPaths::ProjectPluginsDir() / TEXT("AutoMap/Content/AutoMapDepthSnapshot.png")), RenderImage);

		if (DepthTex)
		{
			DepthTex->UpdateResource();
			ReImportImages(DepthTex);
		}
		
		UE_LOG(LogTemp, Display, TEXT("AutoMap-> Created Depth Render"));
	}


}

void AAutoMapRenderer::ReImportImages(UTexture* TextureToImport)
{
	if (TextureToImport)
	{
		// Ensure the texture is updated
		TextureToImport->UpdateResource();

		// Get the import data and re-import the asset
		if (UAssetImportData* ImportData = TextureToImport->AssetImportData)
		{
			FString AssetPath = TextureToImport->GetPathName();
		
			if (ImportData->GetFirstFilename() != "")
			{
				if (FReimportManager::Instance()->Reimport(TextureToImport))
				{
					UE_LOG(LogTemp, Display, TEXT("AutoMap-> Texture Reimported Successfully."));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("AutoMap-> Failed to reimport texture."));
				}
				UE_LOG(LogTemp, Display, TEXT("AutoMap->Texture Reimported Successfully."));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("AutoMap-> Unable to find source file for re-import."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AutoMap-> AssetImportData is invalid for texture."));
		}
	}
}

// Called when the game starts or when spawned
void AAutoMapRenderer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAutoMapRenderer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

