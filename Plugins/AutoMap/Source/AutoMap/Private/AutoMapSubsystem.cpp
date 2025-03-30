// Fill out your copyright notice in the Description page of Project Settings.


#include "AutoMapSubsystem.h"

#include "AutoMapBoundsActor.h"
#include "AutoMapRenderer.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


void UAutoMapSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

}

void UAutoMapSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UAutoMapSubsystem::SnapshotMap()
{
	TArray<AActor*> OutActors;
	if (UWorld* World = GEditor->GetEditorWorldContext().World())
	{
		bool bCoordinateSystemStatus = false;
		float CurrentCoordinateScale = -1.0f;
		float CurrentCoordinateSystemDimension = -1.0f;
		FVector CurrentMapWorldOrigin = FVector::ZeroVector;
		
		GetCoordinateSystem(bCoordinateSystemStatus, CurrentCoordinateScale, CurrentCoordinateSystemDimension, CurrentMapWorldOrigin);

		if (bCoordinateSystemStatus)
		{
			FVector MidPoint = CurrentMapWorldOrigin;
			MidPoint.Z = SnapshotHeight; 
			
			FRotator Rotation(-90.0f, 0.0f, 0.0f);
			FActorSpawnParameters SpawnInfo;

			SnapshotCameraActor = World->SpawnActor<AAutoMapRenderer>(MidPoint, Rotation, SpawnInfo);
			SnapshotCamera = SnapshotCameraActor->GetComponentByClass<USceneCaptureComponent2D>();

			SnapshotCameraActor->PrepareRenderer(CoordinateSystemDimension, Resolution);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AutoMap-> No Maps Bound Actors Found, Place Bounds Actors First"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AutoMap-> No Map Found"));
	}
}

void UAutoMapSubsystem::InitCoordinateSystem(FVector InLocalCoordinatePlane)
{
	bool bCoordinateSystemStatus = false;
	float CurrentCoordinateScale = -1.0f;
	float CurrentCoordinateSystemDimension = -1.0f;
	FVector CurrentMapWorldOrigin = FVector::ZeroVector;

	LocalCoordinatePlane = InLocalCoordinatePlane;
	GetCoordinateSystem(bCoordinateSystemStatus, CurrentCoordinateScale, CurrentCoordinateSystemDimension, CurrentMapWorldOrigin);
}


void UAutoMapSubsystem::GetCoordinateSystem(bool& Status, float& CoordinateScales, float& CoordinateSystemDimensions,  FVector &MapWorldOrigin)
{
	TArray<AActor*> OutActors;
	Status = false;
	CoordinateScales = -1;
	CoordinateSystemDimensions = -1;
	MapWorldOrigin = FVector::ZeroVector;
	
	if (UWorld* World = GEditor->GetEditorWorldContext().World()){
		UGameplayStatics::GetAllActorsOfClass(World, AAutoMapBoundsActor::StaticClass(), OutActors);

		if (OutActors.Num() == 2)
		{
			auto RenderWidth = (1 / UKismetMathLibrary::Sqrt(2)) * UKismetMathLibrary::Vector_Distance(OutActors[0]->GetActorLocation(), OutActors[1]->GetActorLocation());

			FVector PointA = FVector(OutActors[0]->GetActorLocation().X, OutActors[0]->GetActorLocation().Y, OutActors[0]->GetActorLocation().Z);
			FVector PointB = FVector(OutActors[1]->GetActorLocation().X, OutActors[1]->GetActorLocation().Y,  OutActors[1]->GetActorLocation().Z);

			FVector MidPoint = (PointA + PointB) / 2.0f;
			
			// Calculate the dimensions of the rectangle based on Thales' theorem
			float DeltaX = PointB.X - PointA.X;
			float DeltaY = PointB.Y - PointA.Y;
			float DeltaZ = PointB.Z - PointA.Z;

			// Use Pythagorean theorem to calculate the sides
			float RectangleWidth = UKismetMathLibrary::Abs(DeltaX);
			float RectangleHeight = UKismetMathLibrary::Abs(DeltaY);
			float RectangleDepth = UKismetMathLibrary::Abs(DeltaZ);

			if (RectangleDepth == 0)
			{
				//Bypass ZDepth with minimal error to compansate
				RectangleDepth = 0.1f;
			}
			CoordinateScaleX = RectangleWidth;
			CoordinateScaleY = RectangleHeight;
			CoordinateScaleZ = RectangleDepth;
			
				
			// Get max as system dimension that will be used for render target and coordinate base
			CoordinateSystemDimension = UKismetMathLibrary::Max(RectangleWidth, RectangleHeight);
			CoordinateScale = RenderWidth/Resolution;

			CoordinateSystemWorldOrigin = (OutActors[0]->GetActorLocation() + OutActors[1]->GetActorLocation()) / 2.0f;
			
			Status = true;
			CoordinateScales = CoordinateScale;
			CoordinateSystemDimensions = CoordinateSystemDimension;
			MapWorldOrigin = MidPoint;
		}
	}

}

