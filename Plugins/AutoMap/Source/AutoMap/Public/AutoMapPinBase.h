// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AutoMapPinBase.generated.h"

UENUM(BlueprintType)
enum EPinType
{
	Location UMETA(DisplayName = "Location Pin"),
	Objective   UMETA(DisplayName = "Objective Pin"),
	Waypoint    UMETA(DisplayName = "Waypoint Pin"),
	Direction      UMETA(DisplayName = "Direction Pin"),
};

UCLASS()
class AUTOMAP_API AAutoMapPinBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAutoMapPinBase(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(CallInEditor, Category="Auto Map" , DisplayName="Set To Camera View")
	void PositionToCamera();

	virtual void PostEditMove(bool bFinished) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Auto Map - Pin Definations")
	TEnumAsByte<EPinType> PinType = EPinType::Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Auto Map - Pin Definations")
	FName PinName = "Pin Name";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Auto Map - Pin Usability")
	bool bShouldSnapToGround = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnSpawn();

	void SnapToGround();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
