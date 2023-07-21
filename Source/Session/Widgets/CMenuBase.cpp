#include "CMenuBase.h"
#include "Global.h"

void UCMenuBase::SetOwingGameInstance(IIMenuInterface* InOwingInstance)
{
	OwingGameInstance = InOwingInstance;
}

void UCMenuBase::SetUIMode()
{
	AddToViewport();
	bIsFocusable = true;

	FInputModeUIOnly inputMode;
	inputMode.SetWidgetToFocus(TakeWidget());
	inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	UWorld* world = GetWorld();
	CheckNull(world);

	APlayerController* playerController = world->GetFirstPlayerController();
	CheckNull(playerController);

	playerController->SetInputMode(inputMode);
	playerController->bShowMouseCursor = true;
}

void UCMenuBase::SetPlayMode()
{
	RemoveFromViewport();
	bIsFocusable = false;

	FInputModeGameOnly inputMode;

	UWorld* world = GetWorld();
	CheckNull(world);

	APlayerController* playerController = world->GetFirstPlayerController();
	CheckNull(playerController);

	playerController->SetInputMode(inputMode);
	playerController->bShowMouseCursor = false;
}
