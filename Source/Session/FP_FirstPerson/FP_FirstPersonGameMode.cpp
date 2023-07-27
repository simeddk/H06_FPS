#include "FP_FirstPersonGameMode.h"
#include "Global.h"
#include "FP_FirstPersonHUD.h"
#include "FP_FirstPersonCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "FP_FirstPerson/FP_FirstPersonCharacter.h"
#include "EngineUtils.h"
#include "Game/CGameState.h"
#include "Game/CPlayerState.h"
#include "Game/CSpawnPoint.h"

AFP_FirstPersonGameMode::AFP_FirstPersonGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

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
		//player->SetPlayerState(playerState);

		if (RedTeamCharacters.Num() > BlueTeamCharacters.Num())
		{
			BlueTeamCharacters.Add(player);
			playerState->Team = ETeamType::Blue;
		}
		else
		{
			RedTeamCharacters.Add(player);
			playerState->Team = ETeamType::Red;
		}

		player->CurrentTeam = playerState->Team;
		player->SetTeamColor(playerState->Team);

		//Host User�� ���
		UWorld* world = GetWorld();
		if (!!world)
		{
			APlayerController* hostController = world->GetFirstPlayerController();
			if (!!hostController)
			{
				AFP_FirstPersonCharacter* hostPlayer = Cast<AFP_FirstPersonCharacter>(hostController->GetPawn());
				if (!!hostPlayer)
					MoveToSpawnPoint(hostPlayer);
			}
		}

		//Guest User�� ���
		MoveToSpawnPoint(player);
	}
}

void AFP_FirstPersonGameMode::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();
	CheckNull(world);

	for (TActorIterator<ACSpawnPoint> iter(world); iter; ++iter)
	{
		if (iter->GetTeam() == ETeamType::Red)
			RedTeamSpawnPoints.Add(*iter);
		else
			BlueTeamSpawnPoints.Add(*iter);
	}

}

void AFP_FirstPersonGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ToBeSpawnCharacters.Num() > 0)
	{
		for (const auto& toBeSpawnCharacter : ToBeSpawnCharacters)
			MoveToSpawnPoint(toBeSpawnCharacter);
	}
}

void AFP_FirstPersonGameMode::MoveToSpawnPoint(AFP_FirstPersonCharacter* InPlayer)
{
	TArray<ACSpawnPoint*>* souldBeSpawnPoints;

	if (InPlayer->CurrentTeam == ETeamType::Red)
		souldBeSpawnPoints = &RedTeamSpawnPoints;
	else
		souldBeSpawnPoints = &BlueTeamSpawnPoints;

	for (const auto& souldBeSpawnPoint : *souldBeSpawnPoints)
	{
		//��������Ʈ�� ��ϵǾ� ���� �ʴٸ�
		if (souldBeSpawnPoint->IsBlocked() == false)
		{
			InPlayer->SetActorLocation(souldBeSpawnPoint->GetActorLocation());
			souldBeSpawnPoint->UpdateOverlaps();

			if (ToBeSpawnCharacters.Find(InPlayer) >= 0)
				ToBeSpawnCharacters.Remove(InPlayer);

			return;
		}

		//��������Ʈ�� ��ϵǾ� �ִٸ�
		if (ToBeSpawnCharacters.Find(InPlayer) < 0)
			ToBeSpawnCharacters.Add(InPlayer);
	}
}

void AFP_FirstPersonGameMode::Respawn(AFP_FirstPersonCharacter* InPlayer)
{
	AController* controller = InPlayer->GetController();

	//��Ʈ�ѷ� ����
	InPlayer->DetachFromControllerPendingDestroy();

	//�÷��̾� ĳ���͸� �ٽ� ����
	UWorld* world = GetWorld();
	CheckNull(world);

	AFP_FirstPersonCharacter* player = world->SpawnActor<AFP_FirstPersonCharacter>(DefaultPawnClass);
	CheckNull(player);

	//�ٽ� ������ �÷���� ����
	controller->Possess(player);

	//�÷��̾� ������Ʈ ����(��, ų, ���� ����)
	ACPlayerState* playerState = Cast<ACPlayerState>(controller->PlayerState);
	CheckNull(playerState);

	//player->SetSelfPlayerState(playerState);
	player->CurrentTeam = playerState->Team;
	player->SetTeamColor(playerState->Team);

	MoveToSpawnPoint(player);
}
