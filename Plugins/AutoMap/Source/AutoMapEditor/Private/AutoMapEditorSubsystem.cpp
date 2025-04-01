// Fill out your copyright notice in the Description page of Project Settings.


#include "AutoMapEditorSubsystem.h"

#include "AutoMapBoundsActor.h"
#include "AutoMapSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void UAutoMapEditorSubsystem::SnapshotMap()
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

			SnapshotCameraActor = World->SpawnActor<AAutoMapEditorRenderer>(MidPoint, Rotation, SpawnInfo);
			SnapshotCameraActor->PrepareRenderer(CurrentCoordinateSystemDimension, Resolution);
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

void UAutoMapEditorSubsystem::PositionToCamera(AActor* InActor)
{
	FViewport* activeViewport = GEditor->GetActiveViewport();
	FEditorViewportClient* editorViewClient = (activeViewport != nullptr) ? (FEditorViewportClient*)activeViewport->GetClient() : nullptr;
	if( editorViewClient )
	{
		FVector EditorCameraLocation = editorViewClient->GetViewLocation();
		FRotator EditorCameraRotation = editorViewClient->GetViewRotation();
		FVector EditorCameraDirection = FRotationMatrix(EditorCameraRotation).GetUnitAxis(EAxis::X);
		GEditor->MoveActorInFrontOfCamera(*InActor, EditorCameraLocation, EditorCameraDirection);
	}
}

void UAutoMapEditorSubsystem::GetCoordinateSystem(bool& Status, float& CoordinateScales, float& CoordinateSystemDimensions,  FVector &MapWorldOrigin)
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

			
				
			// Get max as system dimension that will be used for render target and coordinate base
			auto CoordinateSystemDimension = UKismetMathLibrary::Max(RectangleWidth, RectangleHeight);
			auto CoordinateScale = RenderWidth/Resolution;

			auto CoordinateSystemWorldOrigin = (OutActors[0]->GetActorLocation() + OutActors[1]->GetActorLocation()) / 2.0f;
			
			Status = true;
			CoordinateScales = CoordinateScale;
			CoordinateSystemDimensions = CoordinateSystemDimension;
			MapWorldOrigin = MidPoint;
		}
	}

}