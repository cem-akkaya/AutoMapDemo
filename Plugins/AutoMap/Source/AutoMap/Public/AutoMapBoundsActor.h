// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AutoMapBoundsActor.generated.h"

UCLASS()
class AUTOMAP_API AAutoMapBoundsActor : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the actor's properties, including transform, are updated in the editor
	virtual void PostEditMove(bool bFinished) override;
	
public:
	// Sets default values for this actor's properties
	explicit AAutoMapBoundsActor(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(CallInEditor, Category="Auto Map" , DisplayName="Set To Camera View")
	void PositionToCamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void AlignBoundsToCoordinateSystem();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
