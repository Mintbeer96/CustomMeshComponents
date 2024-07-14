// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WidgetTestActor.generated.h"

UCLASS()
class CUSTOMPRIMITIVE_API AWidgetTestActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWidgetTestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(CallInEditor)
	void Test();
	
	TSharedRef<SWidget> GenerateEditorWindowContent();
};
