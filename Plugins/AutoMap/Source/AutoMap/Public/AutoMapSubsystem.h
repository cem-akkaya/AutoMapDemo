// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AutoMapPinBase.h"
#include "AutoMapProcessor.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AutoMapSubsystem.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FAutoMapRegionsStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Auto Map - Locations Region")
	FName RegionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Auto Map - Locations Region")
	EMapLocationCategory LocationCategory;
	
	FAutoMapRegionsStruct()
		: RegionName(NAME_None),
		  LocationCategory(EMapLocationCategory::Region)
	{
	}
};

UCLASS()
class AUTOMAP_API UAutoMapSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Auto Map" , DisplayName="Map Snapshot Resolution")
	int32 Resolution = 8192;  // @todo this will be setting

	UFUNCTION(BlueprintCallable, Category="Auto Map" , DisplayName="Init Coordinate System")
	void InitCoordinateSystem(FVector LocalCoordinatePlane);
	
	UFUNCTION(BlueprintCallable, Category="Auto Map" , DisplayName="Get Coordinate System")
	void GetCoordinateSystem(bool &Status, float &CoordinateScales, float &CoordinateSystemDimensions, FVector &MapWorldOrigin);

	UFUNCTION(BlueprintCallable, Category="Auto Map" , DisplayName="Convert World Transform To Map Transform")
	void ConvertWorldTransformToMapTransform( FTransform InWorldTransform, FTransform &OutMapTransform);

	UFUNCTION(BlueprintCallable, Category="Auto Map" , DisplayName="Convert World Transform To Map Screen Transform")
	void ConvertWorldTransformToMapScreenTransform( FTransform InWorldTransform, FTransform &OutMapTransform, float &OutMapAngle);
	
	UFUNCTION(BlueprintCallable, Category="Auto Map - Pins" , DisplayName="Get All Pins By Type")
	void GetAllPins(EPinType PinType, TArray<AAutoMapPinBase*> &OutPins);

	UFUNCTION(BlueprintCallable, Category="Auto Map - Pins" , DisplayName="Get Location Pins")
	void GetLocationPins(FName LocationName, TArray<AAutoMapPinBase*> &OutPins);

	UFUNCTION(BlueprintCallable, Category="Auto Map - Pins" , DisplayName="Get Location Names")
	void GetAllLocationNames(EMapLocationCategory LocationCategory, TArray<FName>& OutLocations);

	UFUNCTION(BlueprintCallable, Category="Auto Map - Pins" , DisplayName="Resolve Region Name")
	FName ResolveRegionName(FName InputRegionName);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Auto Map" , DisplayName="Max Height To Capture")
	AAutoMapProcessor* AutoMapProcessor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Auto Map" , DisplayName="Map Snapshot UMG Resolution")
	int32 UMGResolution = 2048;  // @todo this will be setting

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Auto Map" , DisplayName="Coordinate Scale")
	float CoordinateScale = -1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Auto Map" , DisplayName="Coordinate Scale X")
	float CoordinateScaleX = -1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Auto Map" , DisplayName="Coordinate Scale Y")
	float CoordinateScaleY = -1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Auto Map" , DisplayName="Coordinate Scale Z")
	float CoordinateScaleZ = -1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Auto Map" , DisplayName="Coordinate System Dimension")
	float CoordinateSystemDimension = -1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Auto Map" , DisplayName="Coordinate System World Origin")
	FVector CoordinateSystemWorldOrigin;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Auto Map" , DisplayName="Local Coordinate Plane ")
	FVector LocalCoordinatePlane;

private:

	UPROPERTY()
	EMapLocationCategory LocationCategory = EMapLocationCategory::Region;
};
