#include "CSpawnPoint.h"
#include "Global.h"
#include "Components/CapsuleComponent.h"

ACSpawnPoint::ACSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateSceneComponent(this, &Capsule, "Capsule");
}

void ACSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
	OnActorBeginOverlap.AddDynamic(this, &ACSpawnPoint::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ACSpawnPoint::EndOverlap);
}

void ACSpawnPoint::OnConstruction(const FTransform& Transform)
{
	FColor color = FColor(1, 1, 1);

	switch (Team)
	{
		case ETeamType::Red:	color = FColor(255, 0, 0);		break;
		case ETeamType::Blue:	color = FColor(0, 0, 255);		break;
		default:				color = FColor(255, 0, 0);
	}

	Capsule->ShapeColor = color;
}

void ACSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Capsule->UpdateOverlaps();
}

void ACSpawnPoint::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (OverlappingActors.Find(OtherActor) < 0)
			OverlappingActors.Add(OtherActor);
	}

}

void ACSpawnPoint::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (OverlappingActors.Find(OtherActor) >= 0)
			OverlappingActors.Remove(OtherActor);
	}
}

