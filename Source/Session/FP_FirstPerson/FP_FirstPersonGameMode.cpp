#include "FP_FirstPersonGameMode.h"
#include "Global.h"
#include "FP_FirstPersonHUD.h"
#include "FP_FirstPersonCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "FP_FirstPerson/FP_FirstPersonCharacter.h"
#include "Game/CGameState.h"
#include "Game/CPlayerState.h"

AFP_FirstPersonGameMode::AFP_FirstPersonGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/BP_CPlayer"));
	
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	HUDClass = AFP_FirstPersonHUD::StaticClass();
	GameStateClass = ACGameState::StaticClass();
	PlayerStateClass = ACPlayerState::StaticClass();
}

void AFP_FirstPersonGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ACPlayerState* playerState = Cast<ACPlayerState>(NewPlayer->PlayerState);
	AFP_FirstPersonCharacter* player = Cast<AFP_FirstPersonCharacter>(NewPlayer->GetPawn());

	if (!!playerState && !!player)
	{
		player->SetPlayerState(playerState);
		player->SetTeamColor(playerState->Team);
	}
}
