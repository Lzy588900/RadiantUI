// Copyright 2014 Joseph Riedel, Inc. All Rights Reserved.
// See LICENSE for licensing terms.

#include "RadiantUIPrivatePCH.h"

ARadiantWebViewHUD::ARadiantWebViewHUD(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true; 
	//bShowDebugInfo = true;
	//bShowHitBoxDebugInfo = true;
}

void ARadiantWebViewHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	bool bUnused;
	UWorld* World = GetWorldChecked(bUnused);

	for (auto It = HUDElements.CreateConstIterator(); It; ++It)
	{
		if (*It)
		{
			URadiantWebViewHUDElement *Element = NewObject<URadiantWebViewHUDElement>((UObject*)GetTransientPackage(), *It);
			if (Element)
			{
				Element->WorldPrivate = World;
				HUDElementInstances.Add(Element);
			}
		}
	}
}

void ARadiantWebViewHUD::DrawHUD()
{
	Super::DrawHUD();

	PreDrawHUD();

	const FVector2D ViewportSize(Canvas->SizeX, Canvas->SizeY);

	for (auto It = HUDElementInstances.CreateConstIterator(); It; ++It)
	{
		DrawHUDElement(ViewportSize, *It);
	}

	PostDrawHUD();
}

void ARadiantWebViewHUD::Tick(float InDeltaSeconds)
{
	Super::Tick(InDeltaSeconds);

	float RealTime = FPlatformTime::Seconds() - GStartTime;
	float WorldTime = GetWorld()->GetTimeSeconds();

	for (auto It = HUDElementInstances.CreateConstIterator(); It; ++It)
	{
		(*It)->WebView->Tick(RealTime, WorldTime, InDeltaSeconds, GetWorld()->FeatureLevel);
	}
}

void ARadiantWebViewHUD::BeginPlay()
{
	check(GEngine->GameViewport);

	Super::BeginPlay();

	int ZOrder = 0;

	for (auto It = HUDElementInstances.CreateConstIterator(); It; ++It)
	{
		URadiantWebViewHUDElement* Element = *It;

		Element->WebView->SetNetMode(GetNetMode());
		Element->WebView->Start();

		SAssignNew(Element->SWidget, SRadiantWebViewHUDElement).HUDOwner(this).HUDElement(Element);
		GEngine->GameViewport->AddViewportWidgetContent(SAssignNew(Element->Container, SWeakWidget).PossiblyNullContent(Element->SWidget.ToSharedRef()), ZOrder++);

		Element->SetSlateVisibility();
	}
	/*
	InputComponent = NewObject<UInputComponent>(this, UInputComponent::StaticClass(), TEXT("HUD_InputComponent0"));
	if (InputComponent)
	{
		InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ARadiantWebViewHUD::OnLeftMouseButtonClick);
		InputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &ARadiantWebViewHUD::OnLeftMouseButtonRelease);
		InputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &ARadiantWebViewHUD::OnRightMouseButtonClick);
		InputComponent->BindKey(EKeys::RightMouseButton, IE_Released, this, &ARadiantWebViewHUD::OnRightMouseButtonRelease);

		InputComponent->BindKey(EKeys::Escape, IE_Released, this, &ARadiantWebViewHUD::OnEscape);
		InputComponent->BindKey(EKeys::LeftAlt, IE_Released, this, &ARadiantWebViewHUD::OnEscape);
	}
	
	*/
}
/*

void ARadiantWebViewHUD::OnLeftMouseButtonClick()
{
	float PosX, PosY;
	APlayerController *Controller = GetOwningPlayerController();
	Controller->GetMousePosition(PosX, PosY);

	for (auto It = HUDElementInstances.CreateConstIterator(); It; ++It)
	{
		CefRuntimeMouseEvent Event;
		Event.X = PosX;
		Event.Y = PosY;
		(*It)->WebView->GetBrowser()->SendMouseClickEvent(Event, CEFRT_MouseLeft, false, 1);
	}
}

void ARadiantWebViewHUD::OnLeftMouseButtonRelease()
{
	float PosX, PosY;
	APlayerController *Controller = GetOwningPlayerController();
	Controller->GetMousePosition(PosX, PosY);

	for (auto It = HUDElementInstances.CreateConstIterator(); It; ++It)
	{
		CefRuntimeMouseEvent Event;
		Event.X = PosX;
		Event.Y = PosY;
		(*It)->WebView->GetBrowser()->SendMouseClickEvent(Event, CEFRT_MouseLeft, true, 1);
		(*It)->ForceFocus(Controller);
	}
}

void ARadiantWebViewHUD::OnRightMouseButtonClick()
{
	float PosX, PosY;
	APlayerController *Controller = GetOwningPlayerController();
	Controller->GetMousePosition(PosX, PosY);

	for (auto It = HUDElementInstances.CreateConstIterator(); It; ++It)
	{
		CefRuntimeMouseEvent Event;
		Event.X = PosX;
		Event.Y = PosY;
		(*It)->WebView->GetBrowser()->SendMouseClickEvent(Event, CEFRT_MouseRight, false, 1);
	}
}

void ARadiantWebViewHUD::OnRightMouseButtonRelease()
{
	float PosX, PosY;
	APlayerController *Controller = GetOwningPlayerController();
	Controller->GetMousePosition(PosX, PosY);

	for (auto It = HUDElementInstances.CreateConstIterator(); It; ++It)
	{
		CefRuntimeMouseEvent Event;
		Event.X = PosX;
		Event.Y = PosY;
		(*It)->WebView->GetBrowser()->SendMouseClickEvent(Event, CEFRT_MouseRight, true, 1);
		(*It)->ForceFocus(Controller);
	}
}

void ARadiantWebViewHUD::OnEscape()
{
	APlayerController *Controller = GetOwningPlayerController();
	Controller->bShowMouseCursor = false;
	Controller->SetCinematicMode(false, false, false, true, true);

	FInputModeGameOnly InputMode;
	Controller->SetInputMode(InputMode);

	Controller->PopInputComponent(InputComponent);
}

void ARadiantWebViewHUD::NotifyHitBoxClick(FName BoxName)
{
	Super::NotifyHitBoxClick(BoxName);

	UE_LOG(RadiantUILog, Warning, TEXT("NotifyHitBoxClick!!!!! %s"), *BoxName.ToString());

	float PosX, PosY;
	APlayerController *Controller = GetOwningPlayerController();
	Controller->GetMousePosition(PosX, PosY);

	for (auto It = HUDElementInstances.CreateConstIterator(); It; ++It)
	{
		if (((*It)->GetName() + "HitBox") == BoxName.ToString())
		{
			CefRuntimeMouseEvent Event;
			Event.X = PosX;
			Event.Y = PosY;
			(*It)->WebView->GetBrowser()->SendMouseClickEvent(Event, CEFRT_MouseLeft, false, 1);
			(*It)->ForceFocus(Controller);
			break;
		}
	}
	Controller->bShowMouseCursor = false;
	Controller->bEnableClickEvents = false;
	Controller->SetCinematicMode(false, false, false, true, true);
	Controller->EnableInput(Controller);

}

void ARadiantWebViewHUD::NotifyHitBoxRelease(FName BoxName)
{
	Super::NotifyHitBoxRelease(BoxName);

	UE_LOG(RadiantUILog, Warning, TEXT("NotifyHitBoxRelease!!!!! %s"), *BoxName.ToString());

	float PosX, PosY;
	APlayerController *Controller = GetOwningPlayerController();
	Controller->GetMousePosition(PosX, PosY);

	for (auto It = HUDElementInstances.CreateConstIterator(); It; ++It)
	{
		if (((*It)->GetName() + "HitBox") == BoxName.ToString())
		{
			CefRuntimeMouseEvent Event;
			Event.X = PosX;
			Event.Y = PosY;
			(*It)->WebView->GetBrowser()->SendMouseClickEvent(Event, CEFRT_MouseLeft, true, 1);
			(*It)->ForceFocus(Controller);
			break;
		}
	}

	Controller->bShowMouseCursor = false;
	Controller->bEnableClickEvents = false;
	Controller->SetCinematicMode(false, false, false, true, true);
	Controller->EnableInput(Controller);

}

*/
void ARadiantWebViewHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (auto It = HUDElementInstances.CreateConstIterator(); It; ++It)
	{
		URadiantWebViewHUDElement* Element = *It;

		Element->WebView.Reset();
		Element->MarkPendingKill();
	}

	HUDElementInstances.SetNum(0);

	Super::EndPlay(EndPlayReason);
}

