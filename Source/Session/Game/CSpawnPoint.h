#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CGameState.h"
#include "CSpawnPoint.generated.h"

UCLASS()
class SESSION_API ACSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	ACSpawnPoint();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
		void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		ETeamType Team;

private:
	UPROPERTY(VisibleDefaultsOnly)
		class UCapsuleComponent* Capsule;
};
