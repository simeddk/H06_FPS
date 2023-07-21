#include "CLobbyGameMode.h"
#include "Global.h"
#include "CGameInstance.h"

void ACLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	++NumberOfPlayers;

	CLog::Print("Players : " + FString::FromInt(NumberOfPlayers));

	if (NumberOfPlayers >= 3)
	{
		CLog::Log("Reached 3 Players!");

		GetWorldTimerManager().SetTimer(GameStartTimer, this, &ACLobbyGameMode::StartGame, 10);
	}
}

void ACLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	--NumberOfPlayers;
}

void ACLobbyGameMode::StartGame()
{
	UCGameInstance* gameInstance = Cast<UCGameInstance>(GetGameInstance());
	CheckNull(gameInstance);
	gameInstance->StartSession();

	UWorld* world = GetWorld();
	CheckNull(world);

	bUseSeamlessTravel = true;
	world->ServerTravel("/Game/Maps/Play?listen");
}