void ARadiantWebViewHUD::DrawHUDElement(const FVector2D& ViewportSize, URadiantWebViewHUDElement* InElement)
{
	if (!InElement->SWidget.IsValid())
	{
		return;
	}

	FRadiantWebView* WebView = InElement->WebView.Get();

	FVector2D ItemPosition = ViewportSize * InElement->Position;
	FVector2D ItemSize = ViewportSize * InElement->Size;

	ItemPosition.X = FMath::FloorToFloat(ItemPosition.X + 0.5f);
	ItemPosition.Y = FMath::FloorToFloat(ItemPosition.Y + 0.5f);
	ItemSize.X = FMath::FloorToFloat(ItemSize.X + 0.5f);
	ItemSize.Y = FMath::FloorToFloat(ItemSize.Y + 0.5f);

	InElement->SWidget->ScreenPosition = ItemPosition;
	InElement->SWidget->ScreenSize = ItemSize;

	if (InElement->bAutoMatchViewportResolution)
	{
		WebView->Resize(FIntPoint(FMath::FloorToInt((ItemSize.X*InElement->ViewportResolutionFactor.X) + 0.5f), FMath::FloorToInt((ItemSize.Y*InElement->ViewportResolutionFactor.Y) + 0.5f)));
	}

	if (InElement->bVisible && WebView->HasInitialFrame() && WebView->WebViewCanvas && WebView->WebViewCanvas->RenderTargetTexture)
	{
		FCanvasTileItem TileItem(ItemPosition, WebView->WebViewCanvas->RenderTargetTexture->Resource, ItemSize, FLinearColor::White);
		TileItem.BlendMode = WebView->IsTransparentRendering() ? SE_BLEND_Translucent : SE_BLEND_Opaque;
		Canvas->DrawItem(TileItem);

 		//FName HBName = FName(*(InElement->GetName() + TEXT("HitBox")));
		//AHUD::AddHitBox(ItemPosition, ItemSize, HBName, false);
	}
}

void ARadiantWebViewHUD::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	static const int ArchiveVersion = 1;

	int Version = ArchiveVersion;
	Ar << Version;
}

/*bool ARadiantWebViewHUD::HandleKeyDown(const FKey& Key)
{
	bool handled = false;
	if (TriggerKeys.Contains(Key))
	{
		handled = OnTriggerKeyDown(Key);
		if (!handled)
		{
			Received_OnTriggerKeyDown(Key, handled);
		}
	}
	
	return true;
}*/