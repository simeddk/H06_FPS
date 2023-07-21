#include "CMovingPlatform.h"
#include "Global.h"
#include "Materials/MaterialInstanceConstant.h"

ACMovingPlatform::ACMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	//스태틱메시 컴포넌트 세팅
	UStaticMesh* meshAsset;
	CHelpers::GetAsset(&meshAsset, "StaticMesh'/Game/Geometry/Meshes/1M_Cube_Chamfer.1M_Cube_Chamfer'");
	GetStaticMeshComponent()->SetStaticMesh(meshAsset);
	
	UMaterialInstanceConstant* materialAsset;
	CHelpers::GetAsset(&materialAsset, "MaterialInstanceConstant'/Game/Platforms/MAT_MovingPlatform_Red.MAT_MovingPlatform_Red'");
	GetStaticMeshComponent()->SetMaterial(0, materialAsset);

	GetStaticMeshComponent()->SetRelativeScale3D(FVector(2.f, 2.f, 0.5f));

	//모빌리티 세팅
	SetMobility(EComponentMobility::Movable);
}

void ACMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	//서버의 이동값을 클라이언트에게 복제시키기
	if (HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}

	//시작 위치와 타깃 위치를 저장
	GlobalStartLocation = GetActorLocation();
	GlobalTargetLocation = GetTransform().TransformPosition(TargetLocation);
}

void ACMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//ActiveTrigger가 1 이상일 때만 아래의 이동 코드를 실행
	CheckTrue(ActiveTrigger < 1);

	//서버에서만
	if (HasAuthority())
	{
		//발판 이동
		FVector location = GetActorLocation(); //현재 발판의 위치

		float totalDistance = (GlobalStartLocation - GlobalTargetLocation).Size(); //시작 위치 ~ 타깃 위치까지의 거리
		float currentDistance = (GlobalStartLocation - location).Size(); //시작 위치 ~ 현재 발판까지의 거리

		if (currentDistance >= totalDistance) //시작 위치와 타깃 위치를 스와핑
		{
			FVector temp = GlobalStartLocation;
			GlobalStartLocation = GlobalTargetLocation;
			GlobalTargetLocation = temp;
		}

		FVector direction = (GlobalTargetLocation - GlobalStartLocation).GetSafeNormal(); //시작 위치가 타깃 위치를 바라보는 방향
		
		location += direction * Speed * DeltaTime; //속도를 누적
		SetActorLocation(location);
	}
}

void ACMovingPlatform::AddActiveTrigger()
{
	ActiveTrigger++;
}

void ACMovingPlatform::SubActiveTrigger()
{
	CheckFalse(ActiveTrigger > 0);
	
	ActiveTrigger--;
}
