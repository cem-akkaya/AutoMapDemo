// Fill out your copyright notice in the Description page of Project Settings.


#include "AutoMapEditorSubsystem.h"
#include "AutoMapSubsystem.h"

void UAutoMapEditorSubsystem::SnapshotMap()
{
	TArray<AActor*> OutActors;
	if (UWorld* World = GEditor->GetEditorWorldContext().World())
	{
		bool bCoordinateSystemStatus = false;
		float CurrentCoordinateScale = -1.0f;
		float CurrentCoordinateSystemDimension = -1.0f;
		FVector CurrentMapWorldOrigin = FVector::ZeroVector;

		UAutoMapSubsystem* AutoMapSubsystem = GEngine->GetEngineSubsystem<UAutoMapSubsystem>();
		AutoMapSubsystem->GetCoordinateSystem(bCoordinateSystemStatus, CurrentCoordinateScale, CurrentCoordinateSystemDimension, CurrentMapWorldOrigin);

		if (bCoordinateSystemStatus)
		{
			FVector MidPoint = CurrentMapWorldOrigin;
			MidPoint.Z = SnapshotHeight; 
			
			FRotator Rotation(-90.0f, 0.0f, 0.0f);
			FActorSpawnParameters SpawnInfo;

			SnapshotCameraActor = World->SpawnActor<AAutoMapRenderer>(MidPoint, Rotation, SpawnInfo);
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
