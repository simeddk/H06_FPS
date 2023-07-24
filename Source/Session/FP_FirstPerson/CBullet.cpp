#include "CBullet.h"
#include "Global.h"
#include "Materials/MaterialInstanceConstant.h"
#include "GameFramework/ProjectileMovementComponent.h"

ACBullet::ACBullet()
{
	CHelpers::CreateSceneComponent(this, &Mesh, "Mesh");
	
	CHelpers::CreateActorComponent(this, &Projectile, "Projectile");

	UStaticMesh* meshAsset;
	CHelpers::GetAsset(&meshAsset, "StaticMesh'/Engine/BasicShapes/Sphere.Sphere'");
	Mesh->SetStaticMesh(meshAsset);

	UMaterialInstanceConstant* materialAsset;
	CHelpers::GetAsset(&materialAsset, "MaterialInstanceConstant'/Game/FirstPersonCPP/Blueprints/MAT_Bullet_Inst.MAT_Bullet_Inst'");
	Mesh->SetMaterial(0, materialAsset);

	Mesh->SetRelativeScale3D(FVector(1.f, 0.025f, 0.025f));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->CastShadow = false;

	Projectile->ProjectileGravityScale = 0.f;
	Projectile->InitialSpeed = BulletSpeed;
	Projectile->MaxSpeed = BulletSpeed;
}

void ACBullet::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}
	
	SetLifeSpan(3.f);
}

