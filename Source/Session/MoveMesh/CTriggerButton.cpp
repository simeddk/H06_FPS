#include "CTriggerButton.h"
#include "Global.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "SessionCharacter.h"
#include "CMovingPlatform.h"

ACTriggerButton::ACTriggerButton()
{
	CHelpers::CreateSceneComponent(this, &Root, "Root");
	CHelpers::CreateSceneComponent(this, &Box, "Box", Root);
	CHelpers::CreateSceneComponent(this, &Mesh, "Mesh", Root);

	UStaticMesh* meshAsset;
	CHelpers::GetAsset(&meshAsset, "StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'");
	Mesh->SetStaticMesh(meshAsset);
	
	UMaterialInstanceConstant* materialAsset;
	CHelpers::GetAsset(&materialAsset, "MaterialInstanceConstant'/Game/Platforms/MAT_MovingPlatform_Red.MAT_MovingPlatform_Red'");
	Mesh->SetMaterial(0, materialAsset);

	Mesh->SetRelativeScale3D(FVector(0.7f, 0.7f, 0.05f));
	Mesh->SetRelativeLocation(FVector(0, 0, -30));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Box->SetHiddenInGame(false);
}

void ACTriggerButton::BeginPlay()
{
	Super::BeginPlay();
	
	OnActorBeginOverlap.AddDynamic(this, &ACTriggerButton::OnBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ACTriggerButton::OnEndOverlap);
}

void ACTriggerButton::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	ASessionCharacter* player = Cast<ASessionCharacter>(OtherActor);
	CheckNull(player);

	for (ACMovingPlatform* platform : Platforms)
		platform->AddActiveTrigger();
}

void ACTriggerButton::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	ASessionCharacter* player = Cast<ASessionCharacter>(OtherActor);
	CheckNull(player);

	for (ACMovingPlatform* platform : Platforms)
		platform->SubActiveTrigger();
}