void UAutoMapSubsystem::ConvertWorldTransformToMapTransform(FTransform InWorldTransform, FTransform& OutMapTransform)
{

	FVector WorldLocation = InWorldTransform.GetLocation() + CoordinateSystemWorldOrigin;
	FVector MapLocation;

	MapLocation.X = WorldLocation.X/ (CoordinateScaleX / LocalCoordinatePlane.X);
	MapLocation.Y = WorldLocation.Y / (CoordinateScaleY / LocalCoordinatePlane.Y);
	MapLocation.Z = WorldLocation.Z / (CoordinateScaleZ / LocalCoordinatePlane.Z);

	OutMapTransform = InWorldTransform;
	
	OutMapTransform.SetLocation(MapLocation);
}

void UAutoMapSubsystem::ConvertWorldTransformToMapScreenTransform(FTransform InWorldTransform,
	FTransform& OutMapTransform, float& OutMapAngle)
{
	FVector WorldLocation = InWorldTransform.GetLocation() + CoordinateSystemWorldOrigin;
	FVector MapLocation;

	
	FVector ForwardVector = FVector(1.f, 0.f, 0.f);
FVector WorldRotation = InWorldTransform.GetRotation().Vector();

OutMapAngle = FMath::Fmod((FMath::Atan2(WorldRotation.Y, WorldRotation.X) * (180.0f / PI)) + 360.0f, 360.0f);
	FRotator MapRotation = UKismetMathLibrary::MakeRotFromXY(FVector::ForwardVector, WorldRotation);

	
	auto MaxScale = UKismetMathLibrary::Max(CoordinateScaleX, CoordinateScaleY);
	
	MapLocation.Y = -WorldLocation.X/ (MaxScale / UMGResolution) + UMGResolution/2;
	MapLocation.X = WorldLocation.Y / (MaxScale / UMGResolution)  + UMGResolution/2;
	MapLocation.Z = WorldLocation.Z / (1 / UMGResolution);


	OutMapTransform = InWorldTransform;
	
	OutMapTransform.SetLocation(MapLocation);
    OutMapTransform.SetRotation(MapRotation.Quaternion());
}

void UAutoMapSubsystem::GetAllPins(EPinType PinType, TArray<AAutoMapPinBase*>& OutPins)
{
	TArray<AAutoMapPinBase*> FoundPins;

	if (UWorld* World = GEditor->GetEditorWorldContext().World()){
		TArray<AActor*> TempActors;
		UGameplayStatics::GetAllActorsOfClass(World, AAutoMapPinBase::StaticClass(), TempActors);
		
		for (AActor* Actor : TempActors)
		{
			if (AAutoMapPinBase* Pin = Cast<AAutoMapPinBase>(Actor))
			{
				FoundPins.Add(Pin);
			}
		}

		if (!FoundPins.IsEmpty())
		{
			for (AAutoMapPinBase* FoundPin : FoundPins)
			{
				if (FoundPin->PinType == PinType)
				{
					OutPins.Add(FoundPin);
				}
			}

		}
	}

}

void UAutoMapSubsystem::GetAllLocationNames(EMapLocationCategory LocationTypeCategory, TArray<FName>& OutLocations){

	TArray<FName> Locations;
	if (AAutoMapPinBase* TempPin = NewObject<AAutoMapPinBase>())
	{
		Locations = TempPin->InitializeSubLocation(LocationTypeCategory);
		OutLocations.Append(Locations);
	}
}

FName UAutoMapSubsystem::ResolveRegionName(FName InputRegionName)
{
	FName ResolvedRegionName = InputRegionName;
	if (AAutoMapPinBase* TempPin = NewObject<AAutoMapPinBase>())
	{
		ResolvedRegionName = TempPin->ResolveRegionName(InputRegionName);
	}
	return ResolvedRegionName;
}

void UAutoMapSubsystem::GetLocationPins(FName LocationName, TArray<AAutoMapPinBase*>& OutPins)
{
	TArray<AAutoMapPinBase*> AllLocationPins;
	GetAllPins(Location, AllLocationPins);
	TArray<AAutoMapPinBase*> LocationPins;

	if (!AllLocationPins.IsEmpty()){
		for (AAutoMapPinBase* LocationPin : AllLocationPins)

			if (LocationPin->CurrentLocationNames == LocationName)
			{
				OutPins.Add(LocationPin);
			}
	}
}




