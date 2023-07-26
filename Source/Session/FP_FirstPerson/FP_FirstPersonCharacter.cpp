#include "FP_FirstPersonCharacter.h"
#include "Global.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "CBullet.h"
#include "Game/CSpawnPoint.h"
#include "Game/CPlayerState.h"

#define NotUse

void AFP_FirstPersonCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFP_FirstPersonCharacter, CurrentTeam);
}

AFP_FirstPersonCharacter::AFP_FirstPersonCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->SetRelativeLocation(FVector(0, 0, 64.f));
	Camera->bUsePawnControlRotation = true;
	
	FP_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	FP_Mesh->SetOnlyOwnerSee(true);
	FP_Mesh->SetupAttachment(Camera);
	FP_Mesh->bCastDynamicShadow = false;
	FP_Mesh->CastShadow = false;

	GetMesh()->SetOwnerNoSee(true);
	TP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_Gun"));
	TP_Gun->SetOwnerNoSee(true);
	TP_Gun->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));

	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;	
	FP_Gun->SetupAttachment(FP_Mesh, TEXT("GripPoint"));

	FP_GunShotParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FP_GunShotParticle"));
	FP_GunShotParticle->bAutoActivate = false;
	FP_GunShotParticle->SetupAttachment(FP_Gun, "Muzzle");
	FP_GunShotParticle->SetOnlyOwnerSee(true);

	TP_GunShotParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TP_GunShotParticle"));
	TP_GunShotParticle->bAutoActivate = false;
	TP_GunShotParticle->SetupAttachment(TP_Gun, "Muzzle");
	TP_GunShotParticle->SetOwnerNoSee(true);

	WeaponRange = 5000.0f;
	WeaponDamage = 500000.0f;

	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	CHelpers::GetClass(&BulletClass, "Blueprint'/Game/FirstPersonCPP/Blueprints/BP_CBullet.BP_CBullet_C'");
}

void AFP_FirstPersonCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	SelfPlayerState = Cast<ACPlayerState>(GetPlayerState());

	if (GetLocalRole() == ROLE_AutonomousProxy && !!SelfPlayerState)
		SelfPlayerState->Health = 100.f;
}


void AFP_FirstPersonCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFP_FirstPersonCharacter::OnFire);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &AFP_FirstPersonCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFP_FirstPersonCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFP_FirstPersonCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFP_FirstPersonCharacter::LookUpAtRate);
}

void AFP_FirstPersonCharacter::BeginPlay()
{
	Super::BeginPlay();

#ifndef NotUse
	UWorld* world = GetWorld();
	CheckNull(world);

	ACGameState* gameState = Cast<ACGameState>(world->GetGameState());
	if (!!gameState)
	{
		bool bRed = (gameState->TestTeam == ETeamType::Red);
		FColor textColor = bRed ? FColor::Red : FColor::Blue;

		CLog::Print("Found GameState : " + gameState->GetName(), -1, 5, textColor);
		CLog::Print("Number of Player : " +  FString::FromInt(gameState->PlayerArray.Num()));
	}
	else
	{
		CLog::Print("Not Found Game State");
	}
#endif

	if (HasAuthority() == false)
		SetTeamColor(CurrentTeam);

	UWorld* world = GetWorld();
	CheckNull(world);
	UGameplayStatics::GetAllActorsOfClass(world, ACSpawnPoint::StaticClass(), SpawnPoints);
}

void AFP_FirstPersonCharacter::OnFire()
{
	if (FireAnimation != NULL)
	{
		UAnimInstance* AnimInstance = FP_Mesh->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}

	if (!!FP_GunShotParticle)
		FP_GunShotParticle->Activate(true);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	
	FVector ShootDir = FVector::ZeroVector;
	FVector StartTrace = FVector::ZeroVector;

	if (PlayerController)
	{
		FRotator CamRot;
		PlayerController->GetPlayerViewPoint(StartTrace, CamRot);
		ShootDir = CamRot.Vector();

		StartTrace = StartTrace + ShootDir * ((GetActorLocation() - StartTrace) | ShootDir);
	}

	const FVector EndTrace = StartTrace + ShootDir * WeaponRange;

	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	AActor* DamagedActor = Impact.GetActor();
	UPrimitiveComponent* DamagedComponent = Impact.GetComponent();

	if ((DamagedActor != NULL) && (DamagedActor != this) && (DamagedComponent != NULL) && DamagedComponent->IsSimulatingPhysics())
	{
		DamagedComponent->AddImpulseAtLocation(ShootDir * WeaponDamage, Impact.Location);
	}

	OnServerFire(StartTrace, EndTrace);
}

void AFP_FirstPersonCharacter::OnServerFire_Implementation(const FVector& LineStart, const FVector& LineEnd)
{
	MulticastShootEffects();
}

void AFP_FirstPersonCharacter::MulticastShootEffects_Implementation()
{
	if (!!TP_FireAnimation)
	{
		UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
		if (!!animInstance)
		{
			animInstance->Montage_Play(TP_FireAnimation, 0.25f);
		}
	}

	if (!!FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	if (!!TP_GunShotParticle)
		TP_GunShotParticle->Activate(true);

	if (!!BulletClass)
		GetWorld()->SpawnActor<ACBullet>(BulletClass, FP_Gun->GetSocketLocation("Muzzle"), GetControlRotation());
}

void AFP_FirstPersonCharacter::SetTeamColor_Implementation(ETeamType InTeamType)
{
	FLinearColor color = FLinearColor(0, 0, 0);

	switch (InTeamType)
	{
		case ETeamType::Red:	color = FLinearColor(1, 0, 0);		break;
		case ETeamType::Blue:	color = FLinearColor(0, 0, 1);		break;
		default :				color = FLinearColor(1, 0, 0);
	}

	if (DynamicMaterial == nullptr)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), nullptr);
		DynamicMaterial->SetVectorParameterValue("BodyColor", color);

		GetMesh()->SetMaterial(0, DynamicMaterial);
		FP_Mesh->SetMaterial(0, DynamicMaterial);
	}
}

ACPlayerState* AFP_FirstPersonCharacter::GetSelfPlayerState()
{
	if (SelfPlayerState == nullptr)
		SelfPlayerState = Cast<ACPlayerState>(GetPlayerState());

	return SelfPlayerState;
}

void AFP_FirstPersonCharacter::SetSelfPlayerState(ACPlayerState* NewPlayerState)
{
	CheckFalse(HasAuthority());

	SetPlayerState(NewPlayerState);
	SelfPlayerState = NewPlayerState;
}

void AFP_FirstPersonCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AFP_FirstPersonCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFP_FirstPersonCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFP_FirstPersonCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

FHitResult AFP_FirstPersonCharacter::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const
{
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, GetInstigator());
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.AddIgnoredActors(SpawnPoints);

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_GameTraceChannel1, TraceParams);

	return Hit;
}