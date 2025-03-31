// Fill out your copyright notice in the Description page of Project Settings.


#include "AutoMapWorldRegionGenerator.h"

#include "AutoMapPinBase.h"
#include "Components/SplineComponent.h"


// Sets default values
AAutoMapWorldRegionGenerator::AAutoMapWorldRegionGenerator(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.


	PrimaryActorTick.bCanEverTick = false;

	RegionSplineComponent = CreateDefaultSubobject<USplineComponent>("Region Spline Component");
	SetRootComponent(RegionSplineComponent);

	RegionSplineComponent->SetClosedLoop(true);
	RegionSplineComponent->SetHiddenInGame(true);
	RegionSplineComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RegionSplineComponent->Duration = 100;
	
	InitializeSpline(PinSpawnDensity);
}

#if WITH_EDITOR
void AAutoMapWorldRegionGenerator::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	InitializeSpline(PinSpawnDensity);
}
#endif

void AAutoMapWorldRegionGenerator::BeginDestroy()
{
	Super::BeginDestroy();

	if (!SpawnedWorldRegionsPins.IsEmpty())
	{
		for (auto SpawnedWorldRegionsPin : SpawnedWorldRegionsPins)
		{
			SpawnedWorldRegionsPin->Destroy();
		}
		SpawnedWorldRegionsPins.Empty();
	}
}

// Called when the game starts or when spawned
void AAutoMapWorldRegionGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAutoMapWorldRegionGenerator::InitializeSpline(float SpawnRate)
{
	if (RegionSplineComponent)
	{
		if (!SpawnedWorldRegionsPins.IsEmpty())
		{
			for (auto SpawnedWorldRegionsPin : SpawnedWorldRegionsPins)
			{
				SpawnedWorldRegionsPin->Destroy();
			}
			SpawnedWorldRegionsPins.Empty();
		}

		for (int i = 0; i < RegionSplineComponent->Duration; i = i +SpawnRate)
		{
			SpawnLocationPin(RegionSplineComponent->GetWorldLocationAtTime(i));
		}
	}
}

// Called every frame
void AAutoMapWorldRegionGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TArray<FName> AAutoMapWorldRegionGenerator::InitializeLocation()
{
	FDataTableRowHandle Location;
	Location.DataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *(FPaths::ProjectPluginsDir() / TEXT("/Script/Engine.DataTable'/AutoMap/Data/AutoMapRegions.AutoMapRegions'"))));
	return Location.DataTable->GetRowNames();
}

void AAutoMapWorldRegionGenerator::SpawnLocationPin(FVector SpawnLocation)
{
	if (UWorld* World = GetWorld())
	{
		FVector Location = SpawnLocation;
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;
		
		AAutoMapPinBase* SpawnedActor = World->SpawnActor<AAutoMapPinBase>(Location, Rotation, SpawnInfo);
		if (SpawnedActor)
		{
			SpawnedActor->PinType = EPinType::Location;
			SpawnedActor->SetActorScale3D(FVector(5, 5, 5));
			SpawnedActor->CurrentLocationNames = RegionID; 
			SpawnedActor->bShouldSnapToGround = true;
			SpawnedWorldRegionsPins.Add(SpawnedActor);
			
#if WITH_EDITOR
			SpawnedActor->SetFolderPath("/SpawnedActors/AutoMap/LocationPins");
#endif
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn actor"));
		}
	}
}

