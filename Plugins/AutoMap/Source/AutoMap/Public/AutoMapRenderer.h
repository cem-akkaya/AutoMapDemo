// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AutoMapRenderer.generated.h"

UCLASS()
class AUTOMAP_API AAutoMapRenderer : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAutoMapRenderer(const FObjectInitializer& ObjectInitializer);

	UPROPERTY()
	USceneCaptureComponent2D* SnapshotCamera;

	UFUNCTION()
	void PrepareRenderer(float RenderWidth, int32 TexResolution);

	UFUNCTION()
	void GetRenders();


protected:
	
	void CaptureFinalColor();
	
	void CaptureAlbedo();
	
	void CaptureNormal();
	
	void CaptureDepth();

	void ReImportImages(UTexture* TextureToImport);

	int32 Resolution;

	UTexture* FinalColorTex;
	UTexture * AlbedoTex;
	UTexture * NormalTex;
	UTexture * DepthTex;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
