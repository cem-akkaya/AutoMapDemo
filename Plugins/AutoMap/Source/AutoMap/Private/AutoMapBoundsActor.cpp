// Fill out your copyright notice in the Description page of Project Settings.


#include "AutoMapBoundsActor.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAutoMapBoundsActor::AAutoMapBoundsActor(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	AActor::SetActorHiddenInGame(true);

	UStaticMeshComponent* DebugMesh = CreateDefaultSubobject<UStaticMeshComponent>("DebugMesh");
	SetRootComponent(DebugMesh);
	ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	DebugMesh->SetStaticMesh(SphereMeshAsset.Object);

	ConstructorHelpers::FObjectFinder<UMaterial>DebugMaterial(TEXT("Material'/Engine/MapTemplates/Materials/BasicAsset03.BasicAsset03'"));
	DebugMesh->SetMaterial(0, DebugMaterial.Object);

	UTextRenderComponent* DebugActorText = CreateDefaultSubobject<UTextRenderComponent>("DebugText");
	DebugActorText->SetupAttachment(GetRootComponent());
	DebugActorText->SetText(FText::FromString("Auto Map Bounds Actor"));
	DebugActorText->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	DebugActorText->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
	DebugActorText->TextRenderColor = FColor::Red;

	AlignBoundsToCoordinateSystem();
}

void AAutoMapBoundsActor::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);

	AlignBoundsToCoordinateSystem();
}

void AAutoMapBoundsActor::PositionToCamera()
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
	AlignBoundsToCoordinateSystem();
}

// Called when the game starts or when spawned
void AAutoMapBoundsActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAutoMapBoundsActor::AlignBoundsToCoordinateSystem()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAutoMapBoundsActor::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0){
		for (AActor* FoundActor : FoundActors)
		{
			if (FoundActor != this)
			{
				FoundActor->SetActorLocation(FVector(-1 * this->GetActorLocation().X, -1 * this->GetActorLocation().Y, this->GetActorLocation().Z));
			}
		}
	
	}
		
}

// Called every frame
void AAutoMapBoundsActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

