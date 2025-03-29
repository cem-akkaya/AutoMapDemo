// Fill out your copyright notice in the Description page of Project Settings.


#include "AutoMapProcessor.h"


// Sets default values
AAutoMapProcessor::AAutoMapProcessor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAutoMapProcessor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAutoMapProcessor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

