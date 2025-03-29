// Fill out your copyright notice in the Description page of Project Settings.


#include "AutoMapPinBase.h"

#include "AutoMapSubsystem.h"
#include "Components/TextRenderComponent.h"


// Sets default values
AAutoMapPinBase::AAutoMapPinBase(const FObjectInitializer& ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AActor::SetActorHiddenInGame(true);

	UStaticMeshComponent* DebugMesh = CreateDefaultSubobject<UStaticMeshComponent>("DebugMesh");
	SetRootComponent(DebugMesh);
	ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cone.Cone'"));
	DebugMesh->SetStaticMesh(SphereMeshAsset.Object);

	ConstructorHelpers::FObjectFinder<UMaterial>DebugMaterial(TEXT("Material'/Engine/MapTemplates/Materials/BasicAsset01.BasicAsset01'"));
	DebugMesh->SetMaterial(0, DebugMaterial.Object);
	DebugMesh->SetRelativeScale3D(FVector(10, 10, 10));
	DebugMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	
	UTextRenderComponent* DebugActorText = CreateDefaultSubobject<UTextRenderComponent>("DebugText");
	DebugActorText->SetupAttachment(GetRootComponent());
	DebugActorText->SetText(FText::FromString("Auto Map Pin Actor"));
	DebugActorText->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));

	DebugActorText->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
	DebugActorText->TextRenderColor = FColor::Orange;

	OnSpawn();
}

TArray<FName> AAutoMapPinBase::InitializeLocation()
{
	FDataTableRowHandle Location;
	Location.DataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *(FPaths::ProjectPluginsDir() / TEXT("/Script/Engine.DataTable'/AutoMap/Data/AutoMapRegions.AutoMapRegions'"))));
	return Location.DataTable->GetRowNames();
}

TArray<FName> AAutoMapPinBase::InitializeSubLocation(EMapLocationCategory LocationCategory)
{
	TArray<FName> MatchingRowNames;
	FDataTableRowHandle Location;
	Location.DataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *(FPaths::ProjectPluginsDir() / TEXT("/Script/Engine.DataTable'/AutoMap/Data/AutoMapRegions.AutoMapRegions'"))));
	if (Location.DataTable)
	{

		// Iterate through each row in the DataTable
		for (auto& Row : Location.DataTable->GetRowMap())
		{
			// Cast row data to your specific struct type (assuming FAutoMapRegion is the row struct type)
			FAutoMapRegionsStruct* RegionData = reinterpret_cast<FAutoMapRegionsStruct*>(Row.Value);
			if (RegionData)
			{
				// Check if the ProCat property matches the provided LocationCategory
				if (RegionData->LocationCategory == LocationCategory)
				{
					// Add the name of the matching row to the array
					MatchingRowNames.Add(Row.Key);
				}
			}
		}

		UE_LOG(LogTemp, Display, TEXT("AutoMap-> Found %d  Locations"), MatchingRowNames.Num());
	}
	return MatchingRowNames;
}

void AAutoMapPinBase::PositionToCamera()
{
	FViewport* activeViewport = GEditor->GetActiveViewport();
	FEditorViewportClient* editorViewClient = (activeViewport != nullptr) ? (FEditorViewportClient*)activeViewport->GetClient() : nullptr;
	if( editorViewClient )
	{
		FVector EditorCameraLocation = editorViewClient->GetViewLocation();
		FRotator EditorCameraRotation = editorViewClient->GetViewRotation();
		FVector EditorCameraDirection = FRotationMatrix(EditorCameraRotation).GetUnitAxis(EAxis::X);
		GEditor->MoveActorInFrontOfCamera(*this, EditorCameraLocation, EditorCameraDirection);
	}
	SnapToGround();
}

void AAutoMapPinBase::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);

	SnapToGround();
}

// Called when the game starts or when spawned
void AAutoMapPinBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAutoMapPinBase::OnSpawn()
{
	SnapToGround();
}

void AAutoMapPinBase::SnapToGround()
{
	if (bShouldSnapToGround)
	{
		FVector Start = GetActorLocation();
		Start.Z += 1000.0f; // Trace above the actor.
		FVector End = Start - FVector(0.0f, 0.0f, 11000.0f); // Trace downward up to 1000 units.

		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this); // Ignore this actor during the trace.

		// Perform a line trace to detect the ground
		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
		{
			SetActorLocation(HitResult.Location); // Snap actor to the hit location.
		}
	}
}

// Called every frame
void AAutoMapPinBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

