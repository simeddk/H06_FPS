#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "CMovingPlatform.generated.h"

UCLASS()
class SESSION_API ACMovingPlatform : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	ACMovingPlatform();

protected:
	virtual void BeginPlay() override;

private:
	virtual void Tick(float DeltaTime) override;

public:
	void AddActiveTrigger();
	void SubActiveTrigger();

private:
	UPROPERTY(EditAnywhere)
		float Speed = 60.f;

	UPROPERTY(EditAnywhere, meta = (MakeEditWidget))
		FVector TargetLocation = FVector(200, 0, 0);

	//if this value more than 1, this platform will moving
	UPROPERTY(EditInstanceOnly)
		int32 ActiveTrigger = 0;

private:
	FVector GlobalStartLocation;
	FVector GlobalTargetLocation;
};
