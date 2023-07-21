#include "CGameMenu.h"
#include "Global.h"
#include "Components/Button.h"

bool UCGameMenu::Initialize()
{
	bool bSuccess = Super::Initialize();
	CheckFalseResult(bSuccess, false);

	CheckNullResult(CancelButton, false);
	CancelButton->OnClicked.AddDynamic(this, &UCGameMenu::CancelButtonPressed);

	CheckNullResult(QuitButton, false);
	QuitButton->OnClicked.AddDynamic(this, &UCGameMenu::QuitButtonPressed);
	
	return true;
}

void UCGameMenu::CancelButtonPressed()
{
	SetPlayMode();
}

void UCGameMenu::QuitButtonPressed()
{
	CheckNull(OwingGameInstance);

	SetPlayMode();
	OwingGameInstance->ReturnToMainMenu();
}
