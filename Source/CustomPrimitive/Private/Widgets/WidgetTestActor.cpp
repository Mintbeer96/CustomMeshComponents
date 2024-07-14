// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetTestActor.h"

#include "GraphEditor.h"
#include "Widgets/Docking/SDockTab.h"
#define LOCTEXT_NAMESPACE "CSWeightEditor"

// Sets default values
AWidgetTestActor::AWidgetTestActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWidgetTestActor::BeginPlay()
{
	Super::BeginPlay();
	
}
// Called every frame
void AWidgetTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TSharedRef<SWidget> AWidgetTestActor::GenerateEditorWindowContent()
{
	TSharedRef<SWidget> Result = SNullWidget::NullWidget;
	
	return Result;
}



void AWidgetTestActor::Test()
{
	TSharedRef<SWindow> EditorWindow = SNew(SWindow)
		.Title(LOCTEXT("CSWeightEditorWindow.Title", "TestTest"))
		.SizingRule(ESizingRule::UserSized)
	/*
		/* The windows size fixed and cannot be resized 
		FixedSize,

		/** The window size is computed from its content and cannot be resized by users 
		Autosized,

		/** The window can be resized by users 
		UserSized,
	*/
		.ClientSize(FVector2D(1280.f, 720.f))
		.SupportsMaximize(true)
		.SupportsMinimize(true);
	
	TSharedRef<SWidget> Test = GenerateEditorWindowContent();
	EditorWindow->SetContent(Test);
	TSharedPtr<SWindow> RootWindow = FGlobalTabmanager::Get()->GetRootWindow();
	if (RootWindow.IsValid())
	{
		FSlateApplication::Get().AddWindowAsNativeChild(EditorWindow, RootWindow.ToSharedRef());
	}
	else
	{
		FSlateApplication::Get().AddWindow(EditorWindow);
	}
	return;
}

#undef LOCTEXT_NAMESPACE