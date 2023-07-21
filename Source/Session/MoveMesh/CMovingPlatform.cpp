#include "CMovingPlatform.h"
#include "Global.h"
#include "Materials/MaterialInstanceConstant.h"

ACMovingPlatform::ACMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	//����ƽ�޽� ������Ʈ ����
	UStaticMesh* meshAsset;
	CHelpers::GetAsset(&meshAsset, "StaticMesh'/Game/Geometry/Meshes/1M_Cube_Chamfer.1M_Cube_Chamfer'");
	GetStaticMeshComponent()->SetStaticMesh(meshAsset);
	
	UMaterialInstanceConstant* materialAsset;
	CHelpers::GetAsset(&materialAsset, "MaterialInstanceConstant'/Game/Platforms/MAT_MovingPlatform_Red.MAT_MovingPlatform_Red'");
	GetStaticMeshComponent()->SetMaterial(0, materialAsset);

	GetStaticMeshComponent()->SetRelativeScale3D(FVector(2.f, 2.f, 0.5f));

	//�����Ƽ ����
	SetMobility(EComponentMobility::Movable);
}

void ACMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	//������ �̵����� Ŭ���̾�Ʈ���� ������Ű��
	if (HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}

	//���� ��ġ�� Ÿ�� ��ġ�� ����
	GlobalStartLocation = GetActorLocation();
	GlobalTargetLocation = GetTransform().TransformPosition(TargetLocation);
}

void ACMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//ActiveTrigger�� 1 �̻��� ���� �Ʒ��� �̵� �ڵ带 ����
	CheckTrue(ActiveTrigger < 1);

	//����������
	if (HasAuthority())
	{
		//���� �̵�
		FVector location = GetActorLocation(); //���� ������ ��ġ

		float totalDistance = (GlobalStartLocation - GlobalTargetLocation).Size(); //���� ��ġ ~ Ÿ�� ��ġ������ �Ÿ�
		float currentDistance = (GlobalStartLocation - location).Size(); //���� ��ġ ~ ���� ���Ǳ����� �Ÿ�

		if (currentDistance >= totalDistance) //���� ��ġ�� Ÿ�� ��ġ�� ������
		{
			FVector temp = GlobalStartLocation;
			GlobalStartLocation = GlobalTargetLocation;
			GlobalTargetLocation = temp;
		}

		FVector direction = (GlobalTargetLocation - GlobalStartLocation).GetSafeNormal(); //���� ��ġ�� Ÿ�� ��ġ�� �ٶ󺸴� ����
		
		location += direction * Speed * DeltaTime; //�ӵ��� ����
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
