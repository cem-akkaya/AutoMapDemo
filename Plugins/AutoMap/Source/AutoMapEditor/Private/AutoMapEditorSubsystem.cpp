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

		UAutoMapSubsystem* AutoMapSubsystem = World->GetGameInstance()->GetSubsystem<UAutoMapSubsystem>();
		AutoMapSubsystem->GetCoordinateSystem(bCoordinateSystemStatus, CurrentCoordinateScale, CurrentCoordinateSystemDimension, CurrentMapWorldOrigin);

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