// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AutoMapPinBase.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "AutoMapWorldRegionGenerator.generated.h"

UCLASS()
class AUTOMAP_API AAutoMapWorldRegionGenerator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAutoMapWorldRegionGenerator(const FObjectInitializer& ObjectInitializer);

	virtual void PostEditMove(bool bFinished) override;

	virtual void BeginDestroy() override;

	UPROPERTY(EditAnywhere, Category="Auto Map")
	USplineComponent* RegionSplineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Auto Map - Pin Location", meta = (GetOptions = "InitializeLocation"))
	FName RegionID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Auto Map - Pin Location", meta = (ClampMin="1", ClampMax="50", SliderMin="1", SliderMax="50"))
	float PinSpawnDensity = 10;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitializeSpline(float SpawnRate);

	TArray<AAutoMapPinBase*> SpawnedWorldRegionsPins;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnLocationPin(FVector SpawnLocation);

	UFUNCTION(CallInEditor, BlueprintCallable)
	TArray<FName> InitializeLocation();
};
