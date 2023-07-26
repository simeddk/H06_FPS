#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FP_FirstPersonGameMode.generated.h"

UCLASS(minimalapi)
class AFP_FirstPersonGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFP_FirstPersonGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	void MoveToSpawnPoint(class AFP_FirstPersonCharacter* InPlayer);

private:
	TArray<class AFP_FirstPersonCharacter*> RedTeamCharacters;
	TArray<class AFP_FirstPersonCharacter*> BlueTeamCharacters;
	TArray<class AFP_FirstPersonCharacter*> ToBeSpawnCharacters;

	TArray<class ACSpawnPoint*> RedTeamSpawnPoints;
	TArray<class ACSpawnPoint*> BlueTeamSpawnPoints;
};



