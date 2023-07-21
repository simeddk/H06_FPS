#include "CMainMenu.h"
#include "Global.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "CSessionRow.h"

UCMainMenu::UCMainMenu()
{
	CHelpers::GetClass(&SessionRowWidgetClass, "/Game/Widgets/WB_SessionRow");
}

bool UCMainMenu::Initialize()
{
	bool bSuccess = Super::Initialize();
	CheckFalseResult(bSuccess, false);

	CheckNullResult(HostButton, false);
	HostButton->OnClicked.AddDynamic(this, &UCMainMenu::OpenHostMenu);

	CheckNullResult(JoinButton, false);
	JoinButton->OnClicked.AddDynamic(this, &UCMainMenu::OpenJoinMenu);

	CheckNullResult(CancelJoinMenuButton, false);
	CancelJoinMenuButton->OnClicked.AddDynamic(this, &UCMainMenu::OpenMainMenu);

	CheckNullResult(ConfirmJoinMenuButton, false);
	ConfirmJoinMenuButton->OnClicked.AddDynamic(this, &UCMainMenu::JoinServer);

	CheckNullResult(CancelHostMenuButton, false);
	CancelHostMenuButton->OnClicked.AddDynamic(this, &UCMainMenu::OpenMainMenu);

	CheckNullResult(ConfirmHostMenuButton, false);
	ConfirmHostMenuButton->OnClicked.AddDynamic(this, &UCMainMenu::HostServer);

	CheckNullResult(QuitButton, false);
	QuitButton->OnClicked.AddDynamic(this, &UCMainMenu::QuitGame);

	return true;
}

void UCMainMenu::HostServer()
{
	CheckNull(OwingGameInstance);

	FString desiredSessionName = SessionNameField->GetText().ToString();
	OwingGameInstance->Host(desiredSessionName);
}

void UCMainMenu::JoinServer()
{
	if (!!OwingGameInstance && SessionRowIndex.IsSet())
	{
		CLog::Log("Session Row Index : " + SessionRowIndex.GetValue());

		OwingGameInstance->Join(SessionRowIndex.GetValue());
	}
	else
	{
		CLog::Log("Session Row Index is not set!!");
	}
	
}


void UCMainMenu::SetSessionList(TArray<FSessionData> InSessionDatas)
{
	UWorld* world = GetWorld();
	CheckNull(world);

	SessionList->ClearChildren();

	uint32 i = 0;
	for (const FSessionData& sessionData : InSessionDatas)
	{
		UCSessionRow* row = CreateWidget<UCSessionRow>(world, SessionRowWidgetClass);
		CheckNull(row);

		row->SessionName->SetText(FText::FromString(sessionData.Name));
		row->HostUserName->SetText(FText::FromString(sessionData.HostUserName));
		
		FString fraction = FString::Printf(TEXT("%d/%d"), sessionData.CurrentPlayers, sessionData.MaxPlayers);
		row->ConnectionFraction->SetText(FText::FromString(fraction));

		row->Setup(this, i++);

		SessionList->AddChild(row);
	}

}

void UCMainMenu::SetSessionRowIndex(uint32 InIndex)
{
	SessionRowIndex = InIndex;

	SelectSessionRow();
}


void UCMainMenu::SelectSessionRow()
{
	for (int32 i = 0; i < SessionList->GetChildrenCount(); i++)
	{
		UCSessionRow* rowWidget = Cast<UCSessionRow>(SessionList->GetChildAt(i));
		if (!!rowWidget)
			rowWidget->bSelected = (SessionRowIndex.IsSet() && SessionRowIndex.GetValue() == i);
	}
}

void UCMainMenu::OpenJoinMenu()
{
	CheckNull(MenuSwitcher);
	CheckNull(JoinMenu);

	MenuSwitcher->SetActiveWidget(JoinMenu);

	if (!!OwingGameInstance)
		OwingGameInstance->RefreshSessionList();
}

void UCMainMenu::OpenMainMenu()
{
	CheckNull(MenuSwitcher);
	CheckNull(MainMenu);

	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UCMainMenu::OpenHostMenu()
{
	CheckNull(MenuSwitcher);
	CheckNull(HostMenu);

	MenuSwitcher->SetActiveWidget(HostMenu);
}

void UCMainMenu::QuitGame()
{
	UWorld* world = GetWorld();
	CheckNull(world);

	APlayerController* controller = world->GetFirstPlayerController();
	CheckNull(controller);

	controller->ConsoleCommand("quit");
}

