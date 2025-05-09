﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "AutoMapEditorRenderer.h"
#include "AutoMapEditorSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class AUTOMAPEDITOR_API UAutoMapEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category="Auto Map" , DisplayName="Render Map Snapshot")
	void SnapshotMap();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Auto Map" , DisplayName="Max Height To Capture")
	float SnapshotHeight = 100000; // @todo this will be setting

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Auto Map" , DisplayName="Map Snapshot Resolution")
	int32 Resolution = 8192;  // @todo this will be setting
	
	UPROPERTY()
	AAutoMapEditorRenderer* SnapshotCameraActor;

	UFUNCTION(CallInEditor, Category="Auto Map" , DisplayName="Set Actor To Camera View")
	void PositionToCamera(AActor* InActor);
	
	void GetCoordinateSystem(bool& Status, float& CoordinateScales, float& CoordinateSystemDimensions,
	                         FVector& MapWorldOrigin);
};
