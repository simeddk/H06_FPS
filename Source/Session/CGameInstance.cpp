#include "CGameInstance.h"
#include "Global.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "Widgets/CMainMenu.h"
#include "Widgets/CMenuBase.h"

const static FName SESSION_NAME = TEXT("GameSession");
const static FName SESSION_SETTINGS_KEY = TEXT("SessionSettingsKey");

UCGameInstance::UCGameInstance(const FObjectInitializer& InObject)
{
	CLog::Log("CGameInstance Constructor Called");

	CHelpers::GetClass<UUserWidget>(&MainMenuWidgetClass, "/Game/Widgets/WB_MainMenu");
	CHelpers::GetClass<UUserWidget>(&GameMenuWidgetClass, "/Game/Widgets/WB_GameMenu");
}

void UCGameInstance::Init()
{
	CLog::Log("CGameInstance Init Called");

	IOnlineSubsystem* oss = IOnlineSubsystem::Get();
	if (!!oss)
	{
		CLog::Log("OSS Name : " + oss->GetSubsystemName().ToString());

		//Session Event Binding
		SessionInterface = oss->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UCGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UCGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UCGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UCGameInstance::OnJoinSessionComplete);
		}
	}
	else
	{
		CLog::Log("OSS Not Found");
	}

	if (!!GEngine)
		GEngine->OnNetworkFailure().AddUObject(this, &UCGameInstance::OnNetworkFailure);
}

void UCGameInstance::Host(const FString& InSessionName)
{
	DesiredSessionName = InSessionName;

	if (SessionInterface.IsValid())
	{
		//SessionName捞 吝汗牢 版快, 技记 昏力 -> 技记 犁积己
		auto exsistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if (!!exsistingSession)
		{
			SessionInterface->DestroySession(SESSION_NAME);
		}
		//技记 积己
		else
		{
			CreateSession();
		}
	}
}

void UCGameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings sessionSettings;

		if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
		{
			sessionSettings.bIsLANMatch = true;
		}
		else
		{
			sessionSettings.bIsLANMatch = false;
		}

		sessionSettings.NumPublicConnections = 4;
		sessionSettings.bShouldAdvertise = true;
		sessionSettings.bUsesPresence = true;
		sessionSettings.Set(SESSION_SETTINGS_KEY, DesiredSessionName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		SessionInterface->CreateSession(0, SESSION_NAME, sessionSettings);
	}
}

void UCGameInstance::Join(uint32 InIndex)
{
	CheckFalse(SessionInterface.IsValid());
	CheckFalse(SessionSearch.IsValid());

	if (!!MainMenu)
		MainMenu->SetPlayMode();

	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[InIndex]);
}

void UCGameInstance::LoadMainMenu()
{
	CheckNull(MainMenuWidgetClass);

	MainMenu = CreateWidget<UCMainMenu>(this, MainMenuWidgetClass);
	CheckNull(MainMenu);

	MainMenu->SetOwingGameInstance(this);
	MainMenu->SetUIMode();
}

void UCGameInstance::LoadGameMenu()
{
	CheckNull(GameMenuWidgetClass);

	UCMenuBase* gameMenu = CreateWidget<UCMenuBase>(this, GameMenuWidgetClass);
	CheckNull(gameMenu);

	gameMenu->SetOwingGameInstance(this);
	gameMenu->SetUIMode();
}

void UCGameInstance::ReturnToMainMenu()
{
	APlayerController* controller = GetFirstLocalPlayerController();
	CheckNull(controller);

	controller->ClientTravel("/Game/Maps/MainMenu", ETravelType::TRAVEL_Absolute);
}

void UCGameInstance::RefreshSessionList()
{
	//Find Session
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		CLog::Log("Starting Find Sessions");

		//SessionSearch->bIsLanQuery = true;
		SessionSearch->MaxSearchResults = 100;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void UCGameInstance::StartSession()
{
	if (SessionInterface.IsValid())
		SessionInterface->StartSession(SESSION_NAME);
}

void UCGameInstance::OnCreateSessionComplete(FName InSessionName, bool InSuccess)
{
	//技记捞 沥惑利栏肺 积己登菌绰瘤 眉农
	if (InSuccess == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Cound Not Create Session"));
		return;
	}

	//Lobby 甘栏肺 ServerTravel
	if (!!MainMenu)
		MainMenu->SetPlayMode();

	UWorld* world = GetWorld();
	CheckNull(world);

	world->ServerTravel("/Game/Maps/Lobby?listen");
}

void UCGameInstance::OnDestroySessionComplete(FName InSessionName, bool InSuccess)
{
	if (InSuccess == true)
		CreateSession();
}

void UCGameInstance::OnFindSessionComplete(bool InSuccess)
{
	if (InSuccess == true && SessionSearch.IsValid() && MainMenu != nullptr)
	{
		CLog::Log("Finished Find Sessions");

		TArray<FSessionData> sessionDatas;

		for (const auto& searchResult : SessionSearch->SearchResults)
		{
			CLog::Log("==<Find Session Result>==");
			CLog::Log(" -> SessionID : " + searchResult.GetSessionIdStr());
			CLog::Log(" -> Ping : " +  FString::FromInt(searchResult.PingInMs));

			FSessionData data;
			data.MaxPlayers = searchResult.Session.SessionSettings.NumPublicConnections;
			data.CurrentPlayers = data.MaxPlayers - searchResult.Session.NumOpenPublicConnections;
			data.HostUserName = searchResult.Session.OwningUserName;

			FString sessionName;
			if (searchResult.Session.SessionSettings.Get(SESSION_SETTINGS_KEY, sessionName))
			{
				data.Name = sessionName;
			}
			else
			{
				CLog::Log("Session Settings Key is not found");
			}

			sessionDatas.Add(data);
		}

		MainMenu->SetSessionList(sessionDatas);

	}
	
}

void UCGameInstance::OnJoinSessionComplete(FName InSessionName, EOnJoinSessionCompleteResult::Type InResult)
{
	CheckFalse(SessionInterface.IsValid());

	FString address;
	if (SessionInterface->GetResolvedConnectString(SESSION_NAME, address) == false)
	{
		CLog::Log("Could not get IP Address");
		return;
	}

	CLog::Print("Join to " + address);

	APlayerController* controller = GetFirstLocalPlayerController();
	CheckNull(controller);
	controller->ClientTravel(address, ETravelType::TRAVEL_Absolute);
}

void UCGameInstance::OnNetworkFailure(UWorld* InWorld, UNetDriver* InNetDriver, ENetworkFailure::Type InFaliureType, const FString& OutErrorMessge)
{
	CLog::Print(OutErrorMessge);

	ReturnToMainMenu();
}
